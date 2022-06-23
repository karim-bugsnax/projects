unsigned long fibo(unsigned long n)
{
    unsigned long f0 = 0;
    unsigned long f1 = 1;
    unsigned long f2 = 0;

    for (unsigned long i = 0; i < n-1; i++)
    {
        f0 = f0 +1-1; //it would put unused otherwise
        f2 = f1+f0;
        f0 = f1;
        f1 = f2;
    }
    return f1;
    
}
