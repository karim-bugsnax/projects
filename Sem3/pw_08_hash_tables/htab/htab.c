#include <err.h>
#include <string.h>

#include "htab.h"

uint32_t hash(char *key)
{
    size_t i = 0;
    size_t length = strlen(key);
    uint32_t haash = 0;
    while (i != length) {
        haash += key[i++];
        haash += haash << 10;
        haash ^= haash >> 6;
    }
    haash += haash << 3;
    haash ^= haash >> 11;
    haash += haash << 15;
    return haash;
}

struct htab *htab_new()
{
    struct htab *tab = calloc(1,sizeof(struct htab));
    if (tab == NULL)
    {
        errx(1,"Not enough memory!");
    }
    tab->capacity=4;
    tab->size=0;
    tab->data = calloc(tab->capacity, sizeof(struct pair));
    if (tab->data == NULL)
    {
        errx(1,"Not enough memory!");
    }
    for (size_t i = 0; i < tab->capacity; i++)
    {
        (tab->data+i)->next = NULL;
    }
    
    return tab ;
}

void htab_clear(struct htab *ht)
{
    for (size_t i = 0; i < ht->capacity; i++)
    {
        struct pair *copy1 = ht->data+i;
        while(copy1->next)
        {
            struct pair *tmp1 = copy1->next;
            free(copy1->next);
            copy1 = tmp1;
        }
        struct pair *p = malloc(sizeof(struct pair));

        p->hkey = 0;
        p->key = ""; 
        p->value = ""; 
        p->next = NULL;  
        *(ht->data+i) = *p;
    }
    
    ht->size=0;
}

void htab_free(struct htab *ht)
{
    htab_clear(ht);
    free(ht->data);
    free(ht);
}

struct pair *htab_get(struct htab *ht, char *key)
{
    if (ht->size >0)
    {
        uint32_t in  = hash(key);
        int index = in %ht->capacity;
        ht->data = ht->data + index;
        ht->data = ht->data->next;
        while(ht->data)
        {
            int truth = strcmp(ht->data->key, key);
            if(truth == 0)
            {
                ht->data->next = NULL;
                return ht->data;
            }
            ht->data = ht->data->next;
        }
    }  
    return NULL;
}

int htab_insert(struct htab *ht, char *key, void *value)
{
    if (htab_get(ht, key) != NULL) 
        return 0;


    uint32_t hkey = hash(key);  
    char ind = hkey % ht->capacity;   

    struct pair *sent = ht->data+ind; 
    struct pair *after = sent->next; 

    struct pair *p = malloc(sizeof(struct pair));

	if (p == NULL)
        errx(1,"Not enough memory!");

    p->hkey = 0;
    p->key = ""; 
	p->value = ""; 
	p->next = NULL; 

    struct pair *new = p; 
    new->hkey = hkey; 
    new->value = value; 
    new->next = after; 
    new->key = key; 

    if (sent->next == NULL)
        ht->size+=1;

    sent->next = new; 

    if (100*(ht->size / ht->capacity) > 75)   
    { 
        ht->capacity = (ht->capacity)*2; 
        ht->data = realloc(ht->data, ht->capacity*sizeof(struct pair)); 
    }  

    return 1; 
}

void htab_remove(struct htab *ht, char *key)
{
    uint32_t hach = hash(key);
    int index = hach% ht->capacity ;
    ht->data = ht->data+index;
    ht->data = ht->data->next; 
    while (ht->data)
    {  
        int truth = strcmp(ht->data->key , key);
        if (truth == 0)
        {
            ht->size -=1;
            struct pair *boi = ht->data;
            ht->data = ht->data->next;
            free(boi);   
            break; 
        }  
        ht->data = ht->data->next; 
    }
    
}
