#include "beware_overflow.h"

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t pt = 0;
    if (__builtin_mul_overflow(nmemb, size, &pt))
    {
        return NULL;
    }
    char *res = ptr;
    return (res + pt);
}
