#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define STRINGLENGTH 128

#define OFFSET 54
#define COLOR_BYTE 3

int bmp_check(unsigned char header[OFFSET]);


typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} rgb_t;

rgb_t get_color(FILE *fp, unsigned int offset);
void print_color(rgb_t color);

typedef struct{
	int x;
	int y;
} vec2;

typedef enum{ UP=0, RIGHT=90, DOWN=180, LEFT=270 } Angle;

vec2 get_size(FILE *fp);

vec2 rotate(vec2 vector, Angle angle);
vec2 origin(Angle angle, vec2 size);
vec2 convert(vec2 vector, Angle angle, vec2 size);
vec2 size_convert(Angle angle, vec2 size);
rgb_t** make_set1(FILE* outfp, vec2 size);
void make_set2(FILE* outfp, vec2 size, rgb_t** temp, int padding);
void make_rotated_file(FILE* outfp, Angle angle, vec2 size, rgb_t** color);
void make_rotated2_file(FILE* outfp,char* angle, vec2 size, rgb_t** color, int padding);
void make_inversion(FILE* outfp,vec2 size, rgb_t** color, int padding);
int main(int argc, char *argv[]){

	Angle angle = DOWN;
	double rad;
	char fname[STRINGLENGTH] = "sample_kinoko.bmp";
	FILE* fp;
	if(argc == 1)
	{
		printf("filename ? :");
		scanf("%s", fname);
		fp = fopen(fname, "rb");
	}
	else
	{
		fp = fopen(argv[1], "rb");
		if(argc >= 3)
		{
			
			if(strcmp(argv[2], "-right") == 0)
			{
				angle = RIGHT;
			}	
			else if(strcmp(argv[2], "-left") == 0)
			{ 
				angle = LEFT;
			}	
			else if(strcmp(argv[2], "-up") == 0)
			{ 
				angle = UP;
			}	
			else if(strcmp(argv[2], "-down") == 0)
			{ 
				angle = DOWN;
			}	
			else{
				int angle = atoi(argv[2]);
			}	
		}
	}

	if(fp == NULL){
		printf("Can not open.\n");
		return 1;
	}

	unsigned char header[OFFSET] = {0};

	fread(header, 1, OFFSET, fp);

	if( bmp_check(header) == 1){
		fclose(fp);
		printf("Diffrent file type.\n");
		return 1;
	}

	vec2 size = get_size(fp);
	printf("Horizontal size = %u, Vertical size = %u\n", size.x, size.y);
	int padding = (4 - COLOR_BYTE*size.x % 4) % 4;
	rgb_t **color = (rgb_t **)malloc(size.x * sizeof(rgb_t *));
    for(int i = 0; i < size.x; i++) {
        color[i] = (rgb_t *)malloc(size.y * sizeof(rgb_t));
    }

	fseek(fp, OFFSET, SEEK_SET);
	for(int y=0; y<size.y; y++)
	{
		for(int x=0; x<size.x; x++)
		{
			fread(&color[x][y].b, 1, 1, fp);
			fread(&color[x][y].g, 1, 1, fp);
			fread(&color[x][y].r, 1, 1, fp);
		}
		fseek(fp, padding, SEEK_CUR);
	}

	fclose(fp);

	char* outfname = "23B13163-ex11-16.bmp";
	FILE* outfp = fopen(outfname, "wb");
	fwrite(header, 1, OFFSET, outfp);
	if(argc == 4)
	{
		make_rotated2_file(outfp,argv[3],size, color,padding);
	}else if(argc == 3)
	{
		make_rotated_file(outfp,angle, size, color);
	}else{
		make_inversion(outfp,size,color,padding);
	}

	fclose(outfp);

	for(int i=0; i<size.x; i++)
	{
		free(color[i]);
	}
	free(color);
	printf("%d\n",angle);
	printf("Successfully rotated.\n");
	
	return 0;
}

