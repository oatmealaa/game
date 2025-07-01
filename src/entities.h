
typedef enum behavior_e {
	FOLLOW_PLAYER=1,
	PASSIVE,
	WALK
} behavior;

typedef struct entity_s {
	behavior behaviors[32];
	u32 texture_id;
	vec2f pos;

} entity;

struct entities_s {
	u32 len;
	entity* array;
};

extern struct entities_s entities;

void setupEntities();

void updateEntities();
