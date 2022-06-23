#include <stdio.h> //standard in and output
#include <stdlib.h> //used for memory allocation / pointers
#include <math.h> 
	// abs(), floor(), round(), ceil(), sin(), cos(), sqrt(), trunc()

#include "base_structures.h"

void GrayScale(struct Bitmap* image)                             // Gray-Scale
{
	struct Pixel newPix = make_pixel();
	
	int graytone;

	for (int i = 0; i < image->width * image->height; i++) {
		newPix = image->data[i];
		graytone = (int)(newPix.r * 0.299 + newPix.g * 0.587 
				+ newPix.b * 0.114);
		
		newPix.r = graytone;
		newPix.g = graytone;
		newPix.b = graytone;
		image->data[i] = newPix;
	}
}





void BlackWhite(struct Bitmap* image, int base_treshold)        // Black-White
{
	struct Pixel newPix = make_pixel();
	
	int newColour;
	int threshold;
	
	// Choose Threshold and compute if necessary
	// 0: 127; else: average intensity (Binarization)
	if (base_treshold == 0)
		threshold = 127;
	else {
		int pixel_intensity_sum =0;
		int pixel_count = 0;
		for (int i = 0; i < image->width * image->height; i++) {
			pixel_intensity_sum += image->data[i].r;
			pixel_count += 1;
		}
		threshold = pixel_intensity_sum / pixel_count;
	}

	// go through each pixel and change colour based on threshold
	for (int i = 0; i < image->width * image->height; i++) {
		newPix = image->data[i];
			
		if (newPix.r > threshold)
			newColour = 255;
		else
			newColour = 0;

		newPix.r = newColour;
		newPix.g = newColour;
		newPix.b = newColour;
		image->data[i] = newPix;
	}
}





void DeskewMan(struct Bitmap* image, float angle)           // Deskew Manually
{	
	angle *= 3.14159265 / 180;
		
	struct Bitmap* new_image = make_bitmap(image->width, image->height);
	
	int rotCenterX = image->width/2;
	int rotCenterY = image->height/2;
	
	//double cosAngle = cos(angle);
	double sinAngle = sin(angle);
	double tanAngle = tan(angle/2);
	
	for (int x = 0; x < image->width; x++) {
		for (int y = 0; y < image->height; y++) {
			// Aliasing / Shearing
			int new_x = x - rotCenterX;
			int new_y = y - rotCenterY;

			//Shear1
			new_x = new_x - new_y * tanAngle;

			//Shear2
			new_y = new_x * sinAngle + new_y;

			//Shear3
			new_x = new_x - new_y * tanAngle;

			//Translate (Center)
			new_y = rotCenterY + new_y;
			new_x = rotCenterX + new_x;

			if (new_x >= 0 && new_x < image->width && 
					new_y >= 0 && new_y < image->height) {
				new_image->data[    x +     y * new_image->width] = 
					image->data[new_x + new_y * image->width];	
			}
			
			/*
			int new_x = cosAngle*(x-rotCenterX) + sinAngle*(y-rotCenterY) 
				+ rotCenterX;
			int new_y = -sinAngle*(x-rotCenterX) + cosAngle*(y-rotCenterY)
				+ rotCenterY;
			if (new_x >= 0 && new_x < image->width && 
					new_y >= 0 && new_y < image->height) {
				new_image->data[new_x + new_y * new_image->width] = 
					image->data[    x +     y * image->width];
			}
			*/
		}
	}
	
	for (int i = 0; i < image->width * image->height; i++)
		image->data[i] = new_image->data[i];

	free_bitmap(new_image);
}





