#include "stdio.h"
#include "stdlib.h"


//DECLARE
int lenstring(char string[]);
int check_sep(char s, char sepa[]);
void mix(char s[]);


void mix(char s[]){
    printf("%s\n", s);
    int start=0;
    int n = lenstring(s);
    char separator[] = " ,;:!?./%*$=+)@_-('&1234567890\"\r\n";
    for (int i = 0; i < n; i++)
    {
        
        char ss = s[i];
        int check = check_sep(ss,separator);
        if (check == 0)
        {   
            start +=1;
            if (start >3 && start%2 ==0)
            {
                char tmp= s[i-1];
                s[i-1] = s[i-2];
                s[i-2] = tmp;
            }
        }
        else
        {
            start =0;
        }
        
    }
    printf("%s\n",s);
    
}


//to check if the string has separators
int check_sep(char s, char sepa[])
{
    for (int j = 0; j < 34; j++)
    {
        if (s == sepa[j])
        {
            return 1;
        }
    }
    return 0;
}


//find length of text
int lenstring(char string[])
{
    int i;
    for (i = 0; string[i] != '\0'; ++i);
    return i;
}