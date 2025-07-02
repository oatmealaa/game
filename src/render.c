#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"
#include "utils.h"
#include "render.h"
#include "texture.h"
#include "entities.h"

typedef enum draw_type_e {
	LINE,
	ENTITY,
} dtype;

typedef struct draw_target_s {
	dtype type;
	vec2i pos;
	i32 x;
	f32 dist;
	f32 hit_grid_offset;
	u32 texture_id;
	u32 color_offset;
	
} draw_target;

struct {
	draw_target* targets;
	i32 length;
} draw_order;

void drawLine(int x,f32 dist, f32 hit_grid_offset, u32 texture_id, u32 color_offset) {
	f32 rep = 1/dist;
	i32 height = (int)((f32)X_RES*rep)*FOV_FACTORY;
	if (texture_id>=textures.len) {
		printf("Texture %d not found\n",texture_id);
	}



	i32 top = CENTER_Y+(height/2)+state.pan_y;
	i32 bottom = CENTER_Y-(height/2)+state.pan_y;

	i32 text_hres = textures.array[texture_id].hres;
	i32 text_vres = textures.array[texture_id].vres;
	u32* pixels = textures.array[texture_id].pixels;

	f32 voffset = 0;
	u32 text_posy = 0;
	u32 text_posx = 0;

	for (int y = 0; y < Y_RES; y++) {
		if (y<(Y_RES/2)+state.pan_y) {
			state.pixels[X_RES*y+x] = 0xFF999999;
		} else {
			state.pixels[X_RES*y+x] = 0xFFDDDDDD;
		}
	}

	for (int y = bottom; y < top; y++) {
		if (y<=0) continue;
		if (y>=Y_RES) break;

		voffset = (f32)(y-bottom)/(f32)(top-bottom);

		text_posy = (u32)((f32)text_vres*voffset);
		text_posx = (u32)((f32)text_hres*hit_grid_offset);

		state.pixels[X_RES*y+x] = pixels[(text_posy*text_vres+text_posx)];

	}
}

void drawEntity(vec2i posc,f32 dist,u32 texture_id, u32 color_offset) {
	f32 scale = 1/dist;
	i32 text_x;
	i32 text_y;
	if (texture_id>=textures.len) {
		printf("Texture %d not found\n",texture_id);
	}

	i32 text_hres = textures.array[texture_id].hres;
	i32 text_vres = textures.array[texture_id].vres;
	u32* pixels = textures.array[texture_id].pixels;
	f32 xf;
	f32 yf;

	i32 width = (i32)((f32)X_RES*scale);
	i32 height = (i32)((f32)Y_RES*scale);
		
	vec2i pos = (vec2i) {
		posc.x-(i32)((f32)(X_RES/2)*scale),
		posc.y-(i32)((f32)(Y_RES/2)*(scale))+state.pan_y
	};

	for (i32 x = pos.x; x < width+pos.x; x++) {
		if (x >= X_RES-1||x<0) continue;
		xf = (x-pos.x)/((f32)X_RES*scale);
		text_x = (u32)((f32)text_hres*xf);
		if (text_x > text_hres) continue;
		for (i32 y = pos.y; y < height+pos.y; y++) {
			if (y >= Y_RES-1||y<0) continue;
			yf = (y-pos.y)/((f32)Y_RES*scale);
			text_y = (u32)((f32)text_vres*yf);
			if (text_y > text_vres) break;
			if (pixels[text_vres*text_y+text_x]==0x00000000) continue;
			state.pixels[X_RES*y+x] = pixels[text_vres*text_y+text_x];
		}
	
	}
}

int comp(const void* a,const void* b) {
	draw_target* A = (draw_target*)b;
	draw_target* B = (draw_target*)a; 

	return (i32)(A->dist - B->dist);
}

void render() {

	draw_order.length = 0;	
	draw_order.targets = calloc(X_RES+entities.len,sizeof(draw_target));

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
			draw_target target;
			target.type = LINE;
			target.x = x;
			target.dist = plane_dist;
			target.hit_grid_offset = grid_offset;
			target.texture_id = MAP[MAP_SIZE*current_tile.y+current_tile.x]-1;
			target.color_offset = 0xFF000000;
			memcpy(&draw_order.targets[draw_order.length],&target,sizeof(draw_target));
			draw_order.length++;

		} else {
			grid_offset = hit_pos.y-(f32)((int)hit_pos.y);
			draw_target target;
			target.type = LINE;
			target.x = x;
			target.dist = plane_dist;
			target.hit_grid_offset = grid_offset;
			target.texture_id = MAP[MAP_SIZE*current_tile.y+current_tile.x]-1;
			target.color_offset = 0xFF000000;
			memcpy(&draw_order.targets[draw_order.length],&target,sizeof(draw_target));
			draw_order.length++;
		}
	}


	
	for (u32 i = 0; i<entities.len;i++) {
		draw_target entity;	
		
		entity.texture_id = entities.array[i].texture_id;
		vec2f pos = entities.array[i].pos;
		
		vec2f diff = (vec2f) {
			pos.x-state.pos.x,
			pos.y-state.pos.y
		};
		

		f32 dot = (state.dir.x*diff.x)+(state.dir.y*diff.y);
		f32 det = (state.dir.x*diff.y)-(state.dir.y*diff.x);

		f32 angle = atan2(det,dot);
		
		f32 fovx = atan(FOV_FACTORX/1);


		
		f32 f = (angle+fovx)/(fovx*2);
		f = (f-1)*-1;	

		entity.pos.x = (i32)((f32)X_RES*f);
		entity.pos.y = (i32)((f32)Y_RES/2);
		entity.dist = sqrt((diff.x*diff.x)+(diff.y*diff.y));
		entity.color_offset = 0x00000000;
		entity.type = ENTITY;
		
		if (f<-0.5||f>1.5) continue;
		//drawEntity((vec2i){entity.pos.x,Y_RES/2},entity.dist,entity.texture_id,0);
		memcpy(&draw_order.targets[draw_order.length],&entity,sizeof(draw_target));
		draw_order.length++;

	}

	qsort(draw_order.targets,draw_order.length,sizeof(draw_target),comp);

	for (int i = 0; i < draw_order.length; i++) {
		draw_target target = draw_order.targets[i];
		dtype type = target.type;
		if (type == LINE) {
			drawLine(target.x,target.dist,target.hit_grid_offset,target.texture_id,target.color_offset);
			
		}

		if (type == ENTITY) {
			drawEntity(target.pos,target.dist,target.texture_id,target.color_offset);
		}
	}


	
	free(draw_order.targets);
}



