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

#define X_RES 256
#define Y_RES 144

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
} state;	

#define MAP_SIZE 8

static u8 MAP[MAP_SIZE*MAP_SIZE] = {
	1,0,1,0,1,0,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,0,1,0,1,0,1,
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
	for (int y = 72; y < 72+(72/(int)dist); y++) {
		state.pixels[X_RES*y+x] = color;
	}
	state.pixels[X_RES*72+x] = color;
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
		f32 max = 10.0f;
		f32 distf = 0.0f;
		vec2i current_tile = posi;
		while (!wall_hit&&distf<max) {
			
			if (dist.x<dist.y) {
				current_tile.x += step_dir.x;
				distf = dist.x;
				dist.x += unit_length_ratio.x;
			} else {
				current_tile.y += step_dir.y;
				distf = dist.y;
				dist.y += unit_length_ratio.y;
			}

			if (current_tile.x < MAP_SIZE && current_tile.y < MAP_SIZE && current_tile.x >= 0 && current_tile.y >= 0 ) {
				if (MAP[MAP_SIZE*current_tile.y+current_tile.x]) {
							
					printf("%i,",current_tile.x);
					printf("%i,",current_tile.y);
					printf("%i,",MAP[MAP_SIZE*current_tile.y+current_tile.x]);
					printf("%i\n",(MAP_SIZE*current_tile.y+current_tile.x));

					drawLine(x,0xFFFFFFFF,distf);
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
	state.dir = normalize(((vec2f) {0.0f,-1.0f}));
	state.plane = normalize(((vec2f) {state.dir.y,-state.dir.x}));

	while (!state.quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				state.quit = true;	
			}
		}

		SDL_RenderClear(state.renderer);

		state.pixels[X_RES*72+128] = 0xFF0000FF;
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



