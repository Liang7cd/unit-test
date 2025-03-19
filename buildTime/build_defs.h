/**
 * @file build_defs.h (only one file).
 * @brief 转换编译时间为数字格式以及独立字符形式。
 * @brief 由于部分宏使用复合字面量的解决方案，且因为__DATE__[*]和__TIME__[*]是运行时字符值。（区别于__DATE__和__TIME__）
 * @brief 因此该文件大部分宏不可在函数外全局使用​（例如全局变量初始化），但函数内安全。
 * @date 2025.3.19
 * @author Xiaoman
 * @version 1.0
*/

// Example of __DATE__ string: "Mar  2 2025"
// Example of __DATE__ string: "Mar 19 2025"
// Example of __TIME__ string: "14:36:54"
// Example of __TIME__ string: "06:08:04"
/*
    unsigned int iBuildData = (BUILD_YEAR_I%100)*10000 + (BUILD_MONTH_I%100)*100 + BUILD_DAY_I;
    unsigned int iBuildTime = (BUILD_HOUR_I%100)*10000 + (BUILD_MIN_I%100)*100 + BUILD_SEC_I;
    char sBuildData[9] = {0}, sBuildTime[7] = {0};
    snprintf(sBuildData, sizeof(sBuildData), "%s%s%s", BUILD_YEAR_S, BUILD_MONTH_S, BUILD_DAY_S);
    snprintf(sBuildTime, sizeof(sBuildTime), "%s%s%s", BUILD_HOUR_S, BUILD_MIN_S, BUILD_SEC_S);
*/

#ifndef BUILD_DEFS_H
#define BUILD_DEFS_H

#if 1
// 月份判断宏
#define MONTH_CMP(a,b,c) (__DATE__[0]==a && __DATE__[1]==b && __DATE__[2]==c)

//十二个月的字符判断宏定义
#define BUILD_MONTH_IS_JAN MONTH_CMP('J','a','n')
#define BUILD_MONTH_IS_FEB MONTH_CMP('F','e','b')
#define BUILD_MONTH_IS_MAR MONTH_CMP('M','a','r')
#define BUILD_MONTH_IS_APR MONTH_CMP('A','p','r')
#define BUILD_MONTH_IS_MAY MONTH_CMP('M','a','y')
#define BUILD_MONTH_IS_JUN MONTH_CMP('J','u','n')
#define BUILD_MONTH_IS_JUL MONTH_CMP('J','u','l')
#define BUILD_MONTH_IS_AUG MONTH_CMP('A','u','g')
#define BUILD_MONTH_IS_SEP MONTH_CMP('S','e','p')
#define BUILD_MONTH_IS_OCT MONTH_CMP('O','c','t')
#define BUILD_MONTH_IS_NOV MONTH_CMP('N','o','v')
#define BUILD_MONTH_IS_DEC MONTH_CMP('D','e','c')
#else//更巧妙，但不易读的写法
//十二个月的字符判断宏定义
#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')
#endif

#define BUILD_DATE_IS_BAD (__DATE__[0] == '?')
#define BUILD_TIME_IS_BAD (__TIME__[0] == '?')
/*===============================================================================================*/
/*====================================数字形式宏定义(Start)=======================================*/
//年月日的数字形式宏定义
#define COMPUTE_BUILD_YEAR_I \
    ( \
        (__DATE__[ 7] - '0') * 1000 + \
        (__DATE__[ 8] - '0') *  100 + \
        (__DATE__[ 9] - '0') *   10 + \
        (__DATE__[10] - '0') \
    )

