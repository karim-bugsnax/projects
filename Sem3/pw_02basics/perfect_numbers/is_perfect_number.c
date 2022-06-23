#include "is_perfect_number.h"
#include "divisor_sum.h"


int is_perfect_number(unsigned long n)
{
    if (n == divisor_sum(n))
    {
        return n;
    }
    return 0;
}