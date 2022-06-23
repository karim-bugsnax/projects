#include <err.h>
#include "vector.h"

struct vector *vector_new()
{
    //need to realloc
    struct vector *v = malloc(sizeof(struct vector));
    if (v == NULL)
    {
        errx(1,"Not enough memory!");
    }
    v->capacity=1;
    v->size=0;
    v->data = malloc(v->capacity*sizeof(int));
    if (v->data == NULL)
    {
        errx(1,"Not enough memory!");
    }
    return v ;

    
}

void vector_free(struct vector *v)
{
    v->capacity=1;
    v->size=0;
    // pointer = [] karim retard
    free(v->data);
    free(v);

}

void double_capacity(struct vector *v)
{
    size_t doub = 2*v->capacity;
    v->capacity = doub;
    v->data = realloc(v->data,v->capacity*sizeof(int));
    if (v->data == NULL)
    {
        errx(1,"Not enough memory!");
    }
}

void vector_push(struct vector *v, int x)
{
    if (v->size == v->capacity)
    {
        double_capacity(v);
    }
    v->data[v->size] = x;
    v->size+=1; // -g compilation
    
}

int vector_pop(struct vector *v, int *x)
{
    if (v->size !=0)
    {
        *x = v->data[v->size-1];
        v->data[v->size-1]=0;
        v->size-=1;
        
        return 1;
    }
    return 0;
    
}

int vector_get(struct vector *v, size_t i, int *x)
{
    if (i< v->size) //v.capacity mayhaps?
    {
        *x = v->data[i];
        return 1;
    }
    return 0;
    
}

void vector_insert(struct vector *v, size_t i, int x)
{
    if (i<= v->size)
    {
        v->size+=1;
        if ( v->size > v->capacity)
        {
            double_capacity(v);
        }
        for (size_t z = v->size-1; z >= i; z--)
        {
            v->data[z] = v->data[z-1];
        }
        v->data[i]=x;
        
    }
    
}

int vector_remove(struct vector *v, size_t i, int *x)
{
    if (i< v->size)
    {
        *x= v->data[i];
        size_t j = i;
        for(; j < v->size-1; j++)
        {
            v->data[j] =v->data[j+1];
        }
        v->size-=1;
        return 1;
    }
    return 0;
}