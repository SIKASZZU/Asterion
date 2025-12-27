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
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

namespace Raycast {
    SDL_FPoint sourcePos = {};
    SDL_FPoint lastComputedSource = {};
    bool showRays = false;
    signed int maxActiveSize = ((renderRadius / tileSize) * (renderRadius / tileSize) * PI);
    signed int maxDecaySize = maxActiveSize / 2;
    float maxRayLength = renderRadius * (tileSize * 0.75);
    bool updateMaxGridSize = true;
    /// endpointActiveGrids.size() >= maxActiveSize
    std::set<std::pair<int, int>> endpointActiveGrids;
    // CONSTEXPRES activeGridsMaxSize.size() == maxActiveSize
    std::set<std::pair<int, int>> activeGridsMaxSize;
    std::deque<std::pair<int, int>> decayGrids;

    // Worker thread primitives
    static std::thread workerThread;
    static std::thread::id workerThreadId;
    static std::mutex workerMutex;
    static std::condition_variable workerCv;
    static std::atomic<bool> computeRequested{ false };
    static std::atomic<bool> computeReady{ false };
    static std::atomic<bool> stopWorker{ false };
    static std::vector<SDL_FPoint> rayEndpoints;

    void update_sourcePos() {
        sourcePos = {
            player.x,
            player.y
        };
    }
    float to_radians(float degrees) {
        return degrees * (PI / 180.0f);
    }
    // Computes direction vector from an angle
    SDL_FPoint angle_to_direction(float angleDeg) {
        float angleRad = to_radians(angleDeg);
        return {
            cosf(angleRad),
            sinf(angleRad)
        };
    }
    // Precomputed direction & step cache for fixed-angle fans
    struct PrecompDir { SDL_FPoint dir; SDL_FPoint unitStep; };
    static std::vector<PrecompDir> dirCache;
    static void ensure_dir_cache() {
        if (!dirCache.empty()) return;
        int entries = 360 / angleStep;
        dirCache.reserve(entries + 1);
        for (int a = 0; a < 360; a += angleStep) {
            SDL_FPoint d = angle_to_direction(static_cast<float>(a));
            SDL_FPoint us;
            // avoid division by zero by using very large step value
            us.x = (d.x == 0.0f) ? 1e30f : fabsf(1.0f / d.x);
            us.y = (d.y == 0.0f) ? 1e30f : fabsf(1.0f / d.y);
            dirCache.push_back({ d, us });
        }
    }

    // Trace ray with provided precomputed unit steps (fast path)
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
            localEndpoints.insert(std::make_pair(gridY, gridX));
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
                localEndpoints.insert(std::make_pair(gridY, gridX));
                break;
            }
        }
        return distance;
    }
    // Compute ray endpoints and endpointActiveGrids without any rendering (worker thread)
    void calculate_active_grids(const SDL_FPoint& srcPos, int map[mapSize][mapSize]) {
        ensure_dir_cache();
        std::set<std::pair<int, int>> localEndpoints;
        std::vector<SDL_FPoint> localEnds;
        localEnds.reserve(dirCache.size() + 1);
        int idx = 0;
        for (int angle = 0; angle < 360; angle += angleStep) {
            const SDL_FPoint& direction = dirCache[idx].dir;
            const SDL_FPoint& unitStep = dirCache[idx].unitStep;
            float calculated_length = trace_ray(srcPos, map, direction, unitStep, localEndpoints);
            SDL_FPoint end = {
                srcPos.x + direction.x * calculated_length,
                srcPos.y + direction.y * calculated_length
            };
            localEnds.push_back(end);
            ++idx;
        }
        // swap into shared structures under lock
        {
            std::lock_guard<std::mutex> lk(workerMutex);
            endpointActiveGrids.swap(localEndpoints);
            lastComputedSource = srcPos;
            rayEndpoints.swap(localEnds);
        }
    }
    void calculate_decay_grids() {
        // todo: decay grids setist ei removei mingeid random v22rtusi 2ra, just mazeis olles
        // Testi sedasi, et visionis renderi valged "active" rectid enne "decay" recte ning ss mine mazei.
        // hetkel ongi nii, et renderib decay ss active ehk active overwritib. Fix the overlap!
        std::set_difference(
            activeGridsMaxSize.begin(), activeGridsMaxSize.end(),
            endpointActiveGrids.begin(), endpointActiveGrids.end(),
            std::inserter(decayGrids, decayGrids.begin())
        );
        activeGridsMaxSize = endpointActiveGrids;
    }
    void clear_arrays() {
        std::lock_guard<std::mutex> lk(workerMutex);
        endpointActiveGrids.clear();
        rayEndpoints.clear();
        while (decayGrids.size() >= maxDecaySize) {
            decayGrids.pop_back();
        }
    }
    void update_max_grid_size() {
        if (!updateMaxGridSize) return;
        updateMaxGridSize = false;
        maxActiveSize = (renderRadius * renderRadius * PI);
        maxDecaySize = maxActiveSize / 2;
    }
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[mapSize][mapSize]) {
        if (!r_pressed) return;
        // light source position is published to worker inside request_calculation()
        update_max_grid_size();

        // Ask worker to compute; do not block unless we need to render
        request_calculation();

        if (showRays) {
            // wait for worker result to be ready, then render on main thread
            wait_until_ready();
            render_rays(renderer, offset);
            calculate_decay_grids();
        }
    }

    // Worker loop
    static void worker_loop() {
        while (!stopWorker.load()) {
            // wait for request
            std::unique_lock<std::mutex> lk(workerMutex);
            workerCv.wait(lk, [] { return computeRequested.load() || stopWorker.load(); });
            if (stopWorker.load()) break;
            // take the request (clear the flag) and capture sourcePos under lock
            computeRequested.store(false);
            SDL_FPoint localSrc = sourcePos;
            lk.unlock();

            // compute
            update_max_grid_size();
            // compute into local containers and swap inside calculate_active_grids
            calculate_active_grids(localSrc, map);

            // signal ready
            {
                std::lock_guard<std::mutex> lk2(workerMutex);
                computeReady.store(true);
            }
            workerCv.notify_all();
        }
    }

    void start_worker() {
        stopWorker.store(false);
        computeRequested.store(false);
        computeReady.store(false);
        workerThread = std::thread(worker_loop);
        workerThreadId = workerThread.get_id();
    }

    void stop_worker() {
        stopWorker.store(true);
        workerCv.notify_all();
        if (workerThread.joinable()) workerThread.join();
    }

    void request_calculation() {
        std::lock_guard<std::mutex> lk(workerMutex);
        // publish the current player position for the worker and request recompute
        sourcePos = { player.x, player.y };
        computeReady.store(false);
        computeRequested.store(true);
        workerCv.notify_one();
    }

    bool is_ready() {
        return computeReady.load();
    }

    void wait_until_ready() {
        std::unique_lock<std::mutex> lk(workerMutex);
        workerCv.wait(lk, [] { return computeReady.load() || stopWorker.load(); });
    }

    std::thread::id get_worker_id() {
        return workerThreadId;
    }

    void render_rays(SDL_Renderer* renderer, struct Offset& offset) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
        std::lock_guard<std::mutex> lk(workerMutex);
        SDL_FPoint isoStart = to_isometric_coordinate(lastComputedSource.x, lastComputedSource.y);
        for (const auto& end : rayEndpoints) {
            SDL_FPoint isoEnd = to_isometric_coordinate(end.x, end.y);
            SDL_RenderLine(renderer, isoStart.x + (tileSize / 2), isoStart.y, isoEnd.x + (tileSize / 2), isoEnd.y);
        }
    }
}