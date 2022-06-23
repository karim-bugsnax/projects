#ifndef BASE_STRUCTURES_H
#define BASE_STRUCTURES_H

typedef struct Pixel {
	int r;
	int g;
	int b;
} Pixel;

typedef struct Bitmap {
	int width; //x is width || width
	int height; //y is height || rows
	struct Pixel* data; 
	//one dmensional array of size width * height
	//to get [x][y] we ow need to use [x + y * width]
	//for loop: for (int i = 0; i < image.width * image.height; i++)
} Bitmap;

struct Pixel make_pixel(void);
struct Pixel dupli_pixel(struct Pixel pix);
struct Bitmap* make_bitmap(int rows, int columns);
struct Bitmap* dupli_bitmap(struct Bitmap* image);
void free_bitmap(struct Bitmap* img);
void print_bitmap(struct Bitmap* image);

struct Bitmap* make_random_bitmap(int rows, int columns);

#endif
