#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

#include "common.h"


#include "Sprite.h"

std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_Renderer *ren; //pointer to the SDL_Renderer
SDL_Surface *surface; //pointer to the SDL_Surface
SDL_Texture *tex; //pointer to the SDL_Texture
SDL_Surface *messageSurface; //pointer to the SDL_Surface for message
SDL_Texture *messageTexture; //pointer to the SDL_Texture for message
SDL_Rect message_rect; //SDL_rect for the message

std::vector<Sprite::UniquePointer> spriteList;

//The music that will be played
Mix_Music *musicTrack = nullptr;

//The sound effects that will be used
Mix_Chunk *chunk_FrontLeft = nullptr;
Mix_Chunk *chunk_FrontRight = nullptr;


bool done = false;

void handleInput()
{
	//Event-based input handling
	//The underlying OS is event-based, so **each** key-up or key-down (for example)
	//generates an event.
	//  - https://wiki.libsdl.org/SDL_PollEvent
	//In some scenarios we want to catch **ALL** the events, not just to present state
	//  - for instance, if taking keyboard input the user might key-down two keys during a frame
	//    - we want to catch based, and know the order
	//  - or the user might key-down and key-up the same within a frame, and we still want something to happen (e.g. jump)
	//  - the alternative is to Poll the current state with SDL_GetKeyboardState

	SDL_Event event; //somewhere to store an event

	//NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set donecreate remote branch flag if SDL wants to quit (i.e. if the OS has triggered a close event,
							//  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					//hit escape to exit
					case SDLK_ESCAPE: done = true; break;
					case SDLK_1:
						{
							std::cout << Mix_PlayChannel( -1, chunk_FrontLeft, 0 ) << std::endl;
						}
						break;
					case SDLK_2:  Mix_PlayChannel( -1, chunk_FrontRight, 0 ); break;
					case SDLK_p:
					{
						if (Mix_PausedMusic())
						{
							Mix_ResumeMusic();
						}
						else
						{
							Mix_PauseMusic();
						}
					}
					break;
					default:
						{
							SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Unhandled key down event type, %d", event.type);
						}
						break;
				}
			break;
		default:
			{
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Unhandled event type, %d", event.type);
			}
			break;

		}
	}
}
// end::handleInput[]

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
  //CHANGE ME
}

void render()
{
		//First clear the renderer
		SDL_RenderClear(ren);

		// Draw the texture
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Rendering sprites ...");
		for (auto const& sprite : spriteList) //unique_ptr can't be copied, so use reference
		{
			SDL_RenderCopy(ren, tex, NULL, &sprite->rectangle);
		}

		//Draw the text
		//SDL_RenderCopy(ren, messageTexture, NULL, &message_rect);

		//Update the screen
		SDL_RenderPresent(ren);
}

void cleanExit(int returnValue)
{
	//Free the music
	Mix_FreeMusic( musicTrack );
	musicTrack = nullptr;

	 //Free the sound effects
	Mix_FreeChunk( chunk_FrontLeft );
	Mix_FreeChunk( chunk_FrontRight );


	if (messageTexture != nullptr) SDL_DestroyTexture(messageTexture);
	if (tex != nullptr) SDL_DestroyTexture(tex);
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	if (win != nullptr) SDL_DestroyWindow(win);
	SDL_Quit();
	exit(returnValue);
}

enum class CerealFileFormat {JSON, XML, UNKNOWN};
CerealFileFormat getCerealFileFormat(const std::string& s)
{
	size_t i = s.rfind('.', s.length());
	if (i != std::string::npos) {
		std::string subString = (s.substr(i+1, s.length() - i));
		 if (subString == "json") return CerealFileFormat::JSON;
		if (subString == "xml")  return  CerealFileFormat::XML;
	}
	return(CerealFileFormat::UNKNOWN);
}

void loadSpriteList(std::string spriteListFilePath)
{
	CerealFileFormat spriteListFileFormat = getCerealFileFormat(spriteListFilePath);
	std::ifstream inFile( spriteListFilePath );
	switch (spriteListFileFormat)
	{
	case CerealFileFormat::JSON:
		{
			cereal::JSONInputArchive inArchive(inFile);
			inArchive(spriteList);
		}
		break;
	case CerealFileFormat::XML:
		{
			cereal::XMLInputArchive inArchive(inFile);
			inArchive(spriteList);
		}
		break;
	case CerealFileFormat::UNKNOWN:
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "%s is an unknown format. Ignoring", spriteListFilePath.c_str());
	}

	for (auto const& sprite : spriteList) //unique_ptr can't be copied, so use reference
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Sprite loaded: %d, %d, %d, %d, ", sprite->rectangle.x, sprite->rectangle.y, sprite->rectangle.w, sprite->rectangle.h);
	}
}

