#include <SDL2/SDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

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

#define dot(v0, v1)                  \
    ({ const vec2f _v0 = (v0), _v1 = (v1); (_v0.x * _v1.x) + (_v0.y * _v1.y); })
#define length(v) ({ const v2 _v = (v); sqrtf(dot(_v, _v)); })
#define normalize(u) ({              \
        const vec2f _u = (u);           \
        const f32 l = length(_u);    \
        (vec2f) { _u.x / l, _u.y / l }; \
    })
#define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b; })
#define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b; })
#define sign(a) ({                                       \
        __typeof__(a) _a = (a);                          \
        (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); \
    })

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

static u8 MAP[MAP_SIZE^2] {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,1,0,1,0,1,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
}


static void renderer() {
	for (int x = 0; x < Y_RES; x++) {
			
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
	state.pos.y = 7.0f;
	state.dir = normalize((vec2f) {0.0f,1.0f});

	while (!state.quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				state.quit = true;	
			}
		}

		SDL_RenderClear(state.renderer);

		state.pixels[X_RES*72+128] = 0xFF0000FF;

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

	}
	
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
	return 0;
}



