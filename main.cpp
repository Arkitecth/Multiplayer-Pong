#include "iostream"
#include <SDL3/SDL.h>

struct Paddle
{
	float positionX{};
	float positionY{};
	float width{};
	float height{};

	void draw(SDL_Renderer* m_renderer)
	{
		SDL_FRect rect{positionX, positionY, width, height};
		SDL_SetRenderDrawColor(m_renderer, 255,  255,  255,  1); 
		SDL_RenderRect(m_renderer, &rect); 
	}
}; 

struct Ball 
{
	float positionX{};
	float positionY{};
	float width{};
	float height{};

	void draw(SDL_Renderer* m_renderer)
	{
		SDL_FRect rect{positionX, positionY, width, height};
		SDL_SetRenderDrawColor(m_renderer, 255,  255,  255,  1); 
		SDL_RenderRect(m_renderer, &rect); 
	}
}; 





int main(int argc, const char* argv[])
{
	std::string host = "3490";
	if (argc > 1) 
	{
		host = argv[1];
	}
	int width{800};
	int height{450};

	SDL_Window* window{};
	SDL_Renderer* renderer{};

	Paddle paddle{50, 100, 15, 100};

	Paddle paddle2{750, 100, 15, 100};

	Ball ball{400, 225, 25, 25};

	bool status = SDL_CreateWindowAndRenderer("Pong", width, height, SDL_WINDOW_ALWAYS_ON_TOP, &window, &renderer);

	if (status == false) 
	{
		std::cout << "An error occurred creating an SDL_Window" << '\n';
	}

	bool running = true;

	while (running) 
	{
		SDL_Event e; 
		while (SDL_PollEvent(&e)) 
		{
			if (e.type == SDL_EVENT_QUIT) 
			{
				running = false;
				SDL_Quit();
			}
		}


		//Draw
		paddle.draw(renderer); 
		paddle2.draw(renderer); 
		ball.draw(renderer); 

		SDL_SetRenderDrawColor(renderer, 0,  0,  0,  0);


		SDL_RenderPresent(renderer); 
		SDL_RenderClear(renderer); 
	}

}
