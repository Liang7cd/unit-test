#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>

#define VERSION_3           3
#define VERSION_4           4
 
#define MODE_CLIENT         3
#define MODE_SERVER         4
 
 
#define NTP_LI              0
#define NTP_VN              VERSION_3   
#define NTP_MODE            MODE_CLIENT
#define NTP_STRATUM         0
#define NTP_POLL            4
#define NTP_PRECISION       -6
 
#define NTP_HLEN            48
 
#define NTP_PORT            123
#define NTP_SERVER          "182.92.12.11"
 
#define TIMEOUT             1
 
#define BUFSIZE             1500
 
#define JAN_1970            0x83aa7e80
 
#define NTP_CONV_FRAC32(x)  (uint64_t) ((x) * ((uint64_t)1<<32))    
#define NTP_REVE_FRAC32(x)  ((double) ((double) (x) / ((uint64_t)1<<32)))   
 
#define NTP_CONV_FRAC16(x)  (uint32_t) ((x) * ((uint32_t)1<<16))    
#define NTP_REVE_FRAC16(x)  ((double)((double) (x) / ((uint32_t)1<<16)))    
 
 
#define USEC2FRAC(x)        ((uint32_t) NTP_CONV_FRAC32( (x) / 1000000.0 )) 
#define FRAC2USEC(x)        ((uint32_t) NTP_REVE_FRAC32( (x) * 1000000.0 )) 
 
 
#define NTP_LFIXED2DOUBLE(x)    ((double) ( ntohl(((struct l_fixedpt *) (x))->intpart) - JAN_1970 + FRAC2USEC(ntohl(((struct l_fixedpt *) (x))->fracpart)) / 1000000.0 ))   
 
 
struct s_fixedpt {
    uint16_t    intpart;
    uint16_t    fracpart;
};
 
struct l_fixedpt {
    uint32_t    intpart;
    uint32_t    fracpart;
};
 
 
struct ntphdr {
#if __BYTE_ORDER == __BID_ENDIAN
    unsigned int    ntp_li:2;
    unsigned int    ntp_vn:3;
    unsigned int    ntp_mode:3;
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int    ntp_mode:3;
    unsigned int    ntp_vn:3;
    unsigned int    ntp_li:2;
#endif
    uint8_t         ntp_stratum;
    uint8_t         ntp_poll;
    int8_t          ntp_precision;
    struct s_fixedpt    ntp_rtdelay;
    struct s_fixedpt    ntp_rtdispersion;
    uint32_t            ntp_refid;
    struct l_fixedpt    ntp_refts;
    struct l_fixedpt    ntp_orits;
    struct l_fixedpt    ntp_recvts;
    struct l_fixedpt    ntp_transts;
};

//国内可用的NTP服务器地址
#define NTP_SERVER_SUM      4
static char server_NTP[NTP_SERVER_SUM][30] = {"ntp.aliyun.com", 
                                            "cn.ntp.org.cn", 
                                            "time.windows.com", 
                                            "cn.pool.ntp.org"};
