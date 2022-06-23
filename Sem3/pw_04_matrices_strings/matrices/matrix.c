#include <stdio.h>

void print_matrix(char s[], double m[], size_t rows, size_t cols)
{
    /* array[i][j] = array[i * cols + j]

    i is the row number of the cell we want to access.
    j is the column number of the cell we want to access.
    cols is the total number of columns.*/
    //remove the (int) later 

    printf("%s =\n",s);
    for (int i = 0; i < (int)rows; i++)
    {
        printf(" %4g",m[i*cols+0]);
        for (int j = 1; j < (int)cols; j++)
        {
            printf("%4g",m[i*cols+j]);
        }
        printf("\n");
    }
    
}

void transpose(double m[], size_t rows, size_t cols, double r[])
{
    //write code to test it later in main.c
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            r[j*rows +i]= m[i*cols+j];
        }
    }

}

void add(double m1[], double m2[], size_t rows, size_t cols, double r[])
{
    //also add test funcs 
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            r[i*cols+j] = m1[i*cols+j] + m2[i*cols+j];
        }
    }
}

void mul(double m1[], double m2[], size_t r1, size_t c1, size_t c2, double r[])
{
    for (size_t i = 0; i < r1; i++) {
        for (size_t j = 0; j < c2; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < c1; k++)
            {
                sum+= m1[i*c1 +k] * m2[k*c2 + j]; 
            }
            r[i*c2+j] = sum;
        }
    }
}