#if 1
#define COMPUTE_BUILD_MONTH_I \
    ( \
        (BUILD_MONTH_IS_JAN) ?  1 : \
        (BUILD_MONTH_IS_FEB) ?  2 : \
        (BUILD_MONTH_IS_MAR) ?  3 : \
        (BUILD_MONTH_IS_APR) ?  4 : \
        (BUILD_MONTH_IS_MAY) ?  5 : \
        (BUILD_MONTH_IS_JUN) ?  6 : \
        (BUILD_MONTH_IS_JUL) ?  7 : \
        (BUILD_MONTH_IS_AUG) ?  8 : \
        (BUILD_MONTH_IS_SEP) ?  9 : \
        (BUILD_MONTH_IS_OCT) ? 10 : \
        (BUILD_MONTH_IS_NOV) ? 11 : \
        (BUILD_MONTH_IS_DEC) ? 12 : \
        /* error default */  0 \
    )
#else//更巧妙，但不易读的写法
#define COMPUTE_BUILD_MONTH_I (__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 1 : 6) \
    : __DATE__[2] == 'b' ? 2 \
    : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
    : __DATE__[2] == 'y' ? 5 \
    : __DATE__[2] == 'l' ? 7 \
    : __DATE__[2] == 'g' ? 8 \
    : __DATE__[2] == 'p' ? 9 \
    : __DATE__[2] == 't' ? 10 \
    : __DATE__[2] == 'v' ? 11 : 12)
#endif

#define COMPUTE_BUILD_DAY_I \
    ( \
        ((__DATE__[4] >= '0') ? (__DATE__[4] - '0') * 10 : 0) + \
        (__DATE__[5] - '0') \
    )

#define BUILD_YEAR_I  ((BUILD_DATE_IS_BAD) ? 0 : COMPUTE_BUILD_YEAR_I)
#define BUILD_MONTH_I ((BUILD_DATE_IS_BAD) ? 0 : COMPUTE_BUILD_MONTH_I)
#define BUILD_DAY_I   ((BUILD_DATE_IS_BAD) ? 0 : COMPUTE_BUILD_DAY_I)
/*===============================================================================================*/
#define COMPUTE_BUILD_HOUR_I ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0')
#define COMPUTE_BUILD_MIN_I  ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0')
#define COMPUTE_BUILD_SEC_I  ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0')

#define BUILD_HOUR_I  ((BUILD_TIME_IS_BAD) ? 0 :  COMPUTE_BUILD_HOUR_I)
#define BUILD_MIN_I   ((BUILD_TIME_IS_BAD) ? 0 :  COMPUTE_BUILD_MIN_I)
#define BUILD_SEC_I   ((BUILD_TIME_IS_BAD) ? 0 :  COMPUTE_BUILD_SEC_I)
/*======================================数字形式宏定义(End)=======================================*/
/*===============================================================================================*/

/*===============================================================================================*/
/*====================================字符形式宏定义(Start)=======================================*/
//年月日的字符形式宏定义
#define COMPUTE_BUILD_YEAR_S (&(__DATE__[7]))

#if 1
#define COMPUTE_BUILD_MONTH_S \
    ( \
        (BUILD_MONTH_IS_JAN) ? "01" : \
        (BUILD_MONTH_IS_FEB) ? "02" : \
        (BUILD_MONTH_IS_MAR) ? "03" : \
        (BUILD_MONTH_IS_APR) ? "04" : \
        (BUILD_MONTH_IS_MAY) ? "05" : \
        (BUILD_MONTH_IS_JUN) ? "06" : \
        (BUILD_MONTH_IS_JUL) ? "07" : \
        (BUILD_MONTH_IS_AUG) ? "08" : \
        (BUILD_MONTH_IS_SEP) ? "09" : \
        (BUILD_MONTH_IS_OCT) ? "10" : \
        (BUILD_MONTH_IS_NOV) ? "11" : \
        (BUILD_MONTH_IS_DEC) ? "12" : \
        /* error default */  "00" \
    )
