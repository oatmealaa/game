#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"
#include "utils.h"
#include "render.h"
#include "texture.h"

void drawLine(int x,u32 color ,f32 dist, f32 hit_grid_offset, u32 texture_id) {
	f32 rep = 1/dist;
	i32 height = (int)((f32)X_RES*rep)*FOV_FACTORY;
	
	i32 top = CENTER_Y+(height/2);
	i32 bottom = CENTER_Y-(height/2);

	i32 text_hres = textures.array[texture_id*sizeof(texture)].hres;
	i32 text_vres = textures.array[texture_id*sizeof(texture)].vres;
	u32* pixels = textures.array[texture_id*sizeof(texture)].pixels;

	for (int y = CENTER_Y; y < top; y++) {
		if (y>=Y_RES) break;
		
		f32 voffset = (f32)(y-bottom)/(f32)(top-bottom);
		

		u32 text_posy = (u32)((f32)text_vres*voffset);
		u32 text_posx = (u32)((f32)text_hres*hit_grid_offset);

		state.pixels[X_RES*y+x] = pixels[(text_posy*text_vres+text_posx)];
	}
	for (int y = CENTER_Y; y > bottom; y--) {
		


		if (y<=0) break;
		f32 voffset = (f32)(y-bottom)/(f32)(top-bottom);

		u32 text_posy = (u32)((f32)text_vres*voffset);
		u32 text_posx = (u32)((f32)text_hres*hit_grid_offset);

		state.pixels[X_RES*y+x] = pixels[(text_posy*text_vres+text_posx)];

	}
}

void render() {
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
		i32 hit = 0;
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
					hit = MAP[MAP_SIZE*current_tile.y+current_tile.x];
					break;
				} 
			} 

		}
		
		if (!hit) continue;

		f32 plane_dist;
	
		vec2f good_ray_vector = {
			0.0f + (state.plane.x*planex),
			1.0f + (state.plane.y*planex)
		};


		plane_dist = distf*good_ray_vector.y/sqrt((good_ray_vector.x*good_ray_vector.x)+(good_ray_vector.y*good_ray_vector.y));

		vec2f normal_ray = normalize(ray_vector);

		vec2f hit_vector = {
		normal_ray.x*distf,
		normal_ray.y*distf
		};

		vec2f hit_pos = {
			pos.x+hit_vector.x,
			pos.y+hit_vector.y
		};

		f32 grid_offset;
					

		if (!side) {
			grid_offset = hit_pos.x-(f32)((int)hit_pos.x);
			drawLine(x,0xFFFF00FF,plane_dist,grid_offset,MAP[MAP_SIZE*current_tile.y+current_tile.x]-1);
		} else {
			grid_offset = hit_pos.y-(f32)((int)hit_pos.y);
			drawLine(x,0xFFEE00EE,plane_dist,grid_offset,MAP[MAP_SIZE*current_tile.y+current_tile.x]-1);
		}
	}
}



