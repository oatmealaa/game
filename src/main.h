#include <SDL2/SDL.h>

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;
typedef ssize_t  isize;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// 360x180

#define X_RES 1280
#define Y_RES 720

#define CENTER_Y Y_RES/2

#define MOVE_SPEED 0.04
#define ROT_SPEED 0.04

#define VIEW_DISTANCE 100.0f
#define FOV_FACTORX 0.6f
#define FOV_FACTORY 0.7f

typedef struct vec2f_s {f32 x,y;} vec2f;
typedef struct vec2i_s {i32 x,y;} vec2i;


struct state_s {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[X_RES*Y_RES];
	bool quit;

	
	vec2f pos;
	vec2f dir;
	vec2f plane;
	f32 dirangle;
};	

extern struct state_s state;

#define MAP_SIZE 42

extern u8 MAP[MAP_SIZE*MAP_SIZE];

