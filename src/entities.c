#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"
#include "entities.h"
#include "utils.h"

struct entities_s entities;



void setupEntities() {
	entities.array = calloc(10,sizeof(entity));
	entities.len = 0;
	entity crab;
	crab.texture_id = 3;
	crab.pos = (vec2f) {
		3.0f,
		3.0f
	};
	crab.behaviors[0] = FOLLOW_PLAYER;
	memcpy(&entities.array[entities.len],&crab,sizeof(entity));
	entities.len++;

	for (i32 i = 0; i <3;i++) {
		entity e;
		e.texture_id = 3;
		e.pos = (vec2f) {
			(f32)i*3,
			5.0f
		};
		e.behaviors[0] = PASSIVE;
		memcpy(&entities.array[entities.len],&e,sizeof(entity));
		entities.len++;
		
	}

}

void walk(i32 entity_i) {
	entity* target = &entities.array[entity_i]; 

	target->pos.y += 0.5*state.fixed_delta_time;
}

void followPlayer(i32 entity_i) {
	entity* target = &entities.array[entity_i]; 
	
	vec2f diff = {
		target->pos.x-state.pos.x,
		target->pos.y-state.pos.y
	};

	diff = normalize(diff);

	target->pos.x -= diff.x*state.fixed_delta_time;
	target->pos.y -= diff.y*state.fixed_delta_time;
	
}

void updateEntities() {
	for (i32 i=0; i < entities.len;i++) {
		for (i32 x = 0; i<32; i++) {
			switch (entities.array[i].behaviors[x]) {
			case WALK:
				walk(i);
				break;
			case FOLLOW_PLAYER:
				followPlayer(i);
				break;
			}
		}
	}
}

