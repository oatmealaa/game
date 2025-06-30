#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"
#include "utils.h"
#include "render.h"


void drawLine(int x,u32 color ,f32 dist) {
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
					//plane_dist = distf*((normalize(ray_vector).x*distf)/distf);
		

					vec2f good_ray_vector = {
						0.0f + (state.plane.x*planex),
						1.0f + (state.plane.y*planex)
					};


					plane_dist = distf*good_ray_vector.y/sqrt((good_ray_vector.x*good_ray_vector.x)+(good_ray_vector.y*good_ray_vector.y));

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



