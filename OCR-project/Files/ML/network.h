#ifndef NETWORK_H
#define NETWORK_H

struct Network {
  struct Neuron* input_layer;
  struct Neuron* hidden_layer;
  struct Neuron* output_layer;
  int size_input;
  int size_hidden;
  int size_output;
};

void train(struct Network* network, struct Bitmap* image, char* truth, char print_error);
char guess(struct Network* network, struct Bitmap* image);

void save_to_disk(struct Network* network);
struct Network load_network(struct Bitmap* image); // This makes a default if the file isnt found.
struct Network make_default_network(); // so this should never be needed.
struct Network load_from_disk(); // this should never be needed either

void free_memory_of_network(struct Network* network);

#endif
