#ifndef PRE_PROCESSING_H
#define PRE_PROCESSING_H
#include "base_structures.h"

void GrayScale(struct Bitmap* image);
void BlackWhite(struct Bitmap* image, int base_threshold);
void DeskewAuto(struct Bitmap* image);
void NoiseCancel(struct Bitmap* image);
void ContrastEnhancement(struct Bitmap* image);

#endif
