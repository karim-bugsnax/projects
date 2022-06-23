#include <stdlib.h>
#include "list.h"


void list_init(struct list *list)
{
    list->data=0;
    list->next=NULL;
}

int list_is_empty(struct list *list)
{
    if (list->next == NULL)
    {
        return 1;
    }
    return 0;
}

size_t list_len(struct list *list)
{
    if (list_is_empty(list))
    {
        return 0;
    }
    size_t count =0;
    while (list->next != NULL)
    {
        list =list->next;
        count+=1;
    }
    return count;
}

void list_push_front(struct list *list, struct list *elm)
{
    elm->next = list->next;
    list->next = elm;
}

struct list *list_pop_front(struct list *list)
{
    if (list_is_empty(list))
    {
        return NULL;
    }
    struct list *copy = list->next;
    list->next = list->next->next;
    return copy;
}

struct list *list_find(struct list *list, int value)
{
    list = list->next;
    while (list)
    {
        if(list->data ==value){
            return list;
        }
        list = list->next;
    }
    return NULL;
}

struct list *list_lower_bound(struct list *list, int value)
{
    //beware of sentinel.
    //you might be comparing the sentinel value as well

    //struct list *copy = list;

    while (list != NULL)
    {
        
        if(list->next->data > value){
            return list;
        }
        //copy = list;
        list = list->next;
    }
    return list;
}

int list_is_sorted(struct list *list)
{
    //check list empty??
    if (list->next == NULL)
    {
        return 1;
    }
    list= list->next;
    while(list->next != NULL)
    {
        if(list->next->data < list->data){

            return 0;
        }
        list = list->next;
    }
    return 1;
}

void list_insert(struct list *list, struct list *elm)
{
    struct list *copy = list;

    while (list->next != NULL)
    {
        
        if(list->next->data > elm->data){
            break;
        }
        copy = list;
        list = list->next;
    }
    elm->next = copy->next;
    list->next = elm;
    
}



void list_rev(struct list *list)
{
    if (list_len(list) >1)
    {
        struct list *prev = list->next->next;
        struct list *lis = list->next->next;
        
        struct list *new = list->next;

        new->next = NULL;

        while(prev != NULL)
        {
            lis = lis->next;

            prev->next = new;

            new = prev;
            prev = lis;
        }
        list->next = new;
    }
    
}




void list_half_split(struct list *list, struct list *second)
{
    size_t len = list_len(list);
    if (len > 2)
    {
        struct list* current = list;
        int Count = (len) / 2;
        if (len %2 !=0)
        {
            Count+=1;
        } 
        for (int i = 0; i < Count; i++)
            current = current->next;

        // Now cut at current
        second->next = current->next;
        list = current;
        current->next =NULL;
    }

    
}




