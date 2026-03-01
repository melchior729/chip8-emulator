/// @file main.cpp
/// @brief Handles the SDL3 output
/// @author Abhay Manoj
/// @date Feb 27 2026
#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <fstream>
#include <memory>

static constexpr size_t SCALING_FACTOR = 16;
static constexpr size_t WINDOW_HEIGHT = Chip8::HEIGHT * SCALING_FACTOR;
static constexpr size_t WINDOW_WIDTH = Chip8::WIDTH * SCALING_FACTOR;

/// @brief contains the window, renderer, and cpu
struct AppState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  std::unique_ptr<Chip8> cpu;
};

/// @brief loads the rom from the system into the appstate
/// @param appstate contains the current appstate
/// @param file_path the path of the rom to load
SDL_AppResult load_rom(void *appstate, const std::string &file_path) {
  AppState *state = static_cast<AppState *>(appstate);
  std::ifstream file(file_path, std::ios::binary);
  if (!file) {
    return SDL_APP_FAILURE;
  }

  auto rom = std::make_unique<std::array<uint8_t, Chip8::MEMORY_SIZE>>();
  file.read(reinterpret_cast<char *>(rom->data()), rom->size());
  state->cpu->load_into_memory(*rom);
  return SDL_APP_CONTINUE;
}

/// @brief initializes SDL, the window and the renderer
/// @param appstate contains the current appstate
/// @param argc the number of arguments
/// @param argv the arguments
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  constexpr int NUM_ARGS = 2;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  if (argc != NUM_ARGS) {
    SDL_Log("Usage: %s <rom path>", argv[0]);
    return SDL_APP_FAILURE;
  }

  AppState *state = new AppState();
  state->cpu = std::make_unique<Chip8>();
  *appstate = state;

  if (!SDL_CreateWindowAndRenderer("Abhay's Chip8 Emulator", WINDOW_WIDTH,
                                   WINDOW_HEIGHT, 0, &state->window,
                                   &state->renderer)) {
    delete state;
    *appstate = nullptr;
    return SDL_APP_FAILURE;
  }

  if (load_rom(*appstate, argv[1]) != SDL_APP_CONTINUE) {
    return SDL_APP_FAILURE;
  }

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
  const auto &display_buffer = state->cpu->get_display_buffer();
  std::array<SDL_FRect, Chip8::WIDTH * Chip8::HEIGHT> pixels;
  size_t pixel_count = 0;

  for (size_t i = 0; i < Chip8::HEIGHT; i++) {
    size_t offset = i * Chip8::WIDTH;
    for (size_t j = 0; j < Chip8::WIDTH; j++) {
      if (display_buffer[j + offset]) {
        pixels[pixel_count++] =
            SDL_FRect(j * SCALING_FACTOR, i * SCALING_FACTOR, SCALING_FACTOR,
                      SCALING_FACTOR);
      }
    }
  }

  SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(state->renderer, pixels.data(), pixel_count);
}

/// @brief performs one 'tick' of the application
/// @param appstate contains the current appstate
SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);
  SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(state->renderer);
  draw_to_screen(appstate);
  state->cpu->cycle();
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
