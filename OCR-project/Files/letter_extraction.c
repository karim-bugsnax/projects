#include "base_structures.h"
#include <stdlib.h>
#include <stdio.h>
#include "SDL/SDL_ttf.h"
#include "SDL/SDL.h"
#include "pixel_operations.h"

struct Bitmap* letter_extract(struct Bitmap* img, 
		int l, int r, int t, int b) {
	int width = r - l + 1;
	int height = t - b + 1;
	struct Bitmap* res = make_bitmap(width, height);
	
	int new_y = -1;
	for (int y = b; y <= t; y++) {
		new_y++;
		int new_x = -1;
		for (int x = l; x <= r; x++) {
			new_x++;
			if (new_x == 0)
				res->data[new_x + new_y * res->width] = make_pixel();
			else
				res->data[new_x + new_y * res->width] = img->data[x + y * img->width];
		}
	}

	return res;
}

struct Bitmap* scale_on_28_28(struct Bitmap* img) {
	int size = 28;
	struct Bitmap* res = make_bitmap(size, size);
	
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			int newX = x * img->width / size;
			int newY = y * img->height / size;
			res->data[x + y * size] = img->data[newX + newY * img->width];
		}
	}

	free_bitmap(img);
	
	return res;	
}

struct Bitmap* letter_extract_on_28_28(struct Bitmap* img, 
		int l, int r, int t, int b) {
	struct Bitmap* res = letter_extract(img, l, r, t, b);
	return scale_on_28_28(res);
}



void save_letter(struct Bitmap* letter, int letter_number){
  SDL_Surface* surface;
  Uint32 rmask = 0;
  Uint32 gmask = 0;
  Uint32 bmask = 0;
  Uint32 amask = 0;

  surface = SDL_CreateRGBSurface(0, letter->width, letter->height, 32,
                                 rmask, gmask, bmask, amask);
  for (int x = 0; x < letter->width; x++) {
    for (int y = 0; y < letter->height; y++) {
      struct Pixel pix = letter->data[x + y * letter->width];
      Uint32 new_pixel =SDL_MapRGB(surface->format, pix.r, pix.g, pix.b);
      put_pixel(surface, x, y, new_pixel);
    }
  }
	
	char filename[20];
	sprintf(filename, "letters/%d.bmp", letter_number);
	SDL_SaveBMP(surface, filename);
	SDL_FreeSurface(surface);
}
