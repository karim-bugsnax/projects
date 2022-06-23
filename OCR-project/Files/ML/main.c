//include of standard libraries
#include <stdio.h>
#include <stdlib.h>


//include of our files
#include "../base_structures.h"
#include "../main_lib.h"
#include "network.h"

void concatenate_strings_like_its_1966_lets_all_pretend_its_normal_and_not_insulting(char* thehorror, char c1, char c2, char c3, char t){
  thehorror[16] = c1;
  thehorror[17] = c2;
  thehorror[18] = c3;
  thehorror[20] = t;
}

void bulk_train(struct Network* network, int iteration){
  char thehorror_TRUMP[26] = "training sample/CCC/T.bmp\0";
  for(char t = 48; t < 58; t++){ // [0,1,'2','3','4','5','6',7,8,9]
    for(char c = 32; c < 123; c++){
      char str[5];
      sprintf(str, "%03i", c);
      concatenate_strings_like_its_1966_lets_all_pretend_its_normal_and_not_insulting(thehorror_TRUMP, str[0], str[1], str[2], t);

      struct Bitmap* image = ReadBMP(thehorror_TRUMP);
      train(network, image, &c, (iteration%5 == 0 && c == 100 && t == 50));
    }
  }
}

void test(struct Network* network){
  float attempts = 0;
  float errors = 0;

  char thehorror_TRUMP[26] = "testning sample/CCC/T.bmp\0";
  for(char c = 32; c < 123; c++){
    int chareror  = 0;
        for(char t = 48; t < 50; t++){ // ['0','1']
      if(c > 32 && c < 48) continue;
      if(c > 57 && c < 65) continue;
      if(c > 90 && c < 97) continue;
      char str[5];
      sprintf(str, "%03i", c);
      concatenate_strings_like_its_1966_lets_all_pretend_its_normal_and_not_insulting(thehorror_TRUMP, str[0], str[1], str[2], t);

      struct Bitmap* image = ReadBMP(thehorror_TRUMP);
      char g = guess(network, image);
      if (g != c){
        printf("* * error detected on %c, found %c instead.\n", c, g);
        chareror ++;
      }
    }

    if(chareror == 2 ){
      errors ++;
    }

    attempts++;

  }
  printf("* > this network has precision %f\n\n", (1 - errors/attempts));
}


// Main of the ML subprogram
// it takes images and can train the ML model or test it.
// in the project, you'd use network.h to guess the text for each 28*28 bitmap of letter
int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 3)
    return EXIT_FAILURE;

  if(argv[1][0] == '$') {
    struct Network network = load_from_disk();
    for(int i = 0; i< atoi(argv[2]); i++){
      printf("Training epoch %i\n", i);
      if(i%50 ==0){
        //printf("Training epoch %i\n", i);
        save_to_disk(&network);
      }
      bulk_train(&network, i);
    }
    save_to_disk(&network);
    free_memory_of_network(&network);
    return 0;
  }
  if(argv[1][0] == '!') {
    struct Network network = load_from_disk();
    test(&network);
    free_memory_of_network(&network);
     return 0;
  }
  struct Bitmap* image = ReadBMP(argv[1]);
  struct Network network = load_network(image);

  if (argc==3) {
    char* text = argv[2];
    train(&network, image, text, 1);
    save_to_disk(&network);
    free_memory_of_network(&network);
    return 0;
  } else {
    char g = guess(&network, image);
    printf("* > I think this image is a %c (%i)\n\n", g, g);
    free_memory_of_network(&network);
    return 0;
  }
}
