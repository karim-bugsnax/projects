
// Insertion using the plain algorithm.
void array_insert(int *begin, int *end, int x){
    int i = end - begin; 
    int *p =end;
    while (i >0 && x < *(p-1))
    {
        *p = *(p-1);
        i -=1;
        p-=1; 
    }
    *p = x;
}

int* binary(int *begin, int *end, int x){
    int *l = begin;
    int *r = end;
    while (l <= r) { 
        int *m = l + (r - l) / 2; 
  
        if (*m == x) 
            return m; 
  
        if (*m < x) 
            l = m + 1; 
  
        else
            r = m - 1; 
    } 
    return l; 
}

// Insertion using the binary-search algorithm.
void array_insert_bin(int *begin, int *end, int x){

    int *i = binary( begin,  end,  x);
    int *p = end;
    while (p>i)
    {
        *p = *(p-1);
        p--;
    }
    *p = x;
    
}

// Insertion sort using plain method.
void array_insert_sort(int *begin, int *end){
    for (int *p = begin; p < end; p++)
    {
        array_insert( begin, p, *p);
    }
    
}

// Insertion sort using binary search.
void array_insert_sort_bin(int *begin, int *end){
    for (int *p=begin; p < end; p++)
    {
        array_insert_bin( begin, p, *p);
    }
}