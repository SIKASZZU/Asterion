#include <cmath>
#include <algorithm>

#include "daylight.hpp"
#include "game.hpp"


Daylight daylightSettings;

namespace DaylightNS {

    float timeOfDay = 0.0f;

    void update_daynight(Uint64 elapsedMS) {
        if (!daylightSettings.daylightEnabled) return;
        if (daylightSettings.dayLengthSeconds <= 0.0f) return;
        // advance time by elapsedMS relative to full day length
        timeOfDay += static_cast<float>(elapsedMS) / (daylightSettings.dayLengthSeconds * 1000.0f);
        if (timeOfDay >= 1.0f) timeOfDay = fmod(timeOfDay, 1.0f);
        daylightSettings.day = (get_day_brightness() >= 0.5f);
    }

    float get_day_brightness() {
        // Map timeOfDay (0..1) to hours (0..24)
        float hours = fmod(timeOfDay * 24.0f, 24.0f);
        // Night transition window: from 21:00 to 08:00 (next day) -> length 11 hours
        // Compute hours elapsed since 21:00 in range [0,24)
        float since21 = hours - 21.0f;
        if (since21 < 0.0f) since21 += 24.0f;
        float brightness = 1.0f;
        const float nightWindow = 11.0f;
        const float halfWindow = nightWindow / 2.0f; // 5.5h
        if (since21 < nightWindow) {
            // within darkness transition: from 21 -> dimming to minimum at 21+halfWindow,
            // then brightening to full by 08:00
            if (since21 <= halfWindow) {
                // dimming from 1.0 -> 0.0
                brightness = 1.0f - (since21 / halfWindow);
            }
            else {
                // brightening from 0.0 -> 1.0
                brightness = (since21 - halfWindow) / halfWindow;
            }
        }
        else {
            // daytime between 08:00 and 21:00
            brightness = 1.0f;
        }

        return std::clamp(brightness, 0.0f, 1.0f);
    }

    void debug(SDL_Renderer* renderer) {
        // Day/night debug status
        int hours = static_cast<int>(timeOfDay * 24.0f) % 24;
        int minutes = static_cast<int>((timeOfDay * 24.0f - hours) * 60.0f);
        char buf[96];
        snprintf(buf, sizeof(buf), "daylight: %s  Time: %02d:%02d  Cycle: %.0fs", daylightSettings.daylightEnabled ? "On" : "Off",
            hours, minutes, daylightSettings.dayLengthSeconds);
        SDL_RenderDebugText(renderer, 50, 150, buf);
        char buf2[64];
        snprintf(buf2, sizeof(buf2), "Brightness: %.2f", DaylightNS::get_day_brightness());
        SDL_RenderDebugText(renderer, 50, 170, buf2);
        char bufDay[32];
        snprintf(bufDay, sizeof(bufDay), "day: %s", daylightSettings.day ? "true" : "false");
        SDL_RenderDebugText(renderer, 50, 190, bufDay);
    }

    void draw(SDL_Renderer* renderer) {
        // Draw day/night overlay: tint the final framebuffer according to timeOfDay
        if (daylightSettings.daylightEnabled) {
            // brightness oscillates with cosine: 1.0 = full day, 0.0 = full night
            // get centralized brightness (1=day,0=night), respects inversion
            float brightness = get_day_brightness();
            // compute alpha for night tint (0..maxAlpha)
            const float maxNightAlpha = 200.0f; // how dark the night can get
            float nightAlpha = (1.0f - brightness) * maxNightAlpha;
            if (nightAlpha > 0.5f) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                // bluish night tint
                Uint8 a = static_cast<Uint8>(std::clamp(nightAlpha, 0.0f, 255.0f));
                SDL_SetRenderDrawColor(renderer, 12, 24, 64, a);
                SDL_FRect full = { 0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight) };
                SDL_RenderFillRect(renderer, &full);
            }
        }
    }
}
