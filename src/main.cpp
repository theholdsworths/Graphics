#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include "zmq.hpp"

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_Renderer *ren; //pointer to the SDL_Renderer
SDL_Surface *surface; //pointer to the SDL_Surface
SDL_Texture *tex; //pointer to the SDL_Texture
SDL_Rect texture_rect; //SDL_Rect for the texture

SDL_Surface *messageSurface; //pointer to the SDL_Surface for message
SDL_Texture *messageTexture; //pointer to the SDL_Texture for message
SDL_Rect message_rect; //SDL_rect for the message

bool done = false;

// ZMQVariables - our context and socket
bool ZMQserver = false;
zmq::context_t this_zmq_context(1);

zmq::socket_t this_zmq_publisher(this_zmq_context, ZMQ_PUB); //create publisher and subscriber always (this is dumb)
zmq::socket_t this_zmq_subscriber(this_zmq_context, ZMQ_SUB);

void handleNetwork()
{
	// message format
	// 4 floats, separated by spaces, each with 6 digits, including 0 padding, and 3 digits of precision
	// this will use 8 bytes per float (6 digits, the decimal, and the space)
	// making 4 * 8 bytes = 32 bytes
	const int messageLength = 32;

	if (ZMQserver)
	{
	// create a message
		zmq::message_t message(messageLength);

		// add message content according to above format
		snprintf((char *)message.data(), messageLength,
			"%06.3f %06.3f %06.3f %06.3f ", float(texture_rect.x), float(texture_rect.y), float(message_rect.x), float(message_rect.y));

		std::cout << "Message sent: \"" << std::string(static_cast<char*>(message.data()), message.size()) << "\"" << std::endl;

		//  Send message to all subscribers
		this_zmq_publisher.send(message);
	}
	else
	{
		// set up NULL filter - i.e. accept all messages
		const std::string filter = "";

		// set filter on subscriber (we don't really need to do this everyone time)
		this_zmq_subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.length());

		// storage for a new message
		zmq::message_t update;

		// loop while there are messages (could be more than one)
		while (this_zmq_subscriber.recv(&update, ZMQ_DONTWAIT))
		{
			// get the data from the message as a char* (for debug output)
			char* the_data = static_cast<char*>(update.data());

			// debug output
			std::cout << "Message received: \"" << std::string(the_data) << "\"" << std::endl;

			// get the data as a streamstring (many other options than this)
			std::istringstream iss(static_cast<char*>(update.data()));

			//data format is floats, so have to read back to floats
			float tx, ty, mx, my;

			// read the string stream into the four floats
			iss >> tx >> ty >> mx >> my;

			// use those floats to set the SDL_Rects (auto convert to int)
			texture_rect.x = tx;
			texture_rect.y = ty;
			message_rect.x = mx;
			message_rect.y = my;

			// Debug output
			std::cout << "texture x, y: " << std::to_string(texture_rect.x) << ", " << std::to_string(texture_rect.y) << std::endl;
			std::cout << "message x, y: " << std::to_string(message_rect.x) << ", " << std::to_string(message_rect.y) << std::endl;
		}
	}
}

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
					case SDLK_ESCAPE: done = true;
				}
			break;
		}
	}
}
// end::handleInput[]

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
	// do somesimulation if the server
	if (ZMQserver)
	{
		// bad bad bad simulation - not accounting for real-time
		texture_rect.x += 1.0f;
		texture_rect.x = texture_rect.x % (600-texture_rect.w); //bad hardcoding


		message_rect.y += 22.0f;
		message_rect.y = message_rect.y % (600-message_rect.h);
	}

	handleNetwork(); //send or receive

}

void render()
{
		//First clear the renderer
		SDL_RenderClear(ren);

		//Draw the texture
		SDL_RenderCopy(ren, tex, NULL, &texture_rect);

		//Draw the text
		SDL_RenderCopy(ren, messageTexture, NULL, &message_rect);

		//Update the screen
		SDL_RenderPresent(ren);
}

void cleanExit(int returnValue)
{
	std::cout << "Exiting." << std::endl;
	if (messageTexture != nullptr) SDL_DestroyTexture(messageTexture);
	if (tex != nullptr) SDL_DestroyTexture(tex);
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	if (win != nullptr) SDL_DestroyWindow(win);
	SDL_Quit();
	exit(returnValue);
}

// based on http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world/
int main( int argc, char* args[] )
{
	std::cout << "argc was: " << argc << std::endl;
	if (argc > 1)
	{
		std::string s(args[1]);
		if (s == "--server")
		{
			ZMQserver = true;
			std::cout << "Running as SERVER" << std::endl;
			std::cout << "Args[1] was: \"" << args[1] << "\"" << std::endl;
		}
	}
	else
	{
		ZMQserver = false;
		std::cout << "Running as CLIENT" << std::endl;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL initialised OK!\n";

	//create window (server on left of screen, client to the right)
	if (ZMQserver)
		win = SDL_CreateWindow("SDL Hello World!! (SERVER)", 100, 100, 600, 600, SDL_WINDOW_SHOWN);
	else
		win = SDL_CreateWindow("SDL Hello World!! (CLIENT)", 700, 100, 600, 600, SDL_WINDOW_SHOWN);


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



	texture_rect.x = 100;
	texture_rect.y = 200;
	texture_rect.w = 300;
	texture_rect.h = 100;


	if (ZMQserver)
	{
		this_zmq_publisher.bind("tcp://*:5556");
	}
	else
	{
		std::cout << "Subscribing to server ..." << std::endl;
		this_zmq_subscriber.connect("tcp://localhost:5556");
	}

	while (!done) //loop until done flag is set)
	{
		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		render(); // this should render the world state according to VARIABLES

		SDL_Delay(20); // unless vsync is on??
	}

	cleanExit(0);
	return 0;
}
