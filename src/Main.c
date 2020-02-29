#include <stdio.h>
#include "Constants.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

typedef struct Player
{
	float posX;
	float posY;
} Player;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int ticksLastFrame = 0;

Player player;
int isGameRunning = FALSE;

int initializeWindow();
void destroyWindow();
void setup();

void processInput();

void render();

void update();

int main(int argc, char* argv[])
{
	printf("Hello world\n");

	isGameRunning = initializeWindow();

	setup();
	
	while (isGameRunning == TRUE)
	{
		processInput();
		update();
		render();
	}

	destroyWindow();

	printf("Game finished\n");
	return 0;
}

void setup()
{
	player.posX = 0;
	player.posY = 0;
}

void update()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH));

	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;
	ticksLastFrame = SDL_GetTicks();

	player.posX += deltaTime * 20;
	player.posY += deltaTime * 20;
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect rect = { player.posX, player.posY, 20, 20 };
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);
}

void processInput()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		isGameRunning = FALSE;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
			isGameRunning = FALSE;
		break;
	}
}

int initializeWindow()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initliazing SDL.\n");
		return FALSE;
	}

	window = SDL_CreateWindow(WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0);
	if (!window)
	{
		fprintf(stderr, "Error creating SDL window.\n");
		return FALSE;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Error creating SDL renderer.\n");
		return FALSE;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return TRUE;
}

void destroyWindow()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
