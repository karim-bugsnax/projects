#include <sys/mman.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include "allocator.h"

///gloabla wariable for syscong page size

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *new = calloc(1, sizeof(struct blk_allocator));
    return new;
}

void blka_delete(struct blk_allocator *blka)
{
    while (blka->meta->next)
    {
        struct blk_meta *tmp = blka->meta->next->next;
        blka_free(blka->meta->next);
        blka->meta->next = tmp;
    }
    blka_free(blka->meta);
    free(blka);
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    struct blk_meta *meta1;
    long page_len = sysconf(_SC_PAGESIZE);
    size_t pl = page_len;
    if (size + sizeof(struct blk_meta) < pl)
    {
        meta1 = mmap(NULL, size + sizeof(struct blk_meta),
                PROT_READ | PROT_WRITE, 
                MAP_ANONYMOUS | MAP_PRIVATE, -1 ,0);
    }
    else
    {
        size_t sizer = page_len;
        while (sizer < size + sizeof(struct blk_meta))
        {
            sizer += pl;
        }
        meta1 = mmap(NULL, sizer,
                PROT_READ | PROT_WRITE, 
                MAP_ANONYMOUS | MAP_PRIVATE, -1 ,0);
    }

    if (meta1 == MAP_FAILED)
        return NULL;

    //if we want to allocate like 4098, we need to check if we go over the page_len
   
    meta1->size = page_len - sizeof(struct blk_meta);


    meta1->next = blka->meta;

    blka->meta = meta1;
    return meta1;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, blk->size + sizeof(struct blk_meta));
}

void blka_pop(struct blk_allocator *blka)
{
    if (blka->meta)
    {
        struct blk_meta *tmp = blka->meta->next;
        blka_free(blka->meta);
        blka->meta = tmp;
    }
}
