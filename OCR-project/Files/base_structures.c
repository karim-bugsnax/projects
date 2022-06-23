#include <stdio.h>
#include <stdlib.h>
#include "base_structures.h"

struct Pixel make_pixel(void) {
  struct Pixel new_pixel;
  new_pixel.r = 255;
  new_pixel.g = 255;
  new_pixel.b = 255;

  return new_pixel;
}

struct Pixel dupli_pixel(struct Pixel pix) {
  struct Pixel new_pix = make_pixel();

  new_pix.r = pix.r;
  new_pix.g = pix.g;
  new_pix.b = pix.b;

  return new_pix;
}

struct Bitmap* make_bitmap(int columns, int rows) {
  struct Bitmap* new_bitmap = malloc(sizeof(struct Bitmap));
  new_bitmap->width = columns;
  new_bitmap->height = rows;

  new_bitmap->data = malloc(sizeof(struct Pixel)*rows*columns);

  for (int i = 0; i < columns * rows; i++)
    new_bitmap->data[i] = make_pixel();

  return new_bitmap;
}

struct Bitmap* dupli_bitmap(struct Bitmap* image) {
  struct Pixel *data = image->data;
  int columns = image->width;
  int rows = image->height;

  struct Bitmap *new_bitmap = make_bitmap(columns, rows);
  for (int i = 0; i < rows * columns; i++)
    new_bitmap->data[i] = data[i];

  return new_bitmap;
}

void free_bitmap(struct Bitmap* img) {
	free(img->data);
	free(img);
}

void print_bitmap(struct Bitmap* image) {
  for(int x = 0; x < image->width; x++) {
    printf("\n");
    for(int y = 0; y < image->height; y++) {
      struct Pixel Pix = dupli_pixel(image->data[x+y*image->width]);
      printf("(%d, %d, %d)", Pix.r, Pix.g, Pix.b);
    }
  }
  printf("\n");
}

struct Bitmap* make_random_bitmap(int columns, int rows) {
  struct Bitmap* new_bitmap = make_bitmap(columns, rows);

  struct Pixel newPix = make_pixel();

  for (int i = 0; i < rows * columns; i++) {
    newPix = new_bitmap->data[i];

    newPix.r = rand() % 256;
    newPix.g = rand() % 256;
    newPix.b = rand() % 256;
    new_bitmap->data[i] = newPix;
  }

  return new_bitmap;
}
