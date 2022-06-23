//include of standard libraries
#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL_ttf.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <err.h>
#include <math.h>
#include "SDL/SDL.h"
#include <ctype.h>

//for random generation
#define LENGTH  26

//include of our files
#include "base_structures.h"
#include "main_lib.h"
#include "pixel_operations.h"


int home_alone() { //getting home directory, if needed for anything
    char *homedir = getenv("HOME");

    if (homedir != NULL) {
        printf("Home dir in enviroment");
        printf("%s\n", homedir);
    }

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (pw == NULL) {
        printf("Failed, much like Karim's life really...\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", pw->pw_dir); //(pointer_name)->(variable_name)

    return 0;
}

void image_open_sdl(char path[])//the use of SDL_Quit() to close all SDL systems.
{
    SDL_Surface* image_surface;
    SDL_Surface* screen_surface;
    init_sdl();
    image_surface = load_image(path);
    screen_surface = display_image(image_surface);
    wait_for_keypressed();

    SDL_FreeSurface(image_surface);
    SDL_FreeSurface(screen_surface);
}

void init_sdl()
{
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface* load_image(char *path)
{
    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}

SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;

    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL)
    {
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                img->w, img->h, SDL_GetError());
    }

    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    return screen;
}

//void SDL_FreeSurface(SDL_Surface *surface);

void wait_for_keypressed()
{
    SDL_Event event;

    // Wait for a key to be down.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYDOWN);

    // Wait for a key to be up.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYUP);
}
/*
void save_image(const char * image_path[], char image_save_path[])
{
    char file[] = image_path;
    SDL_Surface *image = IMG_Load(file);
    SDL_SaveBMP(image, image_save_path);
    SDL_FreeSurface(image); // freeing memeory
}*/


struct Bitmap* ReadBMP(const char* filename){
    SDL_Surface * image = IMG_Load(filename);

  if (!image)
        errx(3, "can't load %s: %s", filename, IMG_GetError());

    int height = image->h;
    int width = image->w;
    Uint8 r,g,b;

    struct Bitmap* bmp = make_bitmap(width, height);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Uint32 pixel = get_pixel(image, x, y);
            SDL_GetRGB(pixel,image->format, &r ,&g, &b);

            struct Pixel newpix = make_pixel();
            newpix.r = (int)r;
            newpix.g = (int)g;
            newpix.b = (int)b;
            bmp->data[x + y * width] = newpix;
        }
    }

    SDL_FreeSurface(image);
    //SDL_Quit();
    return bmp;
}

void random_string(char * string, size_t length)
{
    char pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t poolsize = strlen(pool);
    size_t index;
    size_t i;

    srand((unsigned)time(NULL));

    for(i = 0; i < length-4 && poolsize > 0; ++i)
    {
        index = rand() % poolsize;       // a random index into the pool
        string[i] = pool[index];         // take that character
        pool[index] = pool[--poolsize];  // replace it with the last pool ...
    }
    string[length-4] = '.';
    string[length-3] ='b';
    string[length-2] = 'm';
    string[length-1] ='p';
    string[length] = '\0';
}

//24 bits would need to change 32 to 24
void convert_2Bitmap_Save(struct Bitmap* bmp, int mode){
  
	// mode == 0: saving as name processed.bmp; else: radomized

	SDL_Surface* surface;
  Uint32 rmask = 0;
  Uint32 gmask = 0;
  Uint32 bmask = 0;
  Uint32 amask = 0;
  //or 4*width

  surface = SDL_CreateRGBSurface(0, bmp->width, bmp->height, 32,
                                 rmask, gmask, bmask, amask);
    //Uint16 *pixels = (Uint16 *) surface->pixels;// Get the pixels from the
		// Surface; Iterrate through the pixels and chagne the color
  for (int x = 0; x < bmp->width; x++) {
    for (int y = 0; y < bmp->height; y++) {
      struct Pixel pix = bmp->data[x + y * bmp->width];
      Uint32 new_pixel =SDL_MapRGB(surface->format, pix.r, pix.g, pix.b);
      put_pixel(surface, x, y, new_pixel);
      //pixels[i] = SDL_MapRGB(surface->format, rr, gg, bb);
    }
  }
  /*int width3 = wide + wide +wide;
    surface = SDL_CreateRGBSurfaceFrom(bmp->data, bmp->width, bmp->height,
      width3, 8, rmask, gmask, bmask, amask);*/


  if (mode == 0)
		SDL_SaveBMP(surface, "processed.bmp");
	else {
		char s[LENGTH + 1];                  // adequate length
    random_string(s, LENGTH);
    SDL_SaveBMP(surface, s);
	}
	SDL_FreeSurface ( surface );
}
