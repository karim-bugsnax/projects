#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "helper.h"

// Swap the contents of two variables.
void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Print an array.
void array_print(int *begin, int *end)
{
    for (int line = 0; begin != end; ++begin)
    {
        if (line > 72)
        {
            printf("|`|\n");
            line = 0;
        }

        line += printf("| %4d ", *begin);
    }

    printf("|\n");
}

// Test if an array is sorted.
// If the array is sorted, return true (1).
// Otherwise, return false (0).
int array_is_sorted(int *begin, int *end)
{
    for (int *p = begin; p < end-1; p++)
    {
        // 1 0 2 3 4 5
        if (*(p+1) < *p)
        {
            return 0;
        }
    }
    return 1;
}

// Fill an array with sorted values.
void array_sorted_fill(int *begin, int *end)
{
    for (int i = 0; begin != end; ++begin, ++i)
        *begin = i;
}

// Fill an array with random values.
void array_random_fill(int *begin, int *end)
{
    for (; begin != end; ++begin)
        *begin = random() % 10000;
}

// Benchmark an algorithm.
void bench_sort(int *begin, int *end, sort_fun sort, const char *msg)
{
    struct timespec t0, t1;

    clock_gettime(CLOCK_MONOTONIC, &t0);
    sort(begin, end);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    printf("%s:    \t%g\n", msg, time_gdiff(t0, t1));
    assert(array_is_sorted(begin, end));
}