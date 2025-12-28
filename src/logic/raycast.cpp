#include "map.hpp"
#include "player.hpp"
#include "raycast.hpp"
#include "isometric_calc.hpp"
#include "game.hpp"
#include "vision.hpp"
#include "offset.hpp"

#include <SDL3/SDL.h>
#include <iostream>
#include <set>
#include <deque>
#include <utility>
#include <cmath>
#include <algorithm>
#include <thread>
#include <stop_token>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

namespace Raycast {
    bool enabled = false;
    bool showRays = false;
    SDL_FPoint sourcePos = {};
    SDL_FPoint lastComputedSource = {};
    signed int maxActiveSize = ((renderRadius / tileSize) * (renderRadius / tileSize) * PI);
    signed int maxDecaySize = maxActiveSize / 2;
    float maxRayLength = renderRadius * (tileSize * 0.75);
    bool updateMaxGridSize = true;
    std::set<std::pair<int, int>> endpointActiveGrids;
    std::set<std::pair<int, int>> activeGridsMaxSize;
    std::deque<std::pair<int, int>> decayGrids;

    // Worker thread primitives
    static std::jthread workerThread;
    static std::mutex workerMutex;
    static std::condition_variable workerCv;
    static std::atomic<bool> computeRequested{ false };
    static std::atomic<bool> computeReady{ false };
    static std::vector<SDL_FPoint> rayEndpoints;

    void update_sourcePos() {
        sourcePos = { player.x, player.y };
    }

    float to_radians(float degrees) {
        return degrees * (PI / 180.0f);
    }

    SDL_FPoint angle_to_direction(float angleDeg) {
        const float angleRad = to_radians(angleDeg);
        return { cosf(angleRad), sinf(angleRad) };
    }

    // Precomputed direction & step cache for fixed-angle fans
    struct PrecompDir { SDL_FPoint dir; SDL_FPoint unitStep; };
    static std::vector<PrecompDir> dirCache;

    static void ensure_dir_cache() {
        if (!dirCache.empty()) return;
        const int entries = 360 / angleStep;
        dirCache.reserve(entries + 1);
        for (int a = 0; a < 360; a += angleStep) {
            const SDL_FPoint d = angle_to_direction(static_cast<float>(a));
            SDL_FPoint us;
            us.x = (d.x == 0.0f) ? 1e30f : fabsf(1.0f / d.x);
            us.y = (d.y == 0.0f) ? 1e30f : fabsf(1.0f / d.y);
            dirCache.push_back({ d, us });
        }
    }

    // Trace ray with provided precomputed unit steps
    static float trace_ray(const SDL_FPoint& srcPos, int map[mapSize][mapSize], const SDL_FPoint& direction, const SDL_FPoint& rayUnitStep, std::set<std::pair<int, int>>& localEndpoints) {
        int gridX = static_cast<int>(srcPos.x / tileSize);
        int gridY = static_cast<int>(srcPos.y / tileSize);

        SDL_Point step;
        SDL_FPoint rayLength1D;
        if (direction.x < 0) {
            step.x = -1;
            rayLength1D.x = (srcPos.x - (gridX * tileSize)) / tileSize * rayUnitStep.x;
        }
        else {
            step.x = 1;
            rayLength1D.x = ((gridX + 1) * tileSize - srcPos.x) / tileSize * rayUnitStep.x;
        }

        if (direction.y < 0) {
            step.y = -1;
            rayLength1D.y = (srcPos.y - (gridY * tileSize)) / tileSize * rayUnitStep.y;
        }
        else {
            step.y = 1;
            rayLength1D.y = ((gridY + 1) * tileSize - srcPos.y) / tileSize * rayUnitStep.y;
        }

        float distance = 0.0f;
        while (distance < maxRayLength) {
            localEndpoints.insert({ gridY, gridX });
            if (rayLength1D.x < rayLength1D.y) {
                gridX += step.x;
                distance = rayLength1D.x * tileSize;
                rayLength1D.x += rayUnitStep.x;
            }
            else {
                gridY += step.y;
                distance = rayLength1D.y * tileSize;
                rayLength1D.y += rayUnitStep.y;
            }
            if (wallValues.find(map[gridY][gridX]) != wallValues.end()) {
                localEndpoints.insert({ gridY, gridX });
                break;
            }
        }
        return distance;
    }

