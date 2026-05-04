#include "iostream"
#include <SDL3/SDL.h>
#include <cstdlib>
#include "net.h"

struct Paddle
{
	float positionX{};
	float positionY{};
	float width{};
	float height{};
	float speed{1};

	void draw(SDL_Renderer* m_renderer)
	{
		SDL_FRect rect{positionX, positionY, width, height};
		SDL_SetRenderDrawColor(m_renderer, 255,  255,  255,  1); 
		SDL_RenderRect(m_renderer, &rect); 
	}

	void update(GameState* state)
	{
		positionX = state->positionX;

		positionY = state->positionY;

		width = state->width;

		height = state->height;

		speed = state->speed;
	}

	void moveUp()
	{
		positionY -= 1 * speed;
	}

	void moveDown()
	{
		positionY += 1 * speed; 
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

	void move()
	{
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
	Ball ball{400, 225, 25, 25};
	GameState state[2]{};
	int current_player{};

	int serverfd = connectToServer("localhost", "3490"); 
	if (serverfd == -1) 
	{
		exit(1); 
	}

	receive_data(serverfd, state, true); 
	if (state[1].current_index == -1) 
	{
		current_player = 0; //Player_1
	} 
	else 
	{
		current_player = 1; //Player 2
	}

	if (state[0].positionX == 0) 
	{
		std::cout << "An error occurred with data" << '\n';
		exit(1); 
	}

	Paddle paddle{state[0].positionX, state[0].positionY, state[0].width, state[0].height, state[0].speed};

	Paddle paddle2{state[1].positionX, state[1].positionY, state[1].width, state[1].height, state[1].speed};

	bool status = SDL_CreateWindowAndRenderer("Pong", width, height, SDL_WINDOW_ALWAYS_ON_TOP, &window, &renderer);

	if (status == false) 
	{
		std::cout << "An error occurred creating an SDL_Window" << '\n';
	}

	bool running = true;

	while (running) 
	{
		SDL_Event e; 
		receive_data(serverfd, state, false); 
		while (SDL_PollEvent(&e)) 
		{
			if (e.type == SDL_EVENT_QUIT) 
			{
				running = false;
				SDL_Quit();
			}

			if (e.key.key == SDLK_DOWN) 
			{
				if (current_player == 0) 
				{
					paddle.moveDown();
					state[current_player].positionY -= 1 * state[0].speed; 
					send_data(serverfd, state); 
				} 
				else 
				{
					paddle2.moveDown();
					state[current_player].positionY -= 1 * state[1].speed; 
					send_data(serverfd, state); 

				}
			}

			if (e.key.key == SDLK_UP) 
			{

				if (current_player == 0) 
				{
					paddle.moveUp();
					state[current_player].positionY += 1 * state[0].speed; 
					send_data(serverfd, state); 
				} 
				else 
				{
					paddle2.moveUp();
					state[current_player].positionY += 1 * state[1].speed; 
					send_data(serverfd, state); 

				}
			}

		}
		paddle.update(&state[0]);
		paddle2.update(&state[1]);

		//Draw
		paddle.draw(renderer); 
		paddle2.draw(renderer);
		ball.draw(renderer); 

		SDL_SetRenderDrawColor(renderer, 0,  0,  0,  0);


		SDL_RenderPresent(renderer); 
		SDL_RenderClear(renderer); 
	}

}
