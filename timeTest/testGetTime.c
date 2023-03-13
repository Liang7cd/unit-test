#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

int main()
{
 
    struct timeval tv;
    struct timezone tz;
    gettimeofday (&tv, &tz);
    printf("tv_sec; %d\n", tv.tv_sec);
    printf("tv_usec; %d\n", tv.tv_usec);
    printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
    printf("tz_dsttime, %d\n", tz.tz_dsttime);
    
    for(int i=0; i<7; i++)
    {
        struct timeval tv1;
        tv1.tv_sec = tv.tv_sec + i*(24*60*60);
        char *s_time = ctime((time_t *) &tv1.tv_sec);

        struct tm *ptm = NULL;
        ptm = localtime(&(tv1.tv_sec));
        char s_week[4];
        strftime(s_week, sizeof(s_week), "%a", ptm);
        char i_week[2];
        strftime(i_week, sizeof(i_week), "%u", ptm);

        printf("%d len: %d, time: %s s_week: %s, i_week: %s, atoi(i_week): %d\n", i, strlen(s_time), s_time, s_week, i_week, atoi(i_week));
    }

    struct tm tm;
    time_t t;
    int diff = 0;
    char *datetime = "08:00:00";
    strptime(datetime, "%H:%M:%S", &tm);
    printf("h:%d, m:%d, s:%d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
	t = mktime(&tm);
    diff = (int)difftime(t, tv.tv_sec);
    printf("t: %d, tv.tv_sec: %d, diff: %d\n", t, tv.tv_sec, diff);
    printf("ct: %s\n", ctime((time_t *) &t));

    datetime = "2021-11-10 08:00:00";
    strptime(datetime, "%Y-%m-%d %H:%M:%S", &tm);
    printf("h:%d, m:%d, s:%d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
	t = mktime(&tm);
    diff = (int)difftime(t, tv.tv_sec);
    printf("t: %d, tv.tv_sec: %d, diff: %d\n", t, tv.tv_sec, diff);
    printf("ct: %s\n", ctime((time_t *) &t));

    return 0;
}