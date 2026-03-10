/// @file main.cpp
/// @brief Handles the SDL3 output
/// @author Abhay Manoj
/// @date Feb 27 2026

#define SDL_MAIN_USE_CALLBACKS 1
#include "core/chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>

static constexpr size_t SCALING_FACTOR = 16;
static constexpr size_t WINDOW_HEIGHT = Chip8::HEIGHT * SCALING_FACTOR;
static constexpr size_t WINDOW_WIDTH = Chip8::WIDTH * SCALING_FACTOR;
static constexpr size_t FRAME_RATE = 60;
static constexpr size_t SAMPLE_RATE = 48000;
static constexpr size_t SAMPLES = SAMPLE_RATE / FRAME_RATE;
static constexpr size_t CYCLES_PER_FRAME = 10;

/// @brief contains the window, renderer, and cpu
struct AppState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  float audio_data[SAMPLES];
  SDL_AudioStream *stream = nullptr;
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

/// @brief initializes the audio for the appstate
/// @param appstate contains the current appstate
bool setup_audio(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);
  SDL_AudioSpec spec;
  spec.format = SDL_AUDIO_F32;
  spec.channels = 1;
  spec.freq = SAMPLE_RATE;
  state->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                            &spec, nullptr, nullptr);

  if (state->stream == NULL) {
    return false;
  }

  int phase = 0;
  for (size_t i = 0; i < SAMPLES; i++) {
    float t = (float)phase / SAMPLE_RATE;
    state->audio_data[i] =
        fmod(t * Chip8::FREQUENCY, 1.0f) < 0.5f ? 0.3f : -0.3f;
    phase++;
  }

  return true;
}

/// @brief initializes SDL, the window and the renderer
/// @param appstate contains the current appstate
/// @param argc the number of arguments
/// @param argv the arguments
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  constexpr int NUM_ARGS = 2;

  if (argc != NUM_ARGS) {
    SDL_Log("Usage: %s <rom path>", argv[0]);
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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

  if (!setup_audio(*appstate)) {
    return SDL_APP_FAILURE;
  }

  SDL_SetRenderVSync(state->renderer, 1);
  return SDL_APP_CONTINUE;
}

/// @brief handles the outcome of events, like exits, and keyboard inputs
/// @param appstate contains the current appstate
/// @param event the event that has occurred
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (appstate == nullptr) {
    return SDL_APP_CONTINUE;
  }

  AppState *state = static_cast<AppState *>(appstate);
  auto &cpu = state->cpu;

  if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
    bool is_pressed = event->type == SDL_EVENT_KEY_DOWN;
    switch (event->key.key) {
    case SDLK_1:
      cpu->set_keypad(1, is_pressed);
      break;
    case SDLK_2:
      cpu->set_keypad(2, is_pressed);
      break;
    case SDLK_3:
      cpu->set_keypad(3, is_pressed);
      break;
    case SDLK_4:
      cpu->set_keypad(0xC, is_pressed);
      break;
    case SDLK_Q:
      cpu->set_keypad(4, is_pressed);
      break;
    case SDLK_W:
      cpu->set_keypad(5, is_pressed);
      break;
    case SDLK_E:
      cpu->set_keypad(6, is_pressed);
      break;
    case SDLK_R:
      cpu->set_keypad(0xD, is_pressed);
      break;
    case SDLK_A:
      cpu->set_keypad(7, is_pressed);
      break;
    case SDLK_S:
      cpu->set_keypad(8, is_pressed);
      break;
    case SDLK_D:
      cpu->set_keypad(9, is_pressed);
      break;
    case SDLK_F:
      cpu->set_keypad(0xE, is_pressed);
      break;
    case SDLK_Z:
      cpu->set_keypad(0xA, is_pressed);
      break;
    case SDLK_X:
      cpu->set_keypad(0, is_pressed);
      break;
    case SDLK_C:
      cpu->set_keypad(0xB, is_pressed);
      break;
    case SDLK_V:
      cpu->set_keypad(0xF, is_pressed);
      break;
    }
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
        pixels[pixel_count++] = {(float)j * SCALING_FACTOR,
                                 (float)i * SCALING_FACTOR,
                                 (float)SCALING_FACTOR, (float)SCALING_FACTOR};
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
  uint8_t delay_register = state->cpu->get_DT();
  uint8_t sound_register = state->cpu->get_ST();

  if (delay_register > 0) {
    state->cpu->set_DT(delay_register - 1);
  }

  if (sound_register != 0 && state->stream != nullptr) {
    SDL_PutAudioStreamData(state->stream, state->audio_data,
                           sizeof(state->audio_data));
    state->cpu->set_ST(state->cpu->get_ST() - 1);
  }

  for (size_t i = 0; i < CYCLES_PER_FRAME; i++) {
    state->cpu->cycle();
  }
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
  SDL_DestroyAudioStream(state->stream);
  delete state;

  SDL_Quit();
}
