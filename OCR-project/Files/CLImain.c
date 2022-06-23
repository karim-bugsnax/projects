//include of standard libraries
#include <stdio.h>
#include <stdlib.h>

//include of our files
#include "base_structures.h"
#include "main_pre_processing.h"
#include "pre_processing.h"
#include "main_lib.h"
#include "segmentation.h"
//#include "ML/network.h"

// Main
int CLImain(int argc, char **argv)
{
  if (argv[1][0] == 'h')
    {
      printf("H : Help\nG : Grayscale\nB or W : BlackWhite\nD : Deskew\nN : Noise cancel\n");
      printf("C : ContrastEnhancement\nS : Segmentation\nA : All, call the OCR\n(no argument) == All\n");
      return 0;
    }

  struct Bitmap* test_image = ReadBMP(argv[1]);

	if (argc == 2) {
		mainPreProcessing(test_image);
    segmentation(test_image);
    convert_2Bitmap_Save(test_image, 1);
	}

  //starting at one because ./a is the 0th element
  for (int i = 2; i < argc ; i++)
  {
      //making all cases lower
     /*if (argv[i][0] != '$')
      {
      }*/ //this is for when the training command will be fixed
      //argv[i] = SDL_strlwr(argv[i]);

      if (argv[i][0] == 'g' || argv[i][0] == 'G')
      {
          GrayScale(test_image);
          convert_2Bitmap_Save(test_image, 0);
      }
      else if (argv[i][0] == 'b' || argv[i][0] == 'w' 
					|| argv[i][0] == 'B' || argv[i][0] == 'W')
      {
          BlackWhite(test_image,0);
          convert_2Bitmap_Save(test_image, 0);
      }
      else if (argv[i][0] == 'd'  || argv[i][0] == 'D')
      {
          DeskewAuto(test_image);
          convert_2Bitmap_Save(test_image, 0);
      }
      else if (argv[i][0] == 'n' || argv[i][0] == 'N')
      {
          NoiseCancel(test_image);
          convert_2Bitmap_Save(test_image, 0);
      }
      else if (argv[i][0]== 'c'  || argv[i][0] == 'C')
      {
          ContrastEnhancement(test_image);
          convert_2Bitmap_Save(test_image, 0);
      }
      else if (argv[i][0] =='s'  || argv[i][0] == 'S')
      {
          segmentation(test_image);
          convert_2Bitmap_Save(test_image, 0);
      }
      else if (argv[i][0] == 'a'  || argv[i][0] == 'A')
      {
          mainPreProcessing(test_image);
          segmentation(test_image);
          convert_2Bitmap_Save(test_image, 1);
      }
      else if (argv[i][0] == 'h'  || argv[i][0] == 'H')
      {
          printf("G : Grayscale\nB or W : BlackWhite\nD : Deskew\nN : Noise cancel\n");
          printf("C : ContrastEnhancement\nS : Segmentation\nA : All, call the OCR\n(no argument) == All\n");
      }

      //convert_2Bitmap_Save(test_image);
      
    /*else if(argv[i][0] == '$') 
      {
          int x = atoi(argv[i+1]);//get the number after $ : example $ 500 = 500
          i+=1;
          struct Network network = load_from_disk();
          for(int j =0; j< x; j++){
            printf("Training epoch %i\n", j);
            if(j%50 ==0)
            {
              save_to_disk(&network);
            }
            bulk_train(&network, j);
          }
          save_to_disk(&network);
          
      }
      else if(argv[i][0] == '!') 
      {
        struct Network network = load_from_disk();
        test(&network);
        return 0; // train should probs end all other commands
      }*/
  }

	if (test_image != NULL)
		free_bitmap(test_image);

  return 0;
}
