
typedef struct texture_s {
	u32* pixels;
	i32 vres;
	i32 hres;
	i32 id
} texture;

struct textures_s {
	u32 len;
	texture* array
}; 

extern struct textures_s textures;
void loadTextures();
