#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "a_test.h"

//gcc main.c -o main -I a/ -L a/ -la_test -L so/ -lso_test -Wl,-rpath=./so/

int main(void)
{
    printf("main\n");
    a_test_function(5, 10);

    // 以下函数：等待信号的到来
	// 不断等待信号到来
	while(1)
		pause();

    return 0;
}
