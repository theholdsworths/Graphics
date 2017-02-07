#include <iostream>

// GLEW - OpenGL Extension Wrangler - http://glew.sourceforge.net/
// NOTE: include before SDL.h
#include <GL/glew.h>

// SDL - Simple DirectMedia Layer - https://www.libsdl.org/
#include "SDL.h"
#include "SDL_image.h"

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
  win = SDL_CreateWindow("Hello World", 100, 100, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
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

  // Set OpenGL context parameters
  int major = 4, minor = 5;
  SDL_Log("Asking for OpenGL %d.%d context\n", major, minor);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE); //use core profile
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

  // initialise GLEW - sets up the OpenGL function pointers for the version of OpenGL we're using
  GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
  rev = glewInit();
  if (GLEW_OK != rev){
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "glewInit error: %s\n", glewGetErrorString(rev));
    return 1;
  }
  SDL_Log("glew initialised OK!\n");

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

  SDL_GL_DeleteContext(glcontext);

  SDL_Quit();
  return 0;
}
