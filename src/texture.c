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

#define TEXTURES_AMT 2
static char* texture_paths[TEXTURES_AMT] = 
	{"data/wall.bmp"
	,"data/wall2.bmp"};


texture parseBitmap(FILE* fptr) {
	i32 width = 0;
	i32 height = 0;

	char buff[0x1A];
	fread(buff,1,0x1A,fptr);

	memcpy(&width,buff+0x12,sizeof(i32));
	memcpy(&height,buff+0x16,sizeof(i32));

}


void loadTextures() {
	
	for (int x = 0; x < TEXTURES_AMT; x++) {
		FILE* fpointer;

		fpointer = fopen(texture_paths[x],"r");

		if (fpointer==NULL) {
			printf("texture %s not found or cant be opend",texture_paths[x]);
		}
		
		char header[2];

		fread(header,1,2,fpointer);

		if (header[0]==0x42&&header[1]==0x4D) {
			parseBitmap(fpointer);		
		} else {
			printf("Texture file format not recognized %s", texture_paths[x]);
		}


		
	}
}
