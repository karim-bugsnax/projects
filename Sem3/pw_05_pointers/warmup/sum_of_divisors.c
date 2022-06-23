# include <stdio.h>

unsigned long sum_of_divisors(unsigned long n, size_t *count)
{
    unsigned long res =0;
    if (n ==1 )
    {
        return res;
    }
    size_t *c = count;
    *c =1;
    for (unsigned long i=2; i*i <= (n); i++) 
    {   //countgiving 3 instead of 5
        if (n%i==0) 
        { 
            if (i==(n/i)){ 
                res += i;
                *c += 1;} 
            else{
                res += (i + n/i);
                *c += 2;}
        } 
    }
    return res +1;
}


int main()
{
    unsigned long x;
    unsigned long sum;
    size_t count;

    x = 28;
    sum = sum_of_divisors(x, &count);
    printf("x = %lu\n", x);
    printf("sum   = %lu\n", sum);
    printf("count = %zu\n\n", count);

    x = 100;
    sum = sum_of_divisors(x, &count);
    printf("x = %lu\n", x);
    printf("sum   = %lu\n", sum);
    printf("count = %zu\n", count);
}