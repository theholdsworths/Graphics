#include <iostream>

// GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// NOTE: include before SDL.h
#include <GL/glew.h>

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// - OpenGL Mathematics - https://glm.g-truc.net/
#define GLM_FORCE_RADIANS // force glm to use radians
// NOTE: must do before including GLM headers
// NOTE: GLSL uses radians, so will do the same, for consistency
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
  win = SDL_CreateWindow("Hello World", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  if (win == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "SDL_CreateWindow init error: %s\n", SDL_GetError());
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

  // Set OpenGL context parameters
  int major = 4, minor = 3;
  SDL_Log("Asking for OpenGL %d.%d context\n", major, minor);
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE); //use core profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); //ask for forward compatible 
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //default, probably 
  
  // OpenGL Context Creation
  SDL_GLContext glcontext = SDL_GL_CreateContext(win);
  if (glcontext == NULL) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "SDL_GL_CreateContext init error: %s\n", SDL_GetError());
    return 1;
  }
  
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
  SDL_Log("Got an OpenGL %d.%d context\n", major, minor);
  
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

  // Play the music and sound effect
  Mix_PlayChannel(-1, scratch, 0);
  Mix_PlayMusic(music, -1);

  // Clear the back buffer
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Render
  // TODO add render code
  
  // Present the back buffer
  SDL_GL_SwapWindow(win);

  // Wait
  SDL_Delay(2000);

  // Clean up
  SDL_Log("Finished. Cleaning up and closing down\n");

  Mix_FreeChunk(scratch);
  Mix_FreeMusic(music);
  Mix_CloseAudio();

  SDL_GL_DeleteContext(glcontext);  
  SDL_Quit();
  return 0;
}
