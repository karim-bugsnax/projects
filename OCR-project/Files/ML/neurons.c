#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "neurons.h"

#define LEARNING_RATE 0.1

float random_float(){
  float min = -0.5;
  float max = 0.5;
  return  (((double)rand()/((double)RAND_MAX + 1)) * (max - min)) + min;
}

double sigmoid(double x)
{
    return (1.0/(1.0+exp(-x)));
}

double sigmoid_derivative(double x)
{
    return x*(1.0-x);
}

struct Neuron* make_layer(int size) {
  struct Neuron* result = (struct Neuron*)calloc(size, sizeof(struct Neuron));
  //printf("* Layer created of size %i\n", size);
  return result;
}

void load_bitmap_in_layer(struct Bitmap* image, struct Neuron* layer, int size){
  for(int i = 0; i < size; i++){
    struct Pixel p = image->data[i];
    double value = (p.r + p.g + p.b);
    layer[i].value = value / (256*3);
  }
  //printf("* Layer populated from bitmap of size %i\n", size);
}

void initialize_weights(struct Neuron* layer, int size_layer, int size_parent){
  for(int i = 0; i < size_layer; i++) {
    layer[i].weights = malloc(sizeof(float) * size_parent);
    for(int p = 0; p < size_parent; p++) {
      layer[i].weights[p] = random_float();
    }
  }
  //printf("* Initialized layer weights, %i neurons, %i parents\n", size_layer, size_parent);
}

void plug_inputs_to_neuron(struct Neuron* neuron, struct Neuron* parent_layer, int size_parent){
  neuron->inputs = (struct Neuron**)calloc(size_parent, sizeof(struct Neuron*));
  for(int i = 0; i < size_parent; i++) {
    neuron->inputs[i] = &parent_layer[i];
  }
  neuron->inputs_size = size_parent;
}

void plug_inputs(struct Neuron* layer, struct Neuron* parent_layer, int size_layer, int size_parent){
  for(int i = 0; i < size_layer; i++) {
    plug_inputs_to_neuron(&layer[i], parent_layer, size_parent);
  }
  //printf("* Plugged a layer of size %i  as input to one of size %i\n", size_parent, size_layer);
}

void update_value_of_neuron(struct Neuron* neuron){
  float sum = 0;
  for(int i = 0; i < neuron->inputs_size; i++) {
    sum += neuron->weights[i] * neuron->inputs[i]->value;
  }
  neuron->value = sigmoid(sum);
}

void update_value(struct Neuron* layer, int size_layer){
  for(int i = 0; i < size_layer; i++) {
    update_value_of_neuron(&layer[i]);
  }
  //printf("* * Updated value of %i neurons (example: %f)\n", size_layer, layer[0].value);
}

void teach_from_propagated_error(struct Neuron* neuron, float error){
  float output_change_direction = error * sigmoid_derivative(neuron->value);
  for(int i = 0; i < neuron->inputs_size; i++) {
    float diff = neuron->inputs[i]->value * LEARNING_RATE * output_change_direction;
    neuron->weights[i] += diff;
  }
  neuron->error = output_change_direction;
}

float teach_neuron_from_example(struct Neuron* neuron, float expected_value){
  // update_value_of_neuron will be called by the caller function so we can expect the neuron->value to exist.

  float error = expected_value - neuron->value;
  teach_from_propagated_error(neuron, error);
  return error;
}

float teach_from_example(struct Neuron* layer, struct Neuron* expected_values, int size_layer){
  float error = 0;
  for(int i = 0; i < size_layer; i++) {
    error += teach_neuron_from_example(&layer[i], expected_values[i].value);
  }
  //printf("* * Taught %i neurons\n", size_layer);
  return error;
}

void free_memory_of_neuron(struct Neuron* neuron){
  free(neuron->weights);
  free(neuron->inputs);
}
