/* ntpclient.c */
#include "marshmallow_ntp.h"

 
//把字符串地址转换成ip地址
static in_addr_t inet_host(const char *host)
{
    in_addr_t saddr;
    struct hostent *hostent;
 
    if ((saddr = inet_addr(host)) == INADDR_NONE) 
    {
        if ((hostent = gethostbyname(host)) == NULL)
            return INADDR_NONE;
 
        memmove(&saddr, hostent->h_addr, hostent->h_length);
    }
 
    return saddr;
}

//获取ntp请求数据包
static int get_ntp_packet(void *buf, size_t *size)
{
    struct ntphdr *ntp;
    struct timeval tv;
 
    if (!size || *size<NTP_HLEN)
        return -1;
 
    memset(buf, 0, *size);
 
    ntp = (struct ntphdr *) buf;
    ntp->ntp_li = NTP_LI;
    ntp->ntp_vn = NTP_VN;
    ntp->ntp_mode = NTP_MODE;
    ntp->ntp_stratum = NTP_STRATUM;
    ntp->ntp_poll = NTP_POLL;
    ntp->ntp_precision = NTP_PRECISION;
 
    gettimeofday(&tv, NULL);
    ntp->ntp_transts.intpart = htonl(tv.tv_sec + JAN_1970);
    ntp->ntp_transts.fracpart = htonl(USEC2FRAC(tv.tv_usec));
 
    *size = NTP_HLEN;
 
    return 0;
}

//计算获取ntp时间的信息来回耗时
static double get_offset(const struct ntphdr *ntp, const struct timeval *recvtv)
{
    double t1, t2, t3, t4;
 
    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;
 
    return ((t2 - t1) + (t3 - t4)) / 2;
}

int marshmallow_network_set_NTP()
{
    int maxfd1;
    fd_set readfds;
    struct timeval timeout, recvtv, time_NTP;

    //创建TCP套接字
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        printf("socket error\n");
        return -1;
    }

    //存储地址信息的结构体
    int server_num = 0;
    struct sockaddr_in servaddr_NTP;
    for(server_num = 0; server_num < NTP_SERVER_SUM; server_num++)
    {
        //清空地址结构体
        socklen_t len = sizeof(servaddr_NTP);
	    bzero(&servaddr_NTP, len);
        //记录NTP服务器地址
        servaddr_NTP.sin_family = AF_INET;
        servaddr_NTP.sin_port = htons(NTP_PORT);
        servaddr_NTP.sin_addr.s_addr = inet_host(server_NTP[server_num]);

        //进行连接
        if(connect(sockfd, (struct sockaddr *) &servaddr_NTP, sizeof(struct sockaddr)) != 0) 
        {
            printf("connect error\r\n");
            continue;
        }

        //获取ntp申请数据包
        char buf[BUFSIZE];
        size_t size_buf = sizeof(buf);
        if (get_ntp_packet(buf, &size_buf) != 0) 
        {
            printf("construct ntp request error \n");
            return -2;
        }
        //发送NTP申请数据包
        send(sockfd, buf, size_buf, 0);
 
        //设置等待队列
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfd1 = sockfd + 1;
    
        //设置等待时间
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        //等待NTP消息回包
        if (select(maxfd1, &readfds, NULL, NULL, &timeout) > 0) 
        {
            //判断是否有挂起的等待接收
            if (FD_ISSET(sockfd, &readfds)) 
            {
                //接收数据
                int nbytes = 0;
                if ((nbytes = recv(sockfd, buf, BUFSIZE, 0)) < 0) 
                {
                    printf("recv error\n");
                    continue;
                }

                //计算客户端时间与服务器端时间偏移量
                double offset;
                gettimeofday(&recvtv, NULL);
                offset = get_offset((struct ntphdr *) buf, &recvtv);
                //更新系统时间
                gettimeofday(&time_NTP, NULL);
                time_NTP.tv_sec += (int) offset;
                time_NTP.tv_usec += offset - (int) offset;

                printf("%s \n", ctime((time_t *) &time_NTP.tv_sec));
                /*
                if (settimeofday(&tv, NULL) != 0) 
                {
                    printf("settimeofday error");
                    return -3;
                }
                */
                close(sockfd);
                break;
            }
        }else{
            printf("%s out time!\n", server_NTP[server_num]);
        }
    }
    if(server_num == NTP_SERVER_SUM)
    {
        printf( "all server out time!\n");
        return -4;
    }
    close(sockfd);
    return 0;
}
 
int main(int argc, char *argv[])
{
    int ret = 0;
    ret = marshmallow_network_set_NTP();
    if(ret != 0)
    {
        printf("marshmallow_network_set_NTP error ret: %d\r\n", ret);
    }

    return 0;
}