int bmp_check(unsigned char header[OFFSET])
{
	int unsatisfied_items = 0;
	int offset = 0;

	if(header[offset] != 'B' || header[offset+1] != 'M'){
		unsatisfied_items++;
	}

	offset = 0x001c;
	if(header[offset] != 8*COLOR_BYTE || header[offset+1] != 0){
		unsatisfied_items++;
	}

	offset = 0x000a;
	if(header[offset] != OFFSET || header[offset+1] != 0 || header[offset+2] != 0 || header[offset+3] != 0){
		unsatisfied_items++;
	}

	if(unsatisfied_items != 0){
		printf("Number of unsatisfied items = %d\n", unsatisfied_items);
		return 1;
	}
	return 0;
}

vec2 get_size(FILE *fp)
{
	vec2 v;
	fseek(fp, 0x0012, SEEK_SET);
	fread(&(v.x), 4, 1, fp);
	fread(&(v.y), 4, 1, fp);
	return v;
}

rgb_t get_color(FILE *fp, unsigned int offset)
{
	rgb_t color;
	fseek(fp, offset + OFFSET, SEEK_SET);
	fread(&color.b, 1, 1, fp);
	fread(&color.g, 1, 1, fp);
	fread(&color.r, 1, 1, fp);
	return color;
}

void print_color(rgb_t color)
{
	printf("(R=%d, G=%d, B=%d)\n", color.r, color.g, color.b);
}

vec2 add(vec2 vector1, vec2 vector2)
{
	vector1.x += vector2.x;
	vector1.y += vector2.y;
	return vector1;
}

vec2 rotate(vec2 vector, Angle angle)
{
	if (angle == 0)
	{ 
		return vector;
	}
	int temp;
	for(int i=0; i< (int)angle/90; i++)
	{
		temp = vector.x;
		vector.x = vector.y;
		vector.y = -temp;
	}
	return vector;
}

vec2 origin(Angle angle, vec2 size)
{
	switch(angle)
	{
		case UP:
			size.x = 0;
			size.y = 0;
			break;
		case RIGHT:
			size.x = 0;
			size.y -= 1;
			break;
		case DOWN:
			size.x -= 1;
			size.y -= 1;
			break;
		case LEFT:
			size.x -= 1;
			size.y = 0;
			break;
	}
	return size;
}
vec2 convert(vec2 vector, Angle angle, vec2 size)
{
	return add(rotate(vector,angle), origin(angle, size));
}

vec2 size_convert(Angle angle, vec2 size)
{
	size = rotate(size, angle);
	if (size.x < 0) size.x = -size.x;
	if (size.y < 0) size.y = -size.y;
	return size;
}


