#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "so_test.h"

//gcc a_test.c -o a_test.o -I ../so/ -c -fPIC
//ar rcs liba_test.a a_test.o


static int g_a = 0;
static int g_b = 0;

// 信号的标准响应函数：
// 返回值：void
// 参数：int 就是触发该函数的信号
void f(int sig)
{
	printf("收到信号:%d\n", sig);
    so_test_function(g_a, g_b);
    return;
}

int a_test_function(int a, int b)
{
    // 捕捉信号
	signal(SIGUSR1, f);
    g_a = a;
    g_b = b;
    printf("a_test_function: g_a:[%d], g_b:[%d]\n", g_a, g_b);
    return 0;
}
