#ifndef MAIN_LIB_H_
#define MAIN_LIB_H_

#include "SDL/SDL_image.h"

int home_alone();
void init_sdl();
void image_open_sdl(char path[]);
SDL_Surface* load_image(char *path);
SDL_Surface* display_image(SDL_Surface *img);
void wait_for_keypressed();
void random_string(char * string, size_t length);
void SDL_FreeSurface(SDL_Surface *surface);
//void save_image(const char * image_path[], char image_save_path[]);
struct Bitmap* ReadBMP(const char* filename);
void convert_2Bitmap_Save(struct Bitmap* bmp, int mode);
SDL_Surface* SDL_CreateRGBSurface(Uint32 flags,
                                  int    width,
                                  int    height,
                                  int    depth,
                                  Uint32 Rmask,
                                  Uint32 Gmask,
                                  Uint32 Bmask,
                                  Uint32 Amask);

Uint32 SDL_MapRGB(const SDL_PixelFormat* format,
                  Uint8                  r,
                  Uint8                  g,
                  Uint8                  b);


#endif
