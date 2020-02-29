#include <stdio.h>
#include "Constants.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

typedef struct Player
{
	float x;
	float y;
	float width;
	float height;
	int turnDir;  // -1 turn left | +1 turn right
	int walkDir; // -1 move back | + 1 move forward
	float rotationAngle;
	float speed;
	float rotSpeed;
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

void renderMap();
void renderPlayer();
void renderRays();
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
	player.x = 0;
	player.y = 0;
	player.width = 5;
	player.height = 5;
	player.turnDir = 0;
	player.walkDir = 0;
	player.rotationAngle = PI / 2;
	player.speed = 100;
	player.rotSpeed = PI;
}

void update()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH));

	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;
	ticksLastFrame = SDL_GetTicks();

	player.x += deltaTime * 20;
	player.y += deltaTime * 20;
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
	SDL_RenderClear(renderer);

	renderMap();
	//renderRays();
	//renderPlayer();

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

void renderMap()
{
	for (int y = 0; y < MAP_NUM_ROWS; y++)
	{
		for (int x = 0; x < MAP_NUM_COLS; x++)
		{
			int tileX = x * TILE_SIZE;
			int tileY = y * TILE_SIZE;
			int tileColor = map[y][x] != 0 ? 255 : 0;

			SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
			SDL_Rect mapTileRect = { tileX, tileY, TILE_SIZE, TILE_SIZE };
			SDL_RenderFillRect(renderer, &mapTileRect);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &mapTileRect);
		}
	}
}

void renderPlayer()
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect rect = { player.x, player.y, 20, 20 };
	SDL_RenderFillRect(renderer, &rect);
}

void renderRays()
{
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
