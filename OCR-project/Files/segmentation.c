#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "base_structures.h"
#include "letter_extraction.h"
#include "ML/network.h"
#include "ML/neurons.h"

// to use mkdir and rmdir
//#include <sys/stat.h>
//#include <unistd.h>

struct Pixel get_pix(struct Bitmap* image, int x, int y){
  return image->data[x + y * image->width];
}

void set_pixel(struct Bitmap* image, int x, int y, struct Pixel p){
  image->data[x + y * image->width] = p;
}

int* make_array_like_its_1985(unsigned long size){
  int* r = malloc (sizeof (int) * size);
  for (unsigned long y = 0; y < size; y++) {
    r[y] = 0;
  }
  return r;
}

void print_array(int array[], int lets_pretend_its_noraml_to_pass_fcking_size){
  for (int x = 0; x < lets_pretend_its_noraml_to_pass_fcking_size; x++) {
    printf("%i ", array[x]);
  }
  printf("\n");
}

int* vertical_scan(struct Bitmap* image){
  int* histogram =  make_array_like_its_1985 (image->height);

  for (int x = 0; x < image->width; x++) {
    for (int y = 0; y < image->height; y++) {
      if (get_pix(image, x, y).r == 0) {
        histogram[y] += 1;
      }
    }
  }

  return histogram;
}

int* horizontal_scan(struct Bitmap* image, int from_y, int to_y){
  int* histogram =  make_array_like_its_1985 (image->width);

  for (int x = 0; x < image->width; x++){
    for (int y = from_y; y <= to_y; y++){
      if (get_pix(image, x, y).r == 0)
        histogram[x] += 1;
    }
  }

  return histogram;
}

void add_hori_lines(struct Bitmap* image, int* ys,
		int pass_size_of_array_like_its_1975){
  for (int y = 0; y < pass_size_of_array_like_its_1975; y ++){
    if (ys[y] == 0) {
      continue;
    }
    for (int x = 0; x < image->width; x++){
      struct Pixel p = { .r = 0, .g = 0, .b = 255 };
      set_pixel(image, x, y, p);
    }
  }
}

void add_verti_lines(struct Bitmap* image, int* xs,
		int pass_size_of_array_like_its_1975, int from_y, int to_y){
  for (int x = 0; x < pass_size_of_array_like_its_1975; x ++){
    if (xs[x] == 0) {
      continue;
    }
    for (int y = from_y; y < to_y; y++){
      struct Pixel p = { .r = 255, .g = 0, .b = 0 };
      set_pixel(image, x, y, p);
    }
  }
}

void add_verti_blue_lines(struct Bitmap* image, int* xs,
		int pass_size_of_array_like_its_1975, int from_y, int to_y){
  for (int x = 0; x < pass_size_of_array_like_its_1975; x ++){
    if (xs[x] == 0) {
      continue;
    }
    for (int y = from_y; y < to_y; y++){
      struct Pixel p = { .r = 0, .g = 0, .b = 255 };
      set_pixel(image, x, y, p);
    }
  }
}

int* find_all_spikes(int* list, int pass_size_of_array_like_its_1975){
	// every occurance of black pixels will be bounded on its edges
	// a 1 represents a mark; 0 is to be ignored
	int* spikogram = make_array_like_its_1985 (pass_size_of_array_like_its_1975);

	char on_letter = 0; // 0 = not on letter; used like a boolean
	int previous_mark = -1;

	for (int i = 0; i < pass_size_of_array_like_its_1975; i++){
		spikogram[i] = 0;
		if (list[i] > 0) { // we are on black pixels
      if (on_letter == 0) {
				/*if (previous_mark == -1)
					spikogram[i-1] = 1;
				else
					spikogram[(i + previous_mark) / 2] = 1;
				on_letter = 1;
				*/
				spikogram[i-1] = 1;
				on_letter = 1;
			}
    }
		else if (on_letter == 1) { // we are on white, just left black
			on_letter = 0;
			previous_mark = i;
		}
	}

	spikogram[previous_mark] = 1;

	return spikogram;
}

