unsigned long isqrt(unsigned long n){
    unsigned long r = n;
    while (r*r > n)
    {
        r = r + n/r;
        r = r/2;
    }
    return r;
}

unsigned long divisor_sum(unsigned long n){
    unsigned long result = 0;
    if (n ==1 )
    {
        return result;
    }
    for (unsigned long i=2; i<=isqrt(n); i++) 
    {   
        if (n%i==0) 
        { 
            if (i==(n/i)) 
                result += i; 
            else
                result += (i + n/i); 
        } 
    } 
    return (result + 1); 
}