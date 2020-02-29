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
void movePlayer(float deltaTime);

int hasWallAt(int x, int u);

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
	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
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

	movePlayer(deltaTime);
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
	SDL_RenderClear(renderer);

	renderMap();
	//renderRays();
	renderPlayer();

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
		if (event.key.keysym.sym == SDLK_UP)
			player.walkDir = 1;
		if (event.key.keysym.sym == SDLK_DOWN)
			player.walkDir = -1;
		if (event.key.keysym.sym == SDLK_LEFT)
			player.turnDir = -1;
		if (event.key.keysym.sym == SDLK_RIGHT)
			player.turnDir = 1;
		break;
	case SDL_KEYUP:
		if (event.key.keysym.sym == SDLK_UP && player.walkDir == 1)
			player.walkDir = 0;
		if (event.key.keysym.sym == SDLK_DOWN && player.walkDir == -1)
			player.walkDir = 0;
		if (event.key.keysym.sym == SDLK_LEFT && player.turnDir == -1)
			player.turnDir = 0;
		if (event.key.keysym.sym == SDLK_RIGHT && player.turnDir == 1)
			player.turnDir = 0;
		break;
	}
}

void movePlayer(float deltaTime)
{
	player.rotationAngle += player.turnDir * player.rotSpeed * deltaTime;

	float moveStep = player.walkDir * player.speed * deltaTime;

	float newX = player.x + cos(player.rotationAngle) * moveStep;
	float newY = player.y + sin(player.rotationAngle) * moveStep;

	if (!hasWallAt(newX, newY))
	{
	player.x = newX;
	player.y = newY;
}
}

int hasWallAt(int x, int y)
{
	if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
		return TRUE;

	int mapGridIndexX = floor(x / TILE_SIZE);
	int mapGridIndexY = floor(y / TILE_SIZE);

	return map[mapGridIndexY][mapGridIndexX] == 1;
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
			SDL_Rect mapTileRect = { tileX * MINIMAP_SCALE_FACTOR, tileY * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR, TILE_SIZE  * MINIMAP_SCALE_FACTOR };
			SDL_RenderFillRect(renderer, &mapTileRect);
			/*SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &mapTileRect);*/
		}
	}
}

void renderPlayer()
{
	SDL_Rect rect = { player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR, player.width * MINIMAP_SCALE_FACTOR, player.height * MINIMAP_SCALE_FACTOR  };
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_RenderDrawLine(renderer,
		player.x * MINIMAP_SCALE_FACTOR,
		player.y * MINIMAP_SCALE_FACTOR,
		player.x * MINIMAP_SCALE_FACTOR + cos(player.rotationAngle) * 40 * MINIMAP_SCALE_FACTOR,
		player.y * MINIMAP_SCALE_FACTOR + sin(player.rotationAngle) * 40 * MINIMAP_SCALE_FACTOR);
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
