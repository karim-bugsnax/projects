#include "allocator.h"
#include "utils.h"
#include <sys/mman.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include "allocator.h"

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
        if (blka->meta->next->size)
            blka->meta->next->size = 0;
        blka_free(blka->meta->next);
        blka->meta->next = tmp;
    }
    blka->meta->size = 0;
    blka_free(blka->meta);
    free(blka);
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    //do we mmap all the variables of a struct?
    //how to map something if map return void *
    //explicit or implicit cast mayhaps?
    long page_len = sysconf(_SC_PAGESIZE);

    struct blk_meta *meta1 = mmap(NULL, page_len, PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE, -1 ,0);
    if (meta1 == MAP_FAILED)
        return NULL;

    //or is it also the allocator?
    meta1->size = page_len - size - sizeof(struct blk_meta);

    struct blk_meta *tmp = blka->meta;
    meta1->next = tmp;

    blka->meta = meta1;
    return meta1;
}

void blka_free(struct blk_meta *blk)
{
    //how to unmap an array??
    munmap(blk->data, blk->size);
    blk->size = 0;
    munmap(blk, sizeof(struct blk_meta));
}

void blka_pop(struct blk_allocator *blka)
{
    struct blk_meta *tmp = blka->meta->next;
    blka_free(blka->meta);
    blka->meta = tmp;
}
