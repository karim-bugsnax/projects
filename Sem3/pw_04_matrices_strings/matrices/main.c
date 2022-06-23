#include <stdio.h>
#include "matrix.h"

#define ROWS1 1
#define COLS1 2
double m1[] =
{
    1, 2
};

#define ROWS2 2
#define COLS2 2
double m2[] =
{
    1, 2,
    3, 4
};

#define ROWS3 3
#define COLS3 2
double m3[] =
{
    1, 2,
    3, 4,
    5, 6
};

#define ROWS4 4
#define COLS4 3
double m4[] =
{
    1, 0, 3,
    1, 2, 0,
    0, 2, 3,
    1, 0, 3
};

#define ROWS5 3
#define COLS5 4
double m5[] =
{
    1, 2, 3, 4,
    5, 6, 7, 8,
    9, 10, 11, 12,
};

// Results for the transpositions.
double m1_tr[ROWS1 * COLS1];
double m2_tr[ROWS2 * COLS2];
double m3_tr[ROWS3 * COLS3];
double m4_tr[ROWS4 * COLS4];
double m5_tr[ROWS5 * COLS5];

// Results for the additions.
double m3_plus_m3[ROWS3 * COLS3];
double m4_plus_m5tr[ROWS4 * COLS4];
double m5_plus_m4tr[ROWS5 * COLS5];

// Results for the multiplications.
double m2_times_m2[ROWS2 * COLS2];
double m4_times_m5[ROWS4 * COLS5];
double m5_times_m4[ROWS5 * COLS4];

int main()
{
    print_matrix("m1", m1, ROWS1, COLS1);
    print_matrix("m2", m2, ROWS2, COLS2);
    print_matrix("m3", m3, ROWS3, COLS3);
    print_matrix("m4", m4, ROWS4, COLS4);
    print_matrix("m5", m5, ROWS5, COLS5);

    transpose(m1, ROWS1, COLS1, m1_tr);
    print_matrix("m1_tr",m1_tr, COLS1, ROWS1);

    transpose(m2, ROWS2, COLS2, m2_tr);
    print_matrix("m2_tr",m2_tr, COLS2,ROWS2);

    transpose(m3, ROWS3, COLS3, m3_tr);
    print_matrix("m3_tr",m3_tr, COLS3, ROWS3);

    transpose(m4, ROWS4, COLS4, m4_tr);
    print_matrix("m4_tr", m4_tr, COLS4, ROWS4);

    transpose(m5, ROWS5, COLS5, m5_tr);
    print_matrix("m5_tr", m5_tr, COLS5, ROWS5);
    
    add(m3, m3, ROWS3, COLS3,  m3_plus_m3);
    print_matrix("m3_plus_m3", m3_plus_m3 ,ROWS3, COLS3);

    add(m4,m5_tr, ROWS4, COLS4, m4_plus_m5tr);
    print_matrix("m4_plus_m5tr",m4_plus_m5tr,ROWS4, COLS4);

    add(m5,m4_tr, ROWS5, COLS5, m5_plus_m4tr);
    print_matrix("m5_plus_m4tr",m5_plus_m4tr,ROWS5, COLS5);
    
    mul(m2,m2,ROWS2,COLS2,COLS2,m2_times_m2);
    print_matrix("m2_times_m2",m2_times_m2,ROWS2,COLS2);

    mul(m4,m5,ROWS4,COLS4,COLS5,m4_times_m5);
    print_matrix("m4_times_m5",m4_times_m5,ROWS4,COLS5);

    mul(m5,m4,ROWS5,COLS5,COLS4, m5_times_m4);
    print_matrix("m5_times_m4",m5_times_m4,ROWS5,COLS4);

    return 0;
}