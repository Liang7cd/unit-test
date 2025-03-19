#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include "build_defs.h"

const char Software_Ver[]   = "Software: 1.1.0";
const char Compiler_Date[]  = "Date: "__DATE__;
const char Compiler_Time[]  = "Time: "__TIME__;

int main()
{
    printf("Software_Ver:[%s]\n", Software_Ver);
    printf("Compiler_Date:[%s]\n", Compiler_Date);
    printf("Compiler_Time:[%s]\n", Compiler_Time);

    printf("BUILD_YEAR_I:[%d], BUILD_MONTH_I:[%d], BUILD_DAY_I:[%d]\n", BUILD_YEAR_I, BUILD_MONTH_I, BUILD_DAY_I);
    printf("BUILD_HOUR_I:[%d], BUILD_MIN_I:[%d], BUILD_SEC_I:[%d]\n", BUILD_HOUR_I, BUILD_MIN_I, BUILD_SEC_I);
    unsigned int iBuildData = (BUILD_YEAR_I%100)*10000 + (BUILD_MONTH_I%100)*100 + BUILD_DAY_I;
    unsigned int iBuildTime = (BUILD_HOUR_I%100)*10000 + (BUILD_MIN_I%100)*100 + BUILD_SEC_I;
    printf("iBuildData:[%u], iBuildTime:[%d]\n", iBuildData, iBuildTime);

    printf("BUILD_YEAR_S:[%s], BUILD_MONTH_S:[%s], BUILD_DAY_S:[%s]\n", BUILD_YEAR_S, BUILD_MONTH_S, BUILD_DAY_S);
    printf("BUILD_HOUR_S:[%s], BUILD_MIN_S:[%s], BUILD_SEC_S:[%s]\n", BUILD_HOUR_S, BUILD_MIN_S, BUILD_SEC_S);
    char sBuildData[9] = {0}, sBuildTime[7] = {0};
    snprintf(sBuildData, sizeof(sBuildData), "%s%s%s", BUILD_YEAR_S, BUILD_MONTH_S, BUILD_DAY_S);
    snprintf(sBuildTime, sizeof(sBuildTime), "%s%s%s", BUILD_HOUR_S, BUILD_MIN_S, BUILD_SEC_S);
    printf("sBuildData:[%s], sBuildTime:[%s]\n", sBuildData, sBuildTime);
    return 0;
}
