unsigned char digit_count(unsigned long n){
    unsigned long i =0;
    while (n>0)
    {
        n= n/10;
        i+=1;
    }
    return i;
    
}