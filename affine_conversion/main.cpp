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
rgb_t** make_set1(FILE* outfp, vec2 size);//同一処理の関数化
void make_set2(FILE* outfp, vec2 size, rgb_t** temp, int padding);//同一処理の関数化
void make_rotated_file(FILE* outfp, Angle angle, vec2 size, rgb_t** color);//画像回転
void make_rotated2_file(FILE* outfp,char* angle, vec2 size, rgb_t** color, int padding);//画像反転
void make_inversion(FILE* outfp,vec2 size, rgb_t** color, int padding);//画像の色の反転
void make_scalling(FILE*outfp,vec2 size,rgb_t** color, int padding,double scall);//画像の拡大縮小
int main(int argc, char *argv[]){

	Angle angle = DOWN;//DOWNで初期化
	double scall = 1.5;//縮小度１未満なら拡大1以上なら縮小
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
			
		}
	}

	if(fp == NULL)
	{
		printf("Can not open.\n");//画像ファイルが開けないときのerror message 出力
		return 1;
	}

	unsigned char header[OFFSET] = {0};

	fread(header, 1, OFFSET, fp);

	if( bmp_check(header) == 1)
	{
		fclose(fp);
		printf("Diffrent file type.\n");//ファイルの先頭2byteがBMかつ1ピクセル当たりのビット長が24かつファイルヘッダと情報ヘッダの合計オフセットが54の時以外はerror message 出力
		return 1;
	}

	vec2 size = get_size(fp);
	printf("Horizontal size = %u, Vertical size = %u\n", size.x, size.y);//縦と横のサイズを表示
	int padding = (4 - COLOR_BYTE*size.x % 4) % 4;
	rgb_t **color = (rgb_t **)malloc(size.x * sizeof(rgb_t *));
    for(int i = 0; i < size.x; i++)
	{
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

	char* outfname =(char*) "rotate(-left).bmp";//ファイル名を指定
	FILE* outfp = fopen(outfname, "wb");
	fwrite(header, 1, OFFSET, outfp);
	if(argc == 4)//コマンドライン引数の数により使う関数を判別
	{
		make_rotated2_file(outfp,argv[3],size, color,padding);
	}else if(argc == 3)
	{
		make_rotated_file(outfp,angle, size, color);
	}else if(argc == 5)
	{
		make_inversion(outfp,size,color,padding);
	}else
	{
		make_scalling(outfp,size,color,padding,scall);
	}

	fclose(outfp);
	
	for(int i=0; i<size.x; i++)
	{
		free(color[i]);
	}
	free(color);
	printf("Successfully rotated.\n");//成功したらメッセージ出力
	
	return 0;
}

int bmp_check(unsigned char header[OFFSET])
{
	int unsatisfied_items = 0;
	int offset = 0;

	if(header[offset] != 'B' || header[offset+1] != 'M')//ファイルタイプがBMPかどうかチェック。ファイルヘッダの最初2バイトに格納されている
	{
		unsatisfied_items++;
	}

	offset = 0x001c;//1ピクセル当たりのビット長が24かどうかチェック。ビット長が0x001cから2バイト分格納されている。
	if(header[offset] != 8*COLOR_BYTE || header[offset+1] != 0)
	{
		unsatisfied_items++;
	}

	offset = 0x000a;//ファイルヘッダと情報ヘッダ内のオフセットが54かどうかチェック。オフセット数が0x000aから4バイト分格納されている
	if(header[offset] != OFFSET || header[offset+1] != 0 || header[offset+2] != 0 || header[offset+3] != 0)
	{
		unsatisfied_items++;
	}

	if(unsatisfied_items != 0)//上記三つの条件を一つでも満たしていない場合はじく
	{
		printf("Number of unsatisfied items = %d\n", unsatisfied_items);
		return 1;
	}
	return 0;
}

