#pragma once

#define FALSE 0
#define TRUE 1

#define MINIMAP_SCALE_FACTOR  0.3f
#define MAP_NUM_ROWS 11
#define MAP_NUM_COLS 15
#define TILE_SIZE 64

#define NUM_TEXTURES 9
#define TEX_W 64
#define TEX_H 64

#define FPS 30
#define FRAME_TIME_LENGTH (1000.0f / FPS)

#define PI 3.14159265f
#define PI_2 6.28318530f

#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

#define FOV (60 * PI / 180)

#define NUM_RAYS WINDOW_WIDTH

const char* const WINDOW_TITLE = "Raycasting demo";

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] =
{
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1},
			{1, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 3, 0, 1},
			{1, 6, 6, 6, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 2, 4, 4, 4, 3, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{1, 7, 7, 7, 7, 7, 0, 0, 0, 8, 8, 8, 8, 0, 1},
			{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const Uint32 FLOOR_COLOR = 0xFF3E3E3E;
const Uint32 CEILING_COLOR = 0xFF000000;

const char* const REDBRICK_TEX = "./assets/tex/redbrick.png";
const char* const PURPLESTONE_TEX = "./assets/tex/purplestone.png";
const char* const MOSSYSTONE_TEX = "./assets/tex/mossystone.png";
const char* const GRAYSTONE_TEX = "./assets/tex/graystone.png";
const char* const COLORSTONE_TEX = "./assets/tex/colorstone.png";
const char* const BLUESTONE_TEX = "./assets/tex/bluestone.png";
const char* const WOOD_TEX = "./assets/tex/wood.png";
const char* const EAGLE_TEX = "./assets/tex/eagle.png";