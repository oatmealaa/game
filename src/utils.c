#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "utils.h"

vec2f normalize(vec2f vec) {
	f32 len = sqrt((vec.x*vec.x)+(vec.y*vec.y));
	vec2f new = {
		vec.x/fabs(len),
		vec.y/fabs(len) 
	};
	return new;
}

