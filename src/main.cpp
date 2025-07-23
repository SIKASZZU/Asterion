#define SDL_MAIN_HANDLED

#include <ctime>
#include <SDL2/SDL_image.h>

#include "game.hpp"
#include "player.hpp"
#include "textures.hpp"
#include "render.hpp"
#include "collision.hpp"
#include "shader.hpp"

int main(int argc, char* argv[]) {
    SDL_SetMainReady();  // compiler ja windows bitching. Yritab muidu SDL maini kasutada
    Offset offset = { 0, 0 };

    generate_map();
    // print_map(map);

    srand(static_cast<unsigned int>(std::time(NULL)));  // random numbrite jaoks

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Asterion",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    int win_width = 0, win_height = 0;
    SDL_GetWindowSize(window, &win_width, &win_height);

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glewInit();

    // shaders, full‐screen quad setup
    float quadVerts[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // need pathid on atm borken, sest Makefile ei includei shader folderi
    Shader lightShader("shaders/vert.glsl", "shaders/frag.glsl");
    float radius = 0.3f;


    // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // load_textures(renderer);

    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    /* game loop */
    while (isRunning) {
        frame_start = SDL_GetTicks();  // framerate
        SDL_GetMouseState(&mouse_x, &mouse_y);  // a 32-bit button bitmask of the current button state. Mis tra asi see on?

        Uint32 elapsed_ticks = frame_start - previous_tick;
        previous_tick = frame_start;
        tick_lag += elapsed_ticks;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
                break;
            }
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                react_to_keyboard_down(key, player, offset, map);
            }
            else if (event.type == SDL_KEYUP) {
                SDL_Keycode key = event.key.keysym.sym;
                react_to_keyboard_up(key, player);
            }
        }

        react_to_keyboard_state(state, player);  // movement vectori slow downimiseks on vaja seda funci alati callida 

        // compute normalized light pos from mouse
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_FPoint lightPos = { (mx / float(win_width)) * 2.0f - 1.0f, 
            1.0f - (my / float(win_height)) * 2.0f };

        frame_count++;
        while (tick_lag > tick_delay) {

            /* ticki sees peavad olema k6ik, mis fpsiga muutuks kiiremaks
            ja annaks eelise vs teise m2ngija suhtes. */

            // update tickrate
            tick_count++;
            tick_lag -= tick_delay;

            // /* Render begin*/
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            /* Clear shader */
            glViewport(0, 0, win_width, win_height);
            // Now, before your GL draw:
            // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Clear with blue!

            // update_offset(offset, player, window);

            // load_render(renderer, offset, player);
            // update_player(map, offset, state, renderer);

            // // enne millegi renderimist, peab lisama rendererile colori.
            // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            // // dispay new frame
            // SDL_RenderPresent(renderer);

            // shader
            lightShader.use();
            std::cout << "normalised: " << lightPos.x << " " << lightPos.y << "\n";
            lightShader.setVec2("uLight", lightPos.x, lightPos.y);
            lightShader.setFloat("uRadius", radius);


            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            // Present
            SDL_GL_SwapWindow(window);

            // /* Framerate and tickrate updating */
            if (SDL_GetTicks() - fps_timer >= 1000) {
                fps = frame_count * 1000.0f / (frame_start - fps_timer);
                ticks_per_second = tick_count * 1000.0f / (frame_start - tick_timer);

                std::cout << fps << " fps, " << ticks_per_second << " tps" << std::endl;

                fps_timer = SDL_GetTicks();
                tick_timer = SDL_GetTicks();
                frame_count = 0;
                tick_count = 0;
            }
        }

        /* Framerate cap */
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < 16) { SDL_Delay(16 - frame_time); }
    }

    // Cleanup if isRunning == false

    /* shader delete */
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
    SDL_GL_DeleteContext(glContext);
    destroy_all_textures();
    IMG_Quit();

    // SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
