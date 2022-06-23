#include "stdlib.h"
#include <stdio.h>
#include "err.h"

//DECLARATIONS
int alpha_only(char phrase[], char word[]);
int lenstring(char string[]);


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        errx(1,"Usage: str1 str2");
        return 1;
    }
    //now to get the actual string and use alpha_only
    alpha_only(argv[1],argv[2]);
    return 0;
}



int lenstring(char string[]){
    int i;
    for (i = 0; string[i] != '\0'; ++i);
    return i;
}

int alpha_only(char phrase[], char word[])
{
    int x=0;
    int index=0;
    int boolean = 0;

    int length = lenstring(word);
    int size = lenstring(phrase) ;
    for (int i = 0; i < size; i++)
    {
        if (boolean == 0)
        {
            index+=1;
        }
        
        //check for alphabet
        //error is not? ask asm
        //if !((phrase[i] >= 'A' && phrase[i] <= 'Z')||
        //(phrase[i] >= 'a' && phrase[i] <= 'z'))
        /*if (!((signed)phrase[i]>=1 && (signed)phrase[i]<=127))
        {
            return 1;
        }*/
        
        if (phrase[i] == word[x])
        {
            for (int j = i; j < size; j++)
            {
                if (phrase[j] == word[x])
                {
                    x+=1;
                }
                else
                {
                    printf("%s","Not Found!\n");
                    return 1;
                }
                if (x == length-1)
                {
                    printf("%s\n",phrase);
                    printf("%*s",index-1,"");
                    printf("%s","^\n");
                    return 0;
                }

            }
            boolean= 1;
        }
         
    }
    printf("%s","Not Found!\n");
    return 1;
}

