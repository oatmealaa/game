#include <SDL2/SDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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

#define X_RES 360
#define Y_RES 180

#define CENTER_Y Y_RES/2

#define MOVE_SPEED 0.04
#define ROT_SPEED 0.04

#define VIEW_DISTANCE 100.0f
#define FOV_FACTORX 0.8f
#define FOV_FACTORY 0.55f

typedef struct vec2f_s {f32 x,y;} vec2f;
typedef struct vec2i_s {i32 x,y;} vec2i;


static struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[X_RES*Y_RES];
	bool quit;

	
	vec2f pos;
	vec2f dir;
	vec2f plane;
	f32 dirangle;
} state;	

#define MAP_SIZE 42

static u8 MAP[MAP_SIZE*MAP_SIZE] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


static inline vec2f normalize(vec2f vec) {
	f32 len = sqrt((vec.x*vec.x)+(vec.y*vec.y));
	vec2f new = {
		vec.x/fabs(len),
		vec.y/fabs(len) 
	};
	return new;
}

static inline void drawLine(int x,u32 color ,f32 dist) {
	f32 rep = 1/dist;
	i32 height = (int)((f32)X_RES*rep)*FOV_FACTORY;
	for (int y = CENTER_Y; y < CENTER_Y+(height/2); y++) {
		if (y>=Y_RES) break;
		state.pixels[X_RES*y+x] = color;
	}
	for (int y = CENTER_Y; y > CENTER_Y-(height/2); y--) {
		if (y<=0) break;
		state.pixels[X_RES*y+x] = color;
	}
}


static void render() {
	for (int x = 0; x < X_RES; x++) {
		f32 planex =  2 * (x / (f32) X_RES) - 1;

		vec2f ray_vector = {
			state.dir.x + (state.plane.x * planex),
			state.dir.y + (state.plane.y * planex),	
		};
		
		vec2f pos = state.pos;
		vec2i posi = {(int)state.pos.x, (int)state.pos.y};

		//ratio of change of 1 in axsis to length of ray
		vec2f unit_length_ratio = {
			sqrt(1+(ray_vector.y / ray_vector.x)*(ray_vector.y / ray_vector.x)),
			sqrt(1+(ray_vector.x / ray_vector.y)*(ray_vector.x / ray_vector.y))
		}; 
		vec2i step_dir;

		//distance to first x and y cell intersect respectivly
		vec2f dist = {0,0};
		
		if (ray_vector.x>0.0f) {
			step_dir.x = 1;
			dist.x = (((f32)posi.x+1.0f) - pos.x) * unit_length_ratio.x;
		} else if (ray_vector.x<0.0f) {
			step_dir.x = -1;
			dist.x = (pos.x - (f32) posi.x) * unit_length_ratio.x;
		}

		if (ray_vector.y<0.0f) {
			step_dir.y = -1;
			dist.y = (pos.y - (f32) posi.y) * unit_length_ratio.y;

		} else if (ray_vector.y>0.0f) {
			step_dir.y = 1;
			dist.y = (((f32)posi.y+1.0f) - pos.y) * unit_length_ratio.y;
		}

		bool wall_hit = false;
		f32 max = VIEW_DISTANCE;
		f32 distf = 0.0f;
		vec2i current_tile = posi;
		bool side = false;
		while (!wall_hit&&distf<max) {
			
			if (dist.x<dist.y) {
				current_tile.x += step_dir.x;
				distf = dist.x;
				dist.x += unit_length_ratio.x;
				side = true;
			} else {
				current_tile.y += step_dir.y;
				distf = dist.y;
				dist.y += unit_length_ratio.y;
				side = false;
			}

			if (current_tile.x < MAP_SIZE && current_tile.y < MAP_SIZE && current_tile.x >= 0 && current_tile.y >= 0 ) {
				if (MAP[MAP_SIZE*current_tile.y+current_tile.x]) {
					/*
					vec2f d = {
						(ray_vector.x == 0) ? 1e30 : fabs(1/ray_vector.x),
						(ray_vector.y == 0) = 1e30 : fabs(1/ray_vector.y)
					};
					*/

					f32 plane_dist;
					//vec2f cooking = {ray_vector.x-state.dir.x,ray_vector.y-state.dir.y};
					//f32 plane_dist = distf-sqrt((cooking.x*cooking.x)+(cooking.y*cooking.y));
					plane_dist = distf-sqrt((ray_vector.x*ray_vector.x)+(ray_vector.y*ray_vector.y));
					
					if (side) {
						//plane_dist = dist.x - d.x;
						drawLine(x,0xFFFF00FF,plane_dist);
					} else {
						//plane_dist = dist.y - d.y;
						drawLine(x,0xFFEE00EE,plane_dist);
					}
					break;
				} 
			} 

		}


	}
}


int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("SDL init error");
		return 1;
	}

	state.window =
		SDL_CreateWindow(
			":3",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			0
		);
	state.renderer =
		SDL_CreateRenderer(
			state.window,
			-1,
			SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
		);

	state.texture =
		SDL_CreateTexture(
			state.renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			X_RES,
			Y_RES
		);

	state.pos.x = 4.0f;
	state.pos.y = 5.0f;
	state.dir = normalize(((vec2f) {1.0f,-1.0f}));
	state.dirangle = 1;
	state.plane = (vec2f) {state.dir.y*FOV_FACTORX,-state.dir.x*FOV_FACTORX};

	while (!state.quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				state.quit = true;	
			}
		}

		const u8 *key = SDL_GetKeyboardState(NULL);

		if (key[SDLK_UP&0xFFFF]) {
			state.pos.x += state.dir.x*MOVE_SPEED;
			state.pos.y += state.dir.y*MOVE_SPEED;
		}


		if (key[SDLK_DOWN&0xFFFF]) {
			state.pos.x -= state.dir.x*MOVE_SPEED;
			state.pos.y -= state.dir.y*MOVE_SPEED;
		}

		if (key[SDLK_RIGHT&0xFFFF]) {
			state.dirangle -= ROT_SPEED;
			state.dir.x = cos(state.dirangle);
			state.dir.y = sin(state.dirangle);
	        state.plane = (vec2f) {state.dir.y*FOV_FACTORX,-state.dir.x*FOV_FACTORX};
				
		}

		if (key[SDLK_LEFT&0xFFFF]) {
			state.dirangle += ROT_SPEED;
			state.dir.x = cos(state.dirangle);
			state.dir.y = sin(state.dirangle);
	        state.plane = (vec2f) {state.dir.y*FOV_FACTORX,-state.dir.x*FOV_FACTORX};
		}
		SDL_RenderClear(state.renderer);
		
		memset(state.pixels,0,sizeof(state.pixels));
		render();

		SDL_UpdateTexture(state.texture,
			NULL,
			state.pixels,
			X_RES * 4
		);

		SDL_SetRenderTarget(state.renderer, NULL);
    	SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 0xFF);
    	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_NONE);
		
        SDL_RenderCopyEx(
            state.renderer,
            state.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL);
 
		SDL_RenderPresent(state.renderer);

		//state.quit = true;

	}
	
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
	return 0;
}



