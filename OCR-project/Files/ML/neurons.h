#ifndef NEURONS_H
#define NEURONS_H

#include "../base_structures.h"

struct Neuron {
	float* weights;
  float value;
  float error;

  struct Neuron** inputs;
  int inputs_size;
};

struct Neuron* make_layer(int size);

void load_bitmap_in_layer(struct Bitmap* image, struct Neuron* layer, int size);

void initialize_weights(struct Neuron* layer, int size_layer, int size_parent);

void plug_inputs(struct Neuron* layer, struct Neuron* parent_layer, int size_layer, int size_parent);

void update_value(struct Neuron* layer, int size_layer);

float teach_from_example(struct Neuron* layer, struct Neuron* expected_values, int size_layer);

void teach_from_propagated_error(struct Neuron* neuron, float error);

void free_memory_of_neuron(struct Neuron* neuron);

#endif