#else//更巧妙，但不易读的写法
#define COMPUTE_BUILD_MONTH_S (__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? "01" : "06") \
    : __DATE__[2] == 'b' ? "02" \
    : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? "03" : "04") \
    : __DATE__[2] == 'y' ? "05" \
    : __DATE__[2] == 'l' ? "07" \
    : __DATE__[2] == 'g' ? "08" \
    : __DATE__[2] == 'p' ? "09" \
    : __DATE__[2] == 't' ? "10" \
    : __DATE__[2] == 'v' ? "11" : "12")
#endif

#if 1
#define COMPUTE_BUILD_DAY_S ((char[]){ (__DATE__[4] == ' ' ? '0' : __DATE__[4]), __DATE__[5], '\0' })
#else// 更简洁易读的写法
// 天数判断宏
#define DAY_CMP(a,b) (__DATE__[4]==a && __DATE__[5]==b)

// 天数字符转换
#define COMPUTE_BUILD_DAY_S \
(DAY_CMP(' ','1') ? "01" :  \
 DAY_CMP(' ','2') ? "02" :  \
 DAY_CMP(' ','3') ? "03" :  \
 DAY_CMP(' ','4') ? "04" :  \
 DAY_CMP(' ','5') ? "05" :  \
 DAY_CMP(' ','6') ? "06" :  \
 DAY_CMP(' ','7') ? "07" :  \
 DAY_CMP(' ','8') ? "08" :  \
 DAY_CMP(' ','9') ? "09" :  \
 DAY_CMP('1','0') ? "10" :  \
 DAY_CMP('1','1') ? "11" :  \
 DAY_CMP('1','2') ? "12" :  \
 DAY_CMP('1','3') ? "13" :  \
 DAY_CMP('1','4') ? "14" :  \
 DAY_CMP('1','5') ? "15" :  \
 DAY_CMP('1','6') ? "16" :  \
 DAY_CMP('1','7') ? "17" :  \
 DAY_CMP('1','8') ? "18" :  \
 DAY_CMP('1','9') ? "19" :  \
 DAY_CMP('2','0') ? "20" :  \
 DAY_CMP('2','1') ? "21" :  \
 DAY_CMP('2','2') ? "22" :  \
 DAY_CMP('2','3') ? "23" :  \
 DAY_CMP('2','4') ? "24" :  \
 DAY_CMP('2','5') ? "25" :  \
 DAY_CMP('2','6') ? "26" :  \
 DAY_CMP('2','7') ? "27" :  \
 DAY_CMP('2','8') ? "28" :  \
 DAY_CMP('2','9') ? "29" :  \
 DAY_CMP('3','0') ? "30" :  \
 DAY_CMP('3','1') ? "31" : "00")
#endif

#define BUILD_YEAR_S  ((BUILD_DATE_IS_BAD) ? "00" : COMPUTE_BUILD_YEAR_S)
#define BUILD_MONTH_S ((BUILD_DATE_IS_BAD) ? "00" : COMPUTE_BUILD_MONTH_S)
#define BUILD_DAY_S   ((BUILD_DATE_IS_BAD) ? "00" : COMPUTE_BUILD_DAY_S)
/*===============================================================================================*/
#define COMPUTE_BUILD_HOUR_S ((char[]){ __TIME__[0], __TIME__[1], '\0' })
#define COMPUTE_BUILD_MIN_S  ((char[]){ __TIME__[3], __TIME__[4], '\0' })
#define COMPUTE_BUILD_SEC_S  ((char[]){ __TIME__[6], __TIME__[7], '\0' })

#define BUILD_HOUR_S  ((BUILD_TIME_IS_BAD) ? "00" :  COMPUTE_BUILD_HOUR_S)
#define BUILD_MIN_S   ((BUILD_TIME_IS_BAD) ? "00" :  COMPUTE_BUILD_MIN_S)
#define BUILD_SEC_S   ((BUILD_TIME_IS_BAD) ? "00" :  COMPUTE_BUILD_SEC_S)
/*======================================字符形式宏定义(End)=======================================*/
/*===============================================================================================*/
#endif // BUILD_DEFS_H

