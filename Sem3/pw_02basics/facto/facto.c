
unsigned long facto(unsigned long n){
    unsigned long m =1 ;
    for (unsigned long i = 1; i < n; i++)
    {
        m = m * (i+1);
    }
    return m;
}
