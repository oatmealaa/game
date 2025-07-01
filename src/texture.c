#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <strings.h>
#include "main.h"
#include "utils.h"
#include "render.h"
#include "texture.h"

#define TEXTURES_AMT 3
static char* texture_paths[TEXTURES_AMT] = 
	{"data/wall.bmp",
	"data/wall2.bmp",
	"data/Untitled.bmp"};

struct textures_s textures;

static void parseBitmap(int index,FILE* fptr) {
	i32 width = 0;
	i32 height = 0;
	i32 pixel_array_offset;
	i32 file_size = 0;
	char buff[0x1A];

	fseek(fptr,0,SEEK_SET);
	fread(buff,1,0x1A,fptr);
	
	memcpy(&width,buff+0x12,sizeof(i32));
	memcpy(&height,buff+0x16,sizeof(i32));
	memcpy(&pixel_array_offset,buff+0x0A,sizeof(i32));
	memcpy(&file_size,buff+0x02,sizeof(i32));
	
	char* image = malloc(file_size);
	
	fseek(fptr,0,SEEK_SET);
	fread(image,1,file_size,fptr);

	u32* pixels = malloc(4*width*height);
	memcpy(pixels,image+pixel_array_offset,4*width*height);
	free(image);
	
		
	textures.array[index] = (texture){pixels,height,width,index};

}


void loadTextures() {
	
	textures.len = TEXTURES_AMT;
	textures.array = calloc(TEXTURES_AMT,sizeof(texture));


	for (int x = 0; x < TEXTURES_AMT; x++) {
		FILE* fpointer;
		
		fpointer = fopen(texture_paths[x],"rb");

		if (!fpointer) {
			printf("texture %s not found or cant be opend",texture_paths[x]);
			continue;
		}
		char* header = malloc(10);

		fread(header,1,2,fpointer);
		if (header[0]==0x42&&header[1]==0x4D) {
			
			parseBitmap(x,fpointer);


		} else {
			printf("Texture file format not recognized %s", texture_paths[x]);
			continue;
		}
		
		texture text = textures.array[x];
		printf("%d, %d, %d\n",text.vres,text.hres,text.id);

		fclose(fpointer);
		
	}
}