void DeskewAuto(struct Bitmap* image)                  // Deskew Automatically
{
	// We create the hough transform matrix
	// Hough Matrix M x N (M values of the radius, N values of alpha)
	int hough_width = ceil(sqrt(pow(image->width,2) + pow(image->height,2)));
	int hough_height = 201;
	int hough[hough_width][hough_height]; 

	for (int x = 0; x < hough_width; x++)
		for (int y = 0; y < hough_height; y++)
			hough[x][y] = 0;
	

	// The actual algorithm starts here
	float alpha = -20;
	float pi = 3.14159265;

	for (int x = 0; x < image->width; x++) {
		for (int y = 0; y < image->height; y++) {
			if (image->data[x + y*image->width].r == 0 
					&& image->data[x + 
					(y+1)*image->width].r == 255) {
				// for black pixles (and pixel below white), 
				// we do -20 <= alpha <= 20
				// calculate their d, and hough matrix there +1
				for (int i = 0; i <= 200; i++) {
					float tmp_a = alpha + 0.2 * i;
					tmp_a = round(tmp_a * 100)/100;
					
					int d = trunc( y * cos(tmp_a * pi / 180) 
							     - x * sin(tmp_a * pi / 180));

					hough[d][i] += 1;
				}
			}
		}
	}
	
	//Find top 20 (alpha,d) pairs with highest count in Hough Matrix
	int amount_pairs = 20;
	int top_pairs[amount_pairs][2];
	for (int j = 0; j < amount_pairs; j++) { // set list to zero
		top_pairs[j][0] = 0; // in top_pairs: 20 * [i, hough[d][i]]
		top_pairs[j][1] = 0; // remeber: alpha = -20 + 0.2*i (we store i)
	}

	for (int x = 0; x < hough_width; x++) { //we add values from hough matrix
		for (int y = 0; y < hough_height; y++) {
			int count = hough[x][y];
			if (count > top_pairs[amount_pairs-1][1]) {
				top_pairs[amount_pairs-1][0] = y; // add to list
				top_pairs[amount_pairs-1][1] = count;
				
				int i = amount_pairs-1; // sort list
				while (i > 0 && top_pairs[i][1] > top_pairs[i-1][1]) {
					int tmp1 = top_pairs[i][0];
					int tmp2 = top_pairs[i][1];
					top_pairs[i][0] = top_pairs[i-1][0]; // swap
					top_pairs[i][1] = top_pairs[i-1][1];
					top_pairs[i-1][0] = tmp1;
					top_pairs[i-1][1] = tmp2;
					i--;
				}
			}
		}
	}
	
	//Calculate skew angle as average of the alphas (from top 20)
	float average_i = 0;
	for (int i = 0; i < amount_pairs; i++)
		average_i += top_pairs[i][0];
	
	average_i = average_i / amount_pairs;

	// we convert the average i back to an angle
	float deskew_angle =  alpha + 0.2 * average_i;

	// We now want to rotate by skew angle (DeskewMan)
	DeskewMan(image, deskew_angle);
}





void NoiseCancel(struct Bitmap* image)                     // Noise Cancelling
{	
	// Idea: we apply a 3x3 matrix on each pixel; from those neighbouring
	// values, if the smallest intensity occurs only once; we make the 
	// new (x,y) pixel have the value of median of those neighbouring pixels

	struct Bitmap* new_image = dupli_bitmap(image);

    int A_min = -1; // to simulate a 3x3 matrix
    int A_max = 1;

    for (int x = 0; x < image->width; x++) {
	    for (int y = 0; y < image->height; y++) {
            
			int intensity_values[9] = {256}; // 9 because 3x3 matrix
			int index = 0;        // 256 = placeholder for all (= not defined)
			
			for (int i = A_min; i <= A_max; i++) {
            	
				int tmp_x = x + i;
            	
				if (tmp_x >= 0 && tmp_x < image->width) {
					for (int j = A_min; j <= A_max; j++) {
						
						int tmp_y = y + j;
				    	
						if (tmp_y >= 0 && tmp_y < image->height) {
                			intensity_values[index] = 
								image->data[tmp_x + tmp_y * image->width].r;
							index++;
			    		}
           			}
        		}
	    	}

			// sort the intensity_values list (Bubblesort)
			int i = 0;
			int c = 0;
			int border = 1; // inedx still stores the value of the length
			while (c < index - 1) {
				while (i < index - border) {
					if (intensity_values[i] > intensity_values[i+1]) {
						int tmp = intensity_values[i];
						intensity_values[i] = intensity_values[i-1];
						intensity_values[i-1] = tmp;
					}
					i += 1;
				}
				border += 1;
				c += 1;
				i = 0;
			}


			// we count the occurence of the smallest value
			int min_value = intensity_values[0];
			int count_occ = 0;

			for (i = 0; i < index; i++)
				if (intensity_values[i] == min_value)
					count_occ += 1;

			// if appears only once (K-Value), we replace Pix(x,y) with median
			if (count_occ == 1) {
				int median_value = intensity_values[index/2];
				struct Pixel newPix = make_pixel();
				newPix.r = median_value;
				newPix.g = median_value;
				newPix.b = median_value;

				new_image->data[x + y * new_image->width] = newPix;
			}

			//free(intensity_values);
		}
	}

	for (int i = 0; i < image->width * image->height; i++)
		image->data[i] = new_image->data[i];

	free_bitmap(new_image);

	BlackWhite(image, 1); // we apply binarization
}