void writeSpriteList(std::string spriteListFilePath, std::vector<std::unique_ptr<Sprite>>& spriteListTmp)
{
	CerealFileFormat spriteListFileFormat = getCerealFileFormat(spriteListFilePath);
	std::ofstream outFile( spriteListFilePath );

	switch (spriteListFileFormat)
	{
	case CerealFileFormat::JSON:
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Writing spriteList as JSON.");
			cereal::JSONOutputArchive outArchive(outFile);
			outArchive(spriteListTmp);
		}
		break;
	case CerealFileFormat::XML:
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Writing spriteList as XML.");
			cereal::XMLOutputArchive outArchive(outFile);
			outArchive(spriteListTmp);
		}
		break;
	case CerealFileFormat::UNKNOWN:
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "%s is an unknown format. Ignoring", spriteListFilePath.c_str());
	}

	for (auto const& sprite : spriteListTmp) //unique_ptr can't be copied, so use reference
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Sprite written out: %d, %d, %d, %d, ", sprite->rectangle.x, sprite->rectangle.y, sprite->rectangle.w, sprite->rectangle.h);
	}
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "\"%s\" created.", spriteListFilePath.c_str());
}

// based on http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world/
int main( int argc, char* args[] )
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL initialised OK!\n";

	//create window
	win = SDL_CreateWindow("SDL Hello World!", 100, 100, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	//error handling
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL CreatedWindow OK!\n";

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	SDL_RenderSetLogicalSize(ren, 600, 600);

	std::string imagePath = "./assets/Opengl-logo.svg.png";
	surface = IMG_Load(imagePath.c_str());
	if (surface == nullptr){
		std::cout << "SDL IMG_Load Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	tex = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	if (tex == nullptr){
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}


	if( TTF_Init() == -1 )
	{
		std::cout << "TTF_Init Failed: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}

	TTF_Font* sans = TTF_OpenFont("./assets/Hack-Regular.ttf", 96);
	if (sans == nullptr)
	{
		std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}
	SDL_Color White = {255, 255, 255};
	messageSurface = TTF_RenderText_Solid(sans, "Hello World!", White);
	messageTexture = SDL_CreateTextureFromSurface(ren, messageSurface);
	message_rect.x = 0;
	message_rect.y = 0;
	message_rect.w = 300;
	message_rect.h = 100;


	std::string scoped_spriteListFilePath = "spriteList.json"; //CHANGE this for different formats = options {json, xml}

	try
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading spriteList.");
		loadSpriteList(scoped_spriteListFilePath);
	}
	catch (const cereal::Exception& e)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "\"%s\" doesn't load. Creating ...", scoped_spriteListFilePath.c_str());

		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Adding sprites to list ...");
		std::vector<std::unique_ptr<Sprite>> spriteListTmp;
		spriteListTmp.push_back(std::make_unique<Sprite>(0,    0,     100, 43));
		spriteListTmp.push_back(std::make_unique<Sprite>(200,  200,   200, 86));
		spriteListTmp.push_back(std::make_unique<Sprite>(-100, 440,   200, 86));

		writeSpriteList(scoped_spriteListFilePath, spriteListTmp);

		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Re-loading spriteList.");
		loadSpriteList(scoped_spriteListFilePath);
	}


	//Initialize SDL_mixer
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() <<  std::endl;
		cleanExit(1);
	}

	//Load music
	musicTrack = Mix_LoadMUS( "assets/Skipping in the No Standing Zone.mp3" );
	if( musicTrack == NULL )
	{
		std::cout << "Failed to load beat music! SDL_mixer Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	//Load sound effects
	chunk_FrontLeft = Mix_LoadWAV( "assets/Front_Left.wav" );
	if( chunk_FrontLeft == nullptr )
	{
		std::cout << "Failed to load scratch sound effect! SDL_mixer Error: " << Mix_GetError()  << std::endl;
	}

	chunk_FrontRight = Mix_LoadWAV( "assets/Front_Right.wav" );
	if( chunk_FrontRight == nullptr )
	{
		std::cout << "Failed to load scratch sound effect! SDL_mixer Error: " << Mix_GetError()  << std::endl;
	}


	Mix_PlayMusic(musicTrack, -1 );

	while (!done) //loop until done flag is set)
	{
		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		render(); // this should render the world state according to VARIABLES

		SDL_Delay(20); // unless vsync is on??
	}

	std::cout << "\nSDL Exiting!\n";

	cleanExit(0);
	return 0;
}