int* find_good_spikes(int* oversegmentation, int pass_size_of_array_like_its_1975){
	// we filter out potentially flawed marks, and smoothen the existing ones

	int* segmentation = make_array_like_its_1985(pass_size_of_array_like_its_1975);

	// computing average width
	int counter_marks = -1;
	int total_width_marks = 0;
	int previous_mark = -1;
	int zone_start = -1;
	for (int i = 0; i < pass_size_of_array_like_its_1975; i++){
		if (oversegmentation[i] == 1) {
			counter_marks++;
			if (previous_mark != -1) { // everything except the first one
				int width = (i - previous_mark + 1);
				total_width_marks += width;
			}
			else
				zone_start = i;
			previous_mark = i;
		}

		segmentation[i] = 0;
	}

	if (counter_marks <= 0)
		return segmentation;

	double average_width = total_width_marks / counter_marks;
	double bound_bot = average_width * 0.65; // case too many ;; 0.65
	double bound_up = average_width * 1.2; // case too little ;; 1.4

	// We'll take care of cases where there are either too little or too many
	// marks; by using the average width
	previous_mark = zone_start;
	segmentation[zone_start] = 1;

	int previous_width = -1;

	for (int i = zone_start + 1; i < pass_size_of_array_like_its_1975; i++){
    if (oversegmentation[i] == 1) {
			int width = i - previous_mark + 1;

			if (width < bound_bot) { // case too many / too close
				// fuse it with previous marker
				/*if (previous_mark != zone_start) {
					segmentation[previous_mark] = 0;
					//segmentation[(previous_mark + i) / 2] = 1;
					//previous_mark = (previous_mark + i) / 2;
					segmentation[i] = 1;
					previous_mark = i;
				}
				segmentation[i] = 1;
				previous_mark = i;
				previous_width = previous_width; */
				if (previous_mark != zone_start) {
					if (previous_width < bound_bot) {
						segmentation[previous_mark] = 0;
						segmentation[i] = 1;
						previous_mark = i;
					}
				}
				else {
					segmentation[i] = 1;
					previous_mark = i;
				}
			}
			if (bound_up < width) { // case too little / too far
				double tmp_average_width = (total_width_marks - width) / (counter_marks - 1);
				double full = width / tmp_average_width;
				int core = round(full);
				int add_width = tmp_average_width * full / core;

				for (int j = 1; j < core; j++)
					segmentation[previous_mark + j * add_width] = 1;

				segmentation[i] = 1;
				previous_mark = i;
			}
			else { // falls within expectation; keep it
				segmentation[i] = 1;
				previous_mark = i;
			}

			previous_width = width;
		}
  }

	segmentation[previous_mark] = 1;

	return segmentation;
}

void write_letter(char c) {
	FILE *fp;
	fp = fopen("result.txt","a");
	if (fp != NULL) {
		fprintf(fp, "%c", c);
		fclose(fp);
	}
}

void do_something_with_this_its_a_letter(struct Network* network, struct Bitmap* image,
  int bottom_y, int top_y, int left_x, int right_x, int *p_nb_letter){
	struct Bitmap* letter =
	  letter_extract_on_28_28(image, left_x, right_x, top_y, bottom_y);

	*p_nb_letter += 1;
	//save_letter(letter, *p_nb_letter); // we dont need to save them anymore

	char g = guess(network, letter);
	//printf("%c\n", g);

	if (33 <= g && g <= 126)
		write_letter(g);

	free_bitmap(letter);
}

void segment_line(struct Bitmap* image, int from_y, int to_y, int* p_nb_letter){
  int* h = horizontal_scan(image, from_y, to_y);
  int* oversegmentation = find_all_spikes(h, image->width);
  int* h_lines = find_good_spikes(oversegmentation, image->width);

  //add_verti_lines(image, oversegmentation, image->width, from_y, to_y);
  add_verti_blue_lines(image, h_lines, image->width, from_y, to_y);

  int left = -1;
  int right = -1;
  struct Network network = load_from_disk();


  for (int x = 0; x < image->width; x++){
    if (h_lines[x] == 1){
			if (left == -1)
				left = x;
			else {
				right = x - 1;
				do_something_with_this_its_a_letter(&network, image, from_y, to_y, left, right, p_nb_letter);
				left = x;
			}
    }
  }

  free_memory_of_network(&network);
	free(h);
	free(oversegmentation);
	free(h_lines);
}

void segmentation(struct Bitmap* image){
//here we call all the above functions in the same order
//this is the main function
  int* v = vertical_scan(image);
  int* oversegmentation = find_all_spikes(v, image->height);
  int* v_lines = find_good_spikes(oversegmentation, image->height);

	add_hori_lines(image, v_lines, image->height);

	int bottom = -1;
  int top = -1;

	int nb_letter = -1; //-1
	int *p_nb_letter = &nb_letter;

	//char *foldername = "letters";
	//mkdir(foldername, 0777);
	// files from a previous run have to be deleted somehow?

	// we delete result.txt in case it existed before
	remove("result.txt");

	for (int y = 0; y < image->height; y++){
    if (v_lines[y] == 1){
      if (bottom == -1)
				bottom = y + 1;
			else {
				top = y - 1;
      	segment_line(image, bottom, top, p_nb_letter);
      	bottom = y + 1;
    		write_letter('\n');
			}
		}
  }

	free(v);
	free(oversegmentation);
	free(v_lines);
}