vec2 get_size(FILE *fp)
{
	vec2 v;
	fseek(fp, 0x0012, SEEK_SET);
	fread(&(v.x), 4, 1, fp);//縦幅取得
	fread(&(v.y), 4, 1, fp);//横幅取得
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

vec2 add(vec2 vector1, vec2 vector2)//make_rotated_file関数の補助関数
{
	vector1.x += vector2.x;
	vector1.y += vector2.y;
	return vector1;
}

vec2 rotate(vec2 vector, Angle angle)//make_rotated_file関数の補助関数（回転前の原点中心の90度回転の時の座標変換）
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

vec2 origin(Angle angle, vec2 size)//make_rotated_file関数の補助関数(回転前の原点の座標変換)
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
vec2 convert(vec2 vector, Angle angle, vec2 size)//make_rotated_file関数の補助関数
{
	return add(rotate(vector,angle), origin(angle, size));//回転後の原点での元の原点の座標と回転前の原点を中心とした回転座標を足すことで現在の原点を基準とした座標が求まる。
}

vec2 size_convert(Angle angle, vec2 size)//make_rotated_file関数の補助関数
{
	size = rotate(size, angle);
	if (size.x < 0) size.x = -size.x;
	if (size.y < 0) size.y = -size.y;
	return size;
}

rgb_t** make_set1(FILE* outfp, vec2 size)//同一処理の関数化
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

void make_set2(FILE* outfp, vec2 size, rgb_t** temp, int padding)//同一処理の関数化
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

void make_rotated_file(FILE* outfp, Angle angle, vec2 size, rgb_t** color)//実行時は実行可能ファイル、画像、-rightまたは-upまたは-downまたは-leftのいずれか一つを指定
{
	vec2 c_size = size_convert(angle, size);

	rgb_t** temp = make_set1(outfp,c_size);
	vec2 v;
	vec2 c_v;

	for( v.x=0; v.x<size.x; v.x++)
	{
		for(v.y=0; v.y<size.y; v.y++)
		{
			c_v = convert(v, angle, c_size);
			temp[c_v.x][c_v.y] = color[v.x][v.y]; 
		}
	}

	int padding = (4 - COLOR_BYTE*c_size.x % 4) % 4;
	fseek(outfp, OFFSET, SEEK_SET);
	make_set2(outfp,c_size,temp,padding);
}

void make_rotated2_file(FILE* outfp, char* angle, vec2 size, rgb_t** color, int padding)//実行時は実行可能ファイル、画像、任意の引数一つ,00(上下反転)または01(左右反転)を指定
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

void make_inversion(FILE* outfp,vec2 size, rgb_t** color, int padding)//実行時は実行可能ファイル、画像、任意の引数を３個指定
{
	rgb_t** temp = make_set1( outfp, size);
	for(int i = 0; i<size.y; i++)
	{
        
		for(int u = 0; u< size.x; u++) 
		{

		if(i >=0 && i < size.y && u >= 0 && u < size.x)//反転色はr,g,b値のそれぞれ255の余数を取ればよい
		{
			temp[u][i].b = 255 - color[u][i].b;
			temp[u][i].g = 255 - color[u][i].g;
			temp[u][i].r = 255 - color[u][i].r;
		}

	    }
	fseek(outfp, OFFSET, SEEK_SET);
	}
	make_set2(outfp,size,temp,padding);
}

void make_scalling(FILE*outfp,vec2 size,rgb_t** color, int padding,double scall)//実行時は実行可能ファイル、画像、任意の引数を4個指定
{
	rgb_t** temp = make_set1( outfp, size);
	for(int i=0; i<size.y;i++)
	{
		for(int j = 0; j< size.x;j++)
		{
			int scall_j = (int)(scall * j +0.5);//最近傍補完
			int scall_i = (int)(scall * i + 0.5);//最近傍補完

			if(i >=0 && i < size.y && j >= 0 && j < size.x && scall_i >=0 && scall_i < size.y && scall_j >= 0 && scall_j < size.x)//はみ出てるものはコピーしない
			{
				temp[j][i] = color[scall_j][scall_i];
			}

		}
		fseek(outfp, OFFSET, SEEK_SET);
	}
	make_set2(outfp,size,temp,padding);

}