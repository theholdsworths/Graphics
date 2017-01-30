#include <iostream>

// // GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// // NOTE: include before SDL.h
// #include <GL/glew.h>

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// // - OpenGL Mathematics - https://glm.g-truc.net/
// #define GLM_FORCE_RADIANS // force glm to use radians
// // NOTE: must do before including GLM headers
// // NOTE: GLSL uses radians, so will do the same, for consistency
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

int main(int argc, char *argv[]) {
  // SDL initialise
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n",
                    SDL_GetError());
    return 1;
  }
  SDL_Log("SDL initialised OK!\n");

  // Window Creation
  SDL_Window *win = nullptr;
  win = SDL_CreateWindow("Hello World", 100, 100, 800, 600, 0);
  if (win == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "SDL_CreateWindow init error: %s\n", SDL_GetError());
    return 1;
  }

  // Renderer Creation
  SDL_Renderer *renderer = nullptr;
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "SDL_CreateRenderer init error: %s\n", SDL_GetError());
    return 1;
  }

  // SDL_Image initialise
  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_image init error: %s\n",
                    IMG_GetError());
    return 1;
  }
  SDL_Log("SDL_image initialised OK!\n");

  // SDL_ttf initialise
  if (TTF_Init() == -1) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf init error: %s\n",
                    SDL_GetError());
    return 1;
  }
  SDL_Log("SDL_ttf initialised OK!\n");

  // SDL_mixer initialise
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "SDL_mixer init error: %s\n   PERHAPS NO "
                    "HEADPHONES/SPEAKERS CONNECTED\n",
                    Mix_GetError());
    return 1;
  }
  SDL_Log("SDL_mixer initialised OK!\n");

  // Load the image
  SDL_Surface *logoSurface = IMG_Load("assets/Sdl-logo.png");
  if (!logoSurface) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "IMG_Load: %s\n", IMG_GetError());
    return 1;
  }
  int width = logoSurface->w;
  int height = logoSurface->h;
  SDL_Log("Image width:height = %d:%d\n", width, height);
  SDL_Texture *logoTexture =
      SDL_CreateTextureFromSurface(renderer, logoSurface);
  SDL_Rect logoRect{100, 100, logoSurface->w, logoSurface->h};

  // Load the font
  TTF_Font *font = TTF_OpenFont("assets/OpenSans-Bold.ttf", 90);
  if (font == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "SDL_ttf font load error: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Log("SDL_ttf loaded font OK!\n");

  // Load the music
  Mix_Music *music = Mix_LoadMUS("assets/beat.wav");
  if (!music) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadMUS error: %s\n",
                Mix_GetError());
    return 1;
  }
  SDL_Log("SDL_mixer loaded music OK!\n");

  // Load the sound effect
  Mix_Chunk *scratch = Mix_LoadWAV("assets/scratch.wav");
  if (!scratch) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Mix_LoadWAV error: %s\n",
                Mix_GetError());
    return 1;
  }
  SDL_Log("SDL_mixer loaded sound effect OK!\n");

  // Create the font texture
  SDL_Rect fontRect;
  SDL_Color fontColor = {255, 0, 0, 255}; // white
  SDL_Surface *fontSolid = TTF_RenderText_Solid(font, "AwesomeText", fontColor);
  SDL_Texture *fontSolidTexture =
      SDL_CreateTextureFromSurface(renderer, fontSolid);
  SDL_QueryTexture(fontSolidTexture, NULL, NULL, &fontRect.w, &fontRect.h);
  fontRect.x = 0;
  fontRect.y = 0;
  SDL_Log("SDL_ttf created texture with size %i, %i\n", fontRect.w, fontRect.h);

  // Play the music and sound effect
  Mix_PlayChannel(-1, scratch, 0);
  Mix_PlayMusic(music, -1);

  // Clear the back buffer
  SDL_RenderClear(renderer);

  // Render
  SDL_RenderCopy(renderer, logoTexture, NULL, &logoRect);
  SDL_RenderCopy(renderer, fontSolidTexture, NULL, &fontRect);

  // Present the back buffer
  SDL_RenderPresent(renderer);

  // Wait
  SDL_Delay(2000);

  // Clean up
  SDL_Log("Finished. Cleaning up and closing down\n");

  Mix_FreeChunk(scratch);
  Mix_FreeMusic(music);
  Mix_CloseAudio();

  SDL_DestroyTexture(fontSolidTexture);
  SDL_FreeSurface(fontSolid);

  SDL_DestroyTexture(logoTexture);
  SDL_FreeSurface(logoSurface);

  SDL_Quit();
  return 0;
}