void make_rotated_file(FILE* outfp, Angle angle, vec2 size, rgb_t** color)
{
	vec2 c_size = size_convert(angle, size);

	fseek(outfp, 0x0012, SEEK_SET);
	fwrite(&c_size.x, 4, 1, outfp);
	fwrite(&c_size.y, 4, 1, outfp);

	rgb_t **c_color = (rgb_t **)malloc(c_size.x * sizeof(rgb_t *));
    for(int i = 0; i < c_size.x; i++) {
        c_color[i] = (rgb_t *)malloc(c_size.y * sizeof(rgb_t));
    }

	vec2 v;
	vec2 c_v;

	for( v.x=0; v.x<size.x; v.x++)
	{
		for(v.y=0; v.y<size.y; v.y++)
		{
			c_v = convert(v, angle, c_size);
			c_color[c_v.x][c_v.y] = color[v.x][v.y]; 
		}
	}

	int padding = (4 - COLOR_BYTE*c_size.x % 4) % 4;
	fseek(outfp, OFFSET, SEEK_SET);

	for(int y=0; y<c_size.y; y++)
	{
		for(int x=0; x<c_size.x; x++)
		{
			fwrite(&c_color[x][y].b, 1, 1, outfp);
			fwrite(&c_color[x][y].g, 1, 1, outfp);
			fwrite(&c_color[x][y].r, 1, 1, outfp);
		}
		fseek(outfp, padding, SEEK_CUR);
	}
	for(int i=0; i<c_size.x; i++)
	{
		free(c_color[i]);
	}
	free(c_color);
}
rgb_t** make_set1(FILE* outfp, vec2 size)
{
	fseek(outfp, 0x0012, SEEK_SET);
	fwrite(&size.x, 4, 1, outfp);
	fwrite(&size.y, 4, 1, outfp);

	rgb_t **c_color = (rgb_t **)malloc(size.x * sizeof(rgb_t *));
    for(int i = 0; i < size.x; i++) 
	{
        c_color[i] = (rgb_t *)malloc(size.y * sizeof(rgb_t));
    }
	return c_color;
}
void make_set2(FILE* outfp, vec2 size, rgb_t** temp, int padding)
{
	for(int y=0; y<size.y; y++)
	{
		for(int x=0; x<size.x; x++)
		{
			fwrite(&temp[x][y].b, 1, 1, outfp);
			fwrite(&temp[x][y].g, 1, 1, outfp);
			fwrite(&temp[x][y].r, 1, 1, outfp);
		}
		fseek(outfp, padding, SEEK_CUR);
	}
	for(int i=0; i<size.x; i++)
	{
		free(temp[i]);
	}
	free(temp);
}
void make_rotated2_file(FILE* outfp, char* angle, vec2 size, rgb_t** color, int padding)
{
	rgb_t** temp = make_set1( outfp, size);
	if(strcmp(angle,"00") == 0)
	{
		for(int i = 0; i<size.y; i++)
	    {

		    for(int u = 0; u< size.x; u++) 
		    {
			int ver_i = size.y -1 -i;
			if(i >=0 && i < size.y && u >= 0 && u < size.x)
			{
				temp[u][i] = color[u][ver_i];
			}
	        }
	    fseek(outfp, OFFSET, SEEK_SET);
	    }
	}else if(strcmp(angle,"01") == 0)
	{
		for(int i = 0; i<size.y; i++)
	    {
		    for(int u = 0; u< size.x; u++) 
		    {
			int ver_u = size.x -1- u;
			if(i >=0 && i < size.y && u >= 0 && u < size.x)
			{
				temp[u][i] = color[ver_u][i];
			}
	        }
	        fseek(outfp, OFFSET, SEEK_SET);
	    }
	}
	
	make_set2(outfp,size,temp,padding);
}
void make_inversion(FILE* outfp,vec2 size, rgb_t** color, int padding)
{
	fseek(outfp, 0x0012, SEEK_SET);
	fwrite(&size.x, 4, 1, outfp);
	fwrite(&size.y, 4, 1, outfp);

	rgb_t **c_color = (rgb_t **)malloc(size.x * sizeof(rgb_t *));
    for(int i = 0; i < size.x; i++) 
	{
        c_color[i] = (rgb_t *)malloc(size.y * sizeof(rgb_t));
    }
	for(int i = 0; i<size.y; i++)
	{

		for(int u = 0; u< size.x; u++) 
		{
		int ver_i = size.y -1 -i;
		if(i >=0 && i < size.y && u >= 0 && u < size.x)
		{
			c_color[u][i].b = 255 - color[u][i].b;
			c_color[u][i].g = 255 - color[u][i].g;
			c_color[u][i].r = 255 - color[u][i].r;
		}
	    }
	fseek(outfp, OFFSET, SEEK_SET);
	}
	for(int y=0; y<size.y; y++)
	{
		for(int x=0; x<size.x; x++)
		{
			fwrite(&c_color[x][y].b, 1, 1, outfp);
			fwrite(&c_color[x][y].g, 1, 1, outfp);
			fwrite(&c_color[x][y].r, 1, 1, outfp);
		}
		fseek(outfp, padding, SEEK_CUR);
	}
	for(int i=0; i<size.x; i++)
	{
		free(c_color[i]);
	}
	free(c_color);
}