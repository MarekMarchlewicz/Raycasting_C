#include <stdio.h>
#include<limits.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "Constants.h"


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

typedef struct  Ray
{
	float angle;
	float hitX;
	float hitY;
	float dist;
	int isVertical;
	int hitContent;
} Ray;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int ticksLastFrame = 0;

Player player;
Ray rays[NUM_RAYS];
int isGameRunning = FALSE;

int initializeWindow();
void destroyWindow();
void setup();

void processInput();
void movePlayer(float deltaTime);

float normalizeAngle(float angle);
int hasWallAt(int x, int u);
float distanceBetweenPoints(float x1, float y1, float x2, float y2);
void castAllRays();

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
	castAllRays();
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
	SDL_RenderClear(renderer);

	renderMap();
	renderRays();
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

	if (!hasWallAt((int)newX, (int)newY))
	{
		player.x = newX;
		player.y = newY;
	}
}

void castAllRays()
{
	for (int i = 0; i < NUM_RAYS; i++)
	{
		float rayAngle = player.rotationAngle - (FOV / 2.0f) + i * (FOV / NUM_RAYS);
		float angle = normalizeAngle(rayAngle);
		rays[i].angle = angle;
		float tangent = tan(angle);

		int isFacingDown = angle > 0 && angle < PI;
		int isFacingUp = !isFacingDown;

		int isFacingRight = angle < 0.5 * PI || angle > 1.5 * PI;
		int isFacingLeft = !isFacingRight;

		float xintercept, yintercept;
		float xstep, ystep;

		//////// Horizontal //////// 

		int foundHorzHit = FALSE;
		float horzWallHitX = 0;
		float horzWallHitY = 0;
		int horzWallContent = -1;

		yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
		yintercept += isFacingDown ? TILE_SIZE : 0;

		xintercept = player.x + (yintercept - player.y) / tangent;

		ystep =  isFacingUp ? -TILE_SIZE : TILE_SIZE;

		xstep = TILE_SIZE / tangent;
		xstep *= (isFacingLeft && xstep > 0) ? -1 : 1;
		xstep *= (isFacingRight && xstep < 0) ? -1 : 1;

		float nextHorzTouchX = xintercept;
		float nextHorzTouchY = yintercept;

		while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH && nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT) 
		{
			float xToCheck = nextHorzTouchX;
			float yToCheck = nextHorzTouchY + (isFacingUp ? -1 : 0);

			int mapContent = hasWallAt(xToCheck, yToCheck);
			if (mapContent)
			{
				horzWallHitX =  nextHorzTouchX;
				horzWallHitY = nextHorzTouchY;
				horzWallContent = mapContent;
				foundHorzHit = TRUE;
				break;
			}
			else
			{
				nextHorzTouchX += xstep;
				nextHorzTouchY += ystep;
			}
		}

		//////// Vertical //////// 
		
		int foundVertHit = FALSE;
		float vertWallHitX = 0;
		float vertWallHitY = 0;
		int vertWallContent = -1;

		xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
		xintercept += isFacingRight ? TILE_SIZE : 0;

		yintercept = player.y + (xintercept - player.x) * tangent;

		xstep = isFacingLeft ? -TILE_SIZE : TILE_SIZE;

		ystep = TILE_SIZE * tangent;
		ystep *= (isFacingUp == 0 && ystep > 0) ? -1 : 1;
		ystep *= (isFacingDown && ystep < 0) ? -1 : 1;

		float nextVertTouchX = xintercept;
		float nextVertTouchY = yintercept;

		while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH && nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) 
		{
			float xToCheck = nextVertTouchX + (isFacingLeft ? -1 : 0);
			float yToCheck = nextVertTouchY;

			int mapContent = hasWallAt(xToCheck, yToCheck);
			if (mapContent)
			{
				vertWallHitX = nextVertTouchX;
				vertWallHitY =  nextVertTouchY;
				vertWallContent = mapContent;
				foundVertHit = TRUE;
				break;
			}
			else
			{
				nextVertTouchX += xstep;
				nextVertTouchY += ystep;
			}
		}
		
		float hDist = foundHorzHit ? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY) : INT_MAX;
		float vDist =  foundVertHit ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY) : INT_MAX;
		
		if (vDist < hDist)
		{
			//vertical
			rays[i].dist = vDist;
			rays[i].hitContent = vertWallContent;
			rays[i].hitX = vertWallHitX;
			rays[i].hitY = vertWallHitY;
			rays[i].isVertical = TRUE;
		}
		else
		{
			//horizontal
			rays[i].dist = hDist;
			rays[i].hitContent = horzWallContent;
			rays[i].hitX = horzWallHitX;
			rays[i].hitY = horzWallHitY;
			rays[i].isVertical = FALSE;
		}
	}
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) 
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float normalizeAngle(float angle)
{
	float normAngle = remainder(angle, PI_2);
	if (normAngle < 0)
		normAngle += PI_2;
	return normAngle;
}

int hasWallAt(int x, int y)
{
	if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
		return TRUE;

	int mapGridIndexX = floor(x / TILE_SIZE);
	int mapGridIndexY = floor(y / TILE_SIZE);

	return map[mapGridIndexY][mapGridIndexX];
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
	for (int i = 0; i < NUM_RAYS; i++)
	{
		if(rays[i].isVertical)
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		else
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderDrawLine(
			renderer,
			player.x * MINIMAP_SCALE_FACTOR,
			player.y * MINIMAP_SCALE_FACTOR,
			rays[i].hitX * MINIMAP_SCALE_FACTOR,
			rays[i].hitY * MINIMAP_SCALE_FACTOR
		);
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