void __Contrast_Enhancement_Method1(struct Bitmap* image);
void __Contrast_Enhancement_Method2(struct Bitmap* image);
void __Contrast_Enhancement_Method3(struct Bitmap* image);
void ContrastEnhancement(struct Bitmap* image)         // Contrast Enhancement 
{
	__Contrast_Enhancement_Method3(image);
	__Contrast_Enhancement_Method2(image);
	//__Contrast_Enhancement_Method2(image);
}

void __Contrast_Enhancement_Method1(struct Bitmap* image) // Contrast Value
{
	struct Pixel newPix = make_pixel();
	
	float contrast = 10; //????
	float sum = 0;
	float average;

	int newValue;

	for (int i = 0; i < image->width * image->height; i++)
		sum += image->data[i].r;

	average = sum / (float)(image->width * image->height);

	for (int i = 0; i < image->width * image->height; i++) {
		newPix = image->data[i];
		newValue = contrast * ((float)newPix.r - average) + average;

		if (newValue > 255)
			newValue = 255;
		if (newValue < 0)
			newValue = 0;

		newPix.r = newValue;
		newPix.g = newValue;
		newPix.b = newValue;

		image->data[i] = newPix;
	}
}

void __Contrast_Enhancement_Method2(struct Bitmap* image) // Gray Histogram
{	
	int gray_histo[256] = {0};
	for (int i = 0; i < image->width * image->height; i++)
		gray_histo[image->data[i].r] += 1;

	int sum_histo[256] = {0};
	for (int i = 0; i < 256; i++) {
		int sum = 0;
		for (int j = 0; j <= i; j++)
			sum += gray_histo[j];

		sum_histo[i] = sum;
	}
	
	struct Pixel newPix = make_pixel();
	int new_gray;

	for (int i = 0; i < image->width * image->height; i++) {
		newPix = image->data[i];
		new_gray = (255*sum_histo[newPix.r]) / (image->width * image->height);
		
		if (new_gray > 255)
			new_gray = 255;
		if (new_gray < 0)
			new_gray = 0;

		newPix.r = new_gray;
		newPix.g = new_gray;
		newPix.b = new_gray;
		image->data[i] = newPix;
	}
}

void __Contrast_Enhancement_Method3(struct Bitmap* image) // Sharpen
{
	/*	
	Sharpen Matrix:
	[-1][-1][-1]
	[-1][ 9][-1]
	[-1][-1][-1]
	*/

	struct Bitmap* new_image = dupli_bitmap(image);	

	for (int x = 0; x < image->width; x++) {
		for (int y = 0; y < image->height; y++) {
            
			int counter = 1;
			int new_intensity = 0;
			
			for (int i = -1; i <= 1; i++) {
            	
				int tmp_x = x + i;
            	
				if (tmp_x >= 0 && tmp_x < image->width) {
					for (int j = -1; j <= 1; j++) {
						
						int tmp_y = y + j;
				    	
						if (tmp_y >= 0 && tmp_y < image->height) {
                			if (i == 0 && j ==0)
								continue;

							counter++;
							new_intensity -= 
								image->data[tmp_x + tmp_y * image->width].r;
			    		}
					}
				}
			}

			new_intensity += counter * image->data[x + y * image->width].r;

			if (new_intensity > 255)
				new_intensity = 255;
			if (new_intensity < 0)
				new_intensity = 0;

			struct Pixel newPix = make_pixel();
			newPix.r = new_intensity;
			newPix.g = new_intensity;
			newPix.b = new_intensity;

			new_image->data[x + y * new_image->width] = newPix;
		}
	}

	for (int i = 0; i < image->width * image->height; i++)
		image->data[i] = new_image->data[i];

	free_bitmap(new_image);
}
