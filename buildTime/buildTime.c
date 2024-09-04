#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

const char Software_Ver[]   = "Software: 1.0.0";
const char Compiler_Date[]  = "Date: "__DATE__;
const char Compiler_Time[]  = "Time: "__TIME__;

#define YEAR_I ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
    + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define MONTH_I (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 : 12)

#define DAY_I ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
    + (__DATE__ [5] - '0'))

#define DIGIT_TO_STR(n) \
    ((n) == 0 ? "0" : \
    (n) == 1 ? "1" : \
    (n) == 2 ? "2" : \
    (n) == 3 ? "3" : \
    (n) == 4 ? "4" : \
    (n) == 5 ? "5" : \
    (n) == 6 ? "6" : \
    (n) == 7 ? "7" : \
    (n) == 8 ? "8" : \
    (n) == 9 ? "9" : "Invalid Digit")

#define YEAR_S (__DATE__+7)

#define MONTH_S (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? "01" : "06") \
    : __DATE__ [2] == 'b' ? "02" \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? "03" : "04") \
    : __DATE__ [2] == 'y' ? "05" \
    : __DATE__ [2] == 'l' ? "07" \
    : __DATE__ [2] == 'g' ? "08" \
    : __DATE__ [2] == 'p' ? "09" \
    : __DATE__ [2] == 't' ? "10" \
    : __DATE__ [2] == 'v' ? "11" : "12")

//#define DAY_S (__DATE__[4] == ' ' ? \
                    (__DATE__[5] == '1' ? "01" : \
                    (__DATE__[5] == '2' ? "02" : \
                    (__DATE__[5] == '3' ? "03" : \
                    (__DATE__[5] == '4' ? "04" : \
                    (__DATE__[5] == '5' ? "05" : \
                    (__DATE__[5] == '6' ? "06" : \
                    (__DATE__[5] == '7' ? "07" : \
                    (__DATE__[5] == '8' ? "08" : \
                    (__DATE__[5] == '9' ? "09"))))))))) \
            : (__DATE__[4] == '1' ? \
                    (__DATE__[5] == '0' ? "10" : \
                    (__DATE__[5] == '1' ? "11" : \
                    (__DATE__[5] == '2' ? "12" : \
                    (__DATE__[5] == '3' ? "13" : \
                    (__DATE__[5] == '4' ? "14" : \
                    (__DATE__[5] == '5' ? "15" : \
                    (__DATE__[5] == '6' ? "16" : \
                    (__DATE__[5] == '7' ? "17" : \
                    (__DATE__[5] == '8' ? "18" : \
                    (__DATE__[5] == '9' ? "19"))))))))) \
            : (__DATE__[4] == '2' ? \
                    (__DATE__[5] == '0' ? "20" : \
                    (__DATE__[5] == '1' ? "21" : \
                    (__DATE__[5] == '2' ? "22" : \
                    (__DATE__[5] == '3' ? "23" : \
                    (__DATE__[5] == '4' ? "24" : \
                    (__DATE__[5] == '5' ? "25" : \
                    (__DATE__[5] == '6' ? "26" : \
                    (__DATE__[5] == '7' ? "27" : \
                    (__DATE__[5] == '8' ? "28" : \
                    (__DATE__[5] == '9' ? "29"))))))))) \
            : (__DATE__[4] == '3' ? \
                    (__DATE__[5] == '0' ? "30" : \
                    (__DATE__[5] == '1' ? "31"))))))))
#define DAY_S ""

#define DAY_S_S DIGIT_TO_STR(__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0')
#define DAY_G_S DIGIT_TO_STR(__DATE__ [5] - '0')


int main()
{
    printf("Software_Ver:[%s]\n", Software_Ver);
    printf("Compiler_Date:[%s]\n", Compiler_Date);
    printf("Compiler_Time:[%s]\n", Compiler_Time);
    printf("YEAR_I:[%d], MONTH_I:[%d], DAY_I:[%d]\n", YEAR_I, MONTH_I, DAY_I);
    printf("YEAR_S:[%s], MONTH_S:[%s], DAY_S:[%s], DAY_S_S:[%s], DAY_G_S:[%s]\n", YEAR_S, MONTH_S, DAY_S, DAY_S_S, DAY_G_S);

    unsigned int wCompileData = (YEAR_I%100)*10000 + (MONTH_I%100)*100 + DAY_I;
    printf("wCompileData:[%u]\n", wCompileData);

    return 0;
}