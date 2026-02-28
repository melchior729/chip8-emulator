/// @file main.cpp
/// @brief Handles the SDL3 output
/// @author Abhay Manoj
/// @date Feb 27 2026
#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

static constexpr int SCALING_FACTOR = 16;
static constexpr int WINDOW_HEIGHT = Chip8::HEIGHT * SCALING_FACTOR;
static constexpr int WINDOW_WIDTH = Chip8::WIDTH * SCALING_FACTOR;

/// @brief contains the window, renderer, and cpu
struct AppState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  Chip8 cpu;
};

/// @brief initializes SDL, the window and the renderer
/// @param appstate contains the current appstate
/// @param argc the number of arguments
/// @param argv the arguments
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  AppState *state = new AppState();

  if (!SDL_CreateWindowAndRenderer("Abhay's Chip8 Emulator", WINDOW_WIDTH,
                                   WINDOW_HEIGHT, 0, &state->window,
                                   &state->renderer)) {
    delete state;
    return SDL_APP_FAILURE;
  }

  *appstate = state;
  return SDL_APP_CONTINUE;
}

/// @brief handles the outcome of events, like exits, and keyboard inputs
/// @param appstate contains the current appstate
/// @param event the event that has occurred
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

/// @brief draws the content of the display buffer to the SDL window
/// @param appstate contains the current appstate
static void draw_to_screen(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);
  // const auto &display_buffer = state->cpu.get_display_buffer();

  std::array<uint8_t, 64 * 32> display_buffer{};
  display_buffer.fill(0);

  // Solid 10x6 rectangle at top-left
  for (int y = 0; y < 6; ++y) {
    for (int x = 0; x < 10; ++x) {
      display_buffer[y * 64 + x] = 1;
    }
  }

  // Diagonal line starting at (20, 10)
  for (int k = 0; k < 5; ++k) {
    int x = 20 + k;
    int y = 10 + k;
    display_buffer[y * 64 + x] = 1;
  }

  SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (size_t i = 0; i < Chip8::HEIGHT; i++) {
    for (size_t j = 0; j < Chip8::WIDTH; j++) {
      if (!display_buffer[j + i * Chip8::WIDTH]) {
        continue;
      }

      SDL_FRect pixel(j * Chip8::WIDTH * SCALING_FACTOR,
                      i * Chip8::HEIGHT * SCALING_FACTOR, SCALING_FACTOR,
                      SCALING_FACTOR);

      SDL_RenderRect(state->renderer, &pixel);
    }
  }
}

/// @brief performs one 'tick' of the application
/// @param appstate contains the current appstate
SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);
  SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 0xFF);
  SDL_RenderClear(state->renderer);
  draw_to_screen(appstate);
  SDL_RenderPresent(state->renderer);
  return SDL_APP_CONTINUE;
}

/// @brief exits the application and does cleanup
/// @param appstate contains the current appstate
/// @param result the result code that terminated the app
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate == nullptr) {
    SDL_Quit();
    return;
  }

  AppState *state = static_cast<AppState *>(appstate);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  delete state;

  SDL_Quit();
}
