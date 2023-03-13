#include <stdio.h>
#include <string.h>

//gcc so_test.c -o so_test.o -c -fPIC
//gcc -shared -fPIC -o libso_test.so so_test.o

int so_test_function(int a, int b)
{
    printf("so_test_function: a:[%d], b:[%d]\n", a, b);
    int sum = a + b;
    printf("sum:[%d]\n", sum);
    return a+b;
}
