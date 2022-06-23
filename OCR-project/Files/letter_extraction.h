#ifndef LETTER_EXTRACTION_H
#define LETTER_EXTRACTION_H

struct Bitmap* letter_extract_on_28_28(struct Bitmap* img, 
		int l, int r, int t, int b);
void save_letter(struct Bitmap* letter, int letter_number);

#endif
