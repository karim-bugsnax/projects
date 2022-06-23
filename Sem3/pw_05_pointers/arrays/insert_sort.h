# ifndef _INSERT_SORT_H_
# define _INSERT_SORT_H_

# include <stdlib.h>
# include <string.h>

# include "helper.h"

int binary(int *begin, int *end, int x);
void array_insert(int *begin, int *end, int x);
void array_insert_bin(int *begin, int *end, int x);
void array_insert_sort(int *begin, int *end);
void array_insert_sort_bin(int *begin, int *end);

# endif