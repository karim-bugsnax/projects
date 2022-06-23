#include <stdio.h> //standard in and output
#include <stdlib.h> //used for memory allocation / pointers

#include "pre_processing.h"
#include "base_structures.h"

void TestFunction(struct Bitmap* image, int i, int x);

void mainPreProcessing(struct Bitmap* image)
{
  //struct Bitmap* image = make_random_bitmap(10, 10);
  /*
  TestFunction(image, 1, 0);
  TestFunction(image, 2, 4);
  TestFunction(image, 3, 3);
  TestFunction(image, 4, 2);
  */

  // Procedure:
  // Grayscale -> ContrastEnhancement -> NoiseCancel -> DeskewAuto
  GrayScale(image); // colour -> grayscale
  ContrastEnhancement(image); // grayscale -> grayscale
  NoiseCancel(image); // grayscale -> blackwhite
  DeskewAuto(image); // black-white -> blackwhite
}

void TestFunction(struct Bitmap* image, int i, int function)
{
  switch(function)
  {
    case 0:
      GrayScale(image);
      printf("Image%d (GrayScale):", i);
      break;

    case 1:
      BlackWhite(image, 0);
      printf("Image%d (BlackWhite):", i);
      break;

    case 2:
      DeskewAuto(image);
      printf("Image%d (DeskewAuto):", i);
      break;

    case 3:
      NoiseCancel(image);
      printf("Image%d (NoiseCancel):", i);
      break;

    case 4:
      ContrastEnhancement(image);
      printf("Image%d (ContrastEnhancement):", i);
      break;

    default:
      printf("~~~~TYPO~~~~");
      break;
  }

  print_bitmap(image);
  printf("\n");
}