    // Compute ray endpoints and endpointActiveGrids (worker thread)
    void calculate_active_grids(const SDL_FPoint& srcPos) {
        ensure_dir_cache();
        std::set<std::pair<int, int>> localEndpoints;
        std::vector<SDL_FPoint> localEnds;
        localEnds.reserve(dirCache.size() + 1);
        size_t idx = 0;
        for (int angle = 0; angle < 360; angle += angleStep) {
            const auto& [direction, unitStep] = dirCache[idx];
            const float calculated_length = trace_ray(srcPos, map, direction, unitStep, localEndpoints);
            const SDL_FPoint end = {
                srcPos.x + direction.x * calculated_length,
                srcPos.y + direction.y * calculated_length
            };
            localEnds.push_back(end);
            ++idx;
        }
        // Swap into shared structures under lock
        {
            std::lock_guard<std::mutex> lk(workerMutex);
            endpointActiveGrids.swap(localEndpoints);
            rayEndpoints.swap(localEnds);
            lastComputedSource = srcPos;
        }
    }

    void calculate_decay_grids() {

        // see func fucking laagab raigelt, ilmselt
        // 1. maxDecaySize too big?
        // 2. retarded arvutused?
        return;

        std::vector<std::pair<int, int>> added;
        {
            std::lock_guard<std::mutex> lk(workerMutex);
            added.reserve(activeGridsMaxSize.size());
            std::set_difference(
                activeGridsMaxSize.begin(), activeGridsMaxSize.end(),
                endpointActiveGrids.begin(), endpointActiveGrids.end(),
                std::back_inserter(added)
            );
            activeGridsMaxSize.swap(endpointActiveGrids);  // O(1) swap instead of O(N) copy
        }
        // Append to decay queue (newest at front)
        for (auto& p : added) {
            decayGrids.push_front(p);
        }
        // Trim
        while (static_cast<int>(decayGrids.size()) > maxDecaySize) {
            decayGrids.pop_back();
        }
    }

    void clear_arrays() {
        std::lock_guard<std::mutex> lk(workerMutex);
        endpointActiveGrids.clear();
        rayEndpoints.clear();
        while (static_cast<int>(decayGrids.size()) > maxDecaySize) {
            decayGrids.pop_back();
        }
    }

    void update_max_grid_size() {
        if (!updateMaxGridSize) return;
        updateMaxGridSize = false;
        maxActiveSize = (renderRadius * renderRadius * PI);
        maxDecaySize = maxActiveSize / 2;
    }

    void update(SDL_Renderer* renderer, struct Offset& Offset) {
        if (!enabled) return;
        update_max_grid_size();
        SDL_FPoint currentPos = { player.x, player.y };
        float distMoved = hypotf(currentPos.x - lastComputedSource.x, currentPos.y - lastComputedSource.y);
        if (distMoved > tileSize * 0.5f) {  // Adjust threshold as needed
            request_calculation();
            wait_until_ready();
            calculate_decay_grids();
        }
        if (showRays) {
            render_rays(renderer, offset);
        }
    }

    // Worker loop
    static void worker_loop(std::stop_token st) {
        while (!st.stop_requested()) {
            std::unique_lock<std::mutex> lock(workerMutex);
            workerCv.wait(lock, [&] { return computeRequested.load() || st.stop_requested(); });
            if (st.stop_requested()) break;
            computeRequested.store(false);
            const SDL_FPoint localSrc = sourcePos;
            lock.unlock();

            // Compute
            update_max_grid_size();
            calculate_active_grids(localSrc);

            // Signal ready
            {
                std::lock_guard<std::mutex> lock2(workerMutex);
                computeReady.store(true);
            }
            workerCv.notify_all();
        }
    }

    std::jthread::id start_worker() {
        computeRequested.store(false);
        computeReady.store(false);
        workerThread = std::jthread(worker_loop);
        return workerThread.get_id();
    }

    void stop_worker() {
        if (workerThread.joinable()) {
            workerThread.request_stop();
            workerCv.notify_all();
            workerThread.join();
        }
    }

    void request_calculation() {
        std::lock_guard<std::mutex> lk(workerMutex);
        sourcePos = { player.x, player.y };
        computeReady.store(false);
        computeRequested.store(true);
        workerCv.notify_one();
    }

    void wait_until_ready() {
        std::unique_lock<std::mutex> lk(workerMutex);
        workerCv.wait(lk, [] { return computeReady.load(); });
    }

    void render_rays(SDL_Renderer* renderer, struct Offset& offset) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
        std::vector<SDL_FPoint> localEndpoints;
        SDL_FPoint localSource;
        {
            std::lock_guard<std::mutex> lk(workerMutex);  // Protect read
            localEndpoints = rayEndpoints;  // Copy to avoid holding lock during render
            localSource = lastComputedSource;
        }
        const SDL_FPoint isoStart = to_isometric_coordinate(localSource.x, localSource.y);
        for (const auto& end : localEndpoints) {
            const SDL_FPoint isoEnd = to_isometric_coordinate(end.x, end.y);
            SDL_RenderLine(renderer, isoStart.x + (tileSize / 2), isoStart.y, isoEnd.x + (tileSize / 2), isoEnd.y);
        }
    }
}