#include <stdio.h>
#include <stdlib.h>

#include "../base_structures.h"
#include "neurons.h"
#include "network.h"

#define NETWORK_FILE  "ml_weights.csv"
#define IMAGE_SIZE 28*28
#define HIDDEN_LAYER_SIZE 256

struct Network make_network(int size_input, int size_hidden, int size_output) {
  struct Network result;
  result.input_layer = make_layer(size_input);
  result.size_input = size_input;
  result.hidden_layer = make_layer(size_hidden);
  result.size_hidden = size_hidden;
  result.output_layer = make_layer(size_output);
  result.size_output = size_output;
  //printf("* Creating a network of %i/%i/%i\n", size_input, size_hidden, size_output);
  return result;
}

struct Network make_default_network(){
  struct Network network = make_network(IMAGE_SIZE, HIDDEN_LAYER_SIZE, 256);
  initialize_weights(network.hidden_layer, network.size_hidden, network.size_input);
  initialize_weights(network.output_layer, network.size_output, network.size_hidden);
  plug_inputs(network.hidden_layer, network.input_layer, network.size_hidden, network.size_input);
  plug_inputs(network.output_layer, network.hidden_layer, network.size_output, network.size_hidden);
  //printf("* Initialization of new network complete\n");
  return network;
}

struct Network load_network(struct Bitmap* image){
  struct Network network = load_from_disk();
  load_bitmap_in_layer(image, network.input_layer, IMAGE_SIZE);
  return network;
}

void print_layer(FILE *fptr, struct Neuron* layer, int size_layer) {
   for(int i = 0 ; i < size_layer; i++){
      for(int p = 0; p < layer[i].inputs_size; p++) {
        fprintf(fptr,"%f ", layer[i].weights[p]);
      }
      fprintf(fptr,"\n");
   }
}

void save_to_disk(struct Network* network) {
  FILE *fptr;
  fptr = fopen(NETWORK_FILE, "w");

   if(fptr == NULL)
   {
      //printf("ERROR opening file %s ", NETWORK_FILE);
      exit(1);
   }

   fprintf(fptr,"%i %i %i\n", network->size_input, network->size_hidden, network->size_output);
   print_layer(fptr, network->hidden_layer, network->size_hidden);
   print_layer(fptr, network->output_layer, network->size_output);
   fclose(fptr);

   //printf("* Saved ML model to disk\n");
}

void load_layer_from_disk(FILE *fptr, struct Neuron* layer, int size_layer, int size_inputs){
  if (layer->inputs_size != size_inputs){
    //printf(" * ! ERROR3 reading file %s, found %i inputs to layer instead of %i ", NETWORK_FILE, size_inputs, layer->inputs_size);
    return;
  }
  float read;
  int r;
  for(int i = 0; i < size_layer; i++) {
    for(int p = 0; p < size_inputs; p++) {
      r = fscanf(fptr,"%f ", &read);
      layer[i].weights[p] = read;
      if (r==EOF){
        //printf(" * ! ERROR4 reading file: %s ", NETWORK_FILE);
      }
    }
    r = fscanf(fptr,"\n");
    if (r==EOF){
      //printf(" * ! ERROR5 reading file: %s ", NETWORK_FILE);
    }
  }

}

struct Network load_from_disk() {
  struct Network network = make_default_network();

  FILE *fptr;
  fptr = fopen(NETWORK_FILE, "r");
  if(fptr == NULL)
  {
    //printf(" * ! No file found for ML model: %s ", NETWORK_FILE);
    return network;
  }

  int size_input, size_hidden, size_output;
  int r = fscanf(fptr,"%i %i %i\n", &size_input, &size_hidden, &size_output);
  if (r==EOF){
    //printf(" * ! ERROR1 reading file, using default: %s ", NETWORK_FILE);
    return network;
  }
  // Here, we could ignore the size of the network saved to disk, since we always make the same size of network. Or we could make a network of the size we just read. Instead, we'll do some checks.
  if (network.size_input != size_input || network.size_hidden != size_hidden || network.size_output != size_output){
    //printf(" * ! ERROR2 reading file, using default: %s ", NETWORK_FILE);
    return network;
  }

  load_layer_from_disk(fptr, network.hidden_layer, network.size_hidden, network.size_input);
  load_layer_from_disk(fptr, network.output_layer, network.size_output, network.size_hidden);
  //printf("* Loading a network of %i/%i/%i from %s\n", network.size_input, network.size_hidden, network.size_output, NETWORK_FILE);
  fclose(fptr);
  return network;
}

void run_network(struct Network* network){
  update_value(network->hidden_layer, network->size_hidden);
  update_value(network->output_layer, network->size_output);
}

void set_teaching_output(struct Network* teaching_network, char* truth){
  for (int o = 0 ; o < 256; o ++){
    if (o == (*truth)){
      teaching_network->output_layer[o].value = 1;
    } else {
      teaching_network->output_layer[o].value = 0;
    }
  }
}

void propagate_errors_to_upper_layer(struct Network* network){
    for (int h = 0 ; h < HIDDEN_LAYER_SIZE; h ++){
      float error = 0;
      for (int o = 0 ; o < 256; o ++){
        error += network->output_layer[o].error * network->output_layer[o].weights[h];
      }
      teach_from_propagated_error(&(network->hidden_layer[h]), error);
    }

}

void train(struct Network* network, struct Bitmap* image, char* truth, char print_error){
  load_bitmap_in_layer(image, network->input_layer, IMAGE_SIZE);
  run_network(network);

  struct Network teaching_network = make_network(0, HIDDEN_LAYER_SIZE, 256);
  set_teaching_output(&teaching_network, truth);
  float error_output = teach_from_example(network->output_layer, teaching_network.output_layer, network->size_output);

  propagate_errors_to_upper_layer(network);
  free_memory_of_network(&teaching_network);

  if(print_error){
    printf("* Trained network on character %c (%f) \n", *truth, error_output);
  }
}

char read_network_output(struct Network* network){
  float value = -1.0;
  char index = 0;
  for(int i = 0; i < network->size_output; i++) {
    if (network->output_layer[i].value >= value) {
      value = network->output_layer[i].value;
      index = (char)i;
    }
  }

  return index;
}

char guess(struct Network* network, struct Bitmap* image){
  load_bitmap_in_layer(image, network->input_layer, IMAGE_SIZE);
  run_network(network);
  char c = read_network_output(network);
  //printf("* > I think this image is a %c (%i)\n", c, c);
  return c;
}

void free_memory_of_network(struct Network* network){
  for(int i = 0; i < network->size_output; i++) {
    free_memory_of_neuron(&(network->output_layer[i]));
  }
  free(network->output_layer);
  for(int i = 0; i < network->size_hidden; i++) {
    free_memory_of_neuron(&(network->hidden_layer[i]));
  }
  free(network->hidden_layer);
  for(int i = 0; i < network->size_input; i++) {
    free_memory_of_neuron(&(network->input_layer[i]));
  }
  free(network->input_layer);
}
