#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>

//获取网络信息结构体
typedef struct {
	unsigned int n_send;
	unsigned int n_recv;
	unsigned int packet_loss;
	double min_time;
	double avg_time;
	double max_time;
	double sum_time;
} marshmallow_network_ping_info_st;

#define ICMP_DATA_LEN 56		//ICMP默认数据长度
#define ICMP_HEAD_LEN 8			//ICMP默认头部长度
#define ICMP_LEN  (ICMP_DATA_LEN + ICMP_HEAD_LEN)
#define SEND_BUFFER_SIZE 128		//发送缓冲区大小
#define RECV_BUFFER_SIZE 128		//接收缓冲区大小
#define MAX_WAIT_TIME 3

/*校验和算法*/
static u_int16_t Compute_cksum(struct icmp *pIcmp, const int len)
{
	int nleft=len;
	u_int16_t *data = (u_int16_t *)pIcmp;
	u_int32_t sum = 0;
	
	/*把ICMP报头二进制数据以2字节为单位累加起来*/
	while (nleft > 1) {
		sum += *data++;
		nleft -= 2;
	}
	/*若ICMP报头为奇数个字节，会剩下最后一字节。把最后一个字节视为一个2字节数据的高字节，这个2字节数据的低字节为0，继续累加*/
	if (1 == nleft) {
		u_int16_t tmp = *data;
		tmp &= 0xff00;
		sum += tmp;
	}

	//ICMP校验和带进位
	while (sum >> 16)
		sum = (sum >> 16) + (sum & 0x0000ffff);
	sum = ~sum;
	return sum;
}

/*获取ICMP报头*/
static int get_ICMP_head(u_int16_t seq, char *icmp_head)
{
	struct icmp *pIcmp;
	struct timeval *pTime;

	pIcmp = (struct icmp*)icmp_head;
	/* 类型和代码分别为ICMP_ECHO,0代表请求回送 */
	pIcmp->icmp_type = ICMP_ECHO;
	pIcmp->icmp_code = 0;
	pIcmp->icmp_cksum = 0;		//校验和
	pIcmp->icmp_seq = seq;		//序号
	pIcmp->icmp_id = getpid();	//取进程号作为标志
	pTime = (struct timeval *)pIcmp->icmp_data;
	gettimeofday(pTime, NULL);	//数据段存放发送时间
	pIcmp->icmp_cksum = Compute_cksum(pIcmp, ICMP_LEN);
	return 0;
}

/*发送ICMP报文*/
static int send_packet(int sock_icmp, struct sockaddr_in *dest_addr, int nSend)
{
	char SendBuffer[SEND_BUFFER_SIZE];
	get_ICMP_head(nSend, SendBuffer);
	if (sendto(sock_icmp, SendBuffer, ICMP_LEN, 0,
		(struct sockaddr *)dest_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("sendto() error!");
		return -1;
	}
	return 0;
}

/*两个timeval结构相减*/
static double get_timeval_Rtt(struct timeval *RecvTime, struct timeval *SendTime)
{
	struct timeval sub = *RecvTime;
	if ((sub.tv_usec -= SendTime->tv_usec) < 0)
	{
		--(sub.tv_sec);
		sub.tv_usec += 1000000;
	}
	sub.tv_sec -= SendTime->tv_sec;
	return sub.tv_sec * 1000.0 + sub.tv_usec / 1000.0; //转换单位为毫秒
}

/*剥去ICMP报头*/
static double unpack_head(struct timeval *RecvTime, char *recv_data, int recv_data_len)
{
	struct ip *Ip = (struct ip *)recv_data;
	struct icmp *Icmp;

	int ipHeadLen = Ip->ip_hl << 2;	/*求ip报头长度,即ip报头的长度标志乘4*/
	Icmp = (struct icmp *)(recv_data + ipHeadLen);/*越过ip报头,指向ICMP报头*/
	recv_data_len-=ipHeadLen;            /*ICMP报头及ICMP数据报的总长度*/
    /*小于ICMP报头长度则不合理*/
    if(recv_data_len < ICMP_HEAD_LEN) {
        printf("ICMP packets\'s length is less than 8\n");
        return -1;
    }

	//判断接收到的报文是否是自己所发报文的响应
	if ((Icmp->icmp_type == ICMP_ECHOREPLY) && Icmp->icmp_id == getpid())
	{
		struct timeval *SendTime = (struct timeval *)Icmp->icmp_data;
		/*计算接收和发送的时间差*/
		double timeRTT = get_timeval_Rtt(RecvTime, SendTime);
		/*显示相关信息*/
		printf("%u bytes from %s: icmp_seq=%u ttl=%u time=%.1f ms\n",
			ntohs(Ip->ip_len) - ipHeadLen,
			inet_ntoa(Ip->ip_src),
			Icmp->icmp_seq,
			Ip->ip_ttl,
			timeRTT);
		return timeRTT;
	}
	return -1;
}

/*接收ICMP报文*/
static double recv_packet(int sock_icmp, struct sockaddr_in *dest_addr)
{
	double timeRTT = 0;
	while(1)
	{
		char RecvBuffer[RECV_BUFFER_SIZE] = {0};
		int addrlen = sizeof(struct sockaddr_in);
		int RecvBytes = recvfrom(sock_icmp, RecvBuffer, RECV_BUFFER_SIZE, 0, (struct sockaddr *)dest_addr, &addrlen);
		if (RecvBytes < 0) {
			perror("recvfrom error!");
			return -1;
		}

		/*记录接收时间*/
		struct timeval RecvTime;
		gettimeofday(&RecvTime, NULL);
		timeRTT = unpack_head(&RecvTime, RecvBuffer, RecvBytes);
		if (0 <= timeRTT) {
			break;
		}
	}
	return timeRTT;
}

//获取ping时延接口
int marshmallow_network_ping(const char *target_addr, const int ping_times, marshmallow_network_ping_info_st *ping_info)
{
	printf("target_addr:[%s], ping_times:[%d]\n", target_addr, ping_times);

	if(ping_info == NULL) {
        lmLogError("ping_info is null!");
        return -1;
    }
    memset(ping_info, 0, sizeof(marshmallow_network_ping_info_st));
	
	struct protoent *protocol;
	if ((protocol = getprotobyname("icmp")) == NULL) {
		perror("getprotobyname");
		return -2;
	}

	/* 创建ICMP套接字 */
	//AF_INET:IPv4, SOCK_RAW:IP协议数据报接口, IPPROTO_ICMP:ICMP协议
	int sock_icmp = socket(PF_INET, SOCK_RAW, protocol->p_proto);
	if (sock_icmp < 0) {
		perror("socket() error!");
		return -3;
	}

	//设置超时
    struct timeval timeout;
    timeout.tv_sec = MAX_WAIT_TIME;//秒
    timeout.tv_usec = 0;//微秒
    if (setsockopt(sock_icmp, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt failed!");
		close(sock_icmp);
        return -4;
    }

	//IPv4专用socket地址,保存目的地址
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	/* 将点分十进制ip地址转换为网络字节序 */
	//ip地址（网络字节序）
	in_addr_t inaddr = inet_addr(target_addr);
	if (inaddr == INADDR_NONE) {
		/* 转换失败，表明是主机名,需通过主机名获取ip */
		struct hostent * pHost = gethostbyname(target_addr);
		if (pHost == NULL) {
			herror("gethostbyname()");
			close(sock_icmp);
			return -5;
		}
		memmove(&dest_addr.sin_addr, pHost->h_addr_list[0], pHost->h_length);
	} else {
		memmove(&dest_addr.sin_addr, &inaddr, sizeof(struct in_addr));
	}
	printf("PING %s(%s): %d bytes data in ICMP packets.\n", target_addr,
            inet_ntoa(dest_addr.sin_addr), ICMP_LEN);

	int num = 0;
	while (num++ < ping_times)
	{
		if(0 != send_packet(sock_icmp, &dest_addr, num)) {
			perror("send_packet() failed!");
			continue;
		}
		ping_info->n_send++;
		
		double timeRtt = recv_packet(sock_icmp, &dest_addr);
		if(timeRtt < 0) {
			perror("recv_packet() failed!");
			continue;
		}
		ping_info->n_recv++;

		if (timeRtt < ping_info->min_time || 0 == ping_info->min_time)
			ping_info->min_time = timeRtt;
		if (timeRtt > ping_info->max_time)
			ping_info->max_time = timeRtt;
		ping_info->sum_time += timeRtt;
	}
	close(sock_icmp);

	//发送或者接收有一个为0，则表示ping失败
	if(!(ping_info->n_send) || !(ping_info->n_recv)) {
		printf("ping error! n_send:[%d], n_recv:[%d]\n", ping_info->n_send, ping_info->n_recv);
		return 0;
	}

	ping_info->avg_time = ping_info->sum_time / ping_info->n_recv;
	ping_info->packet_loss = (float)(ping_info->n_send - ping_info->n_recv) / (float)ping_info->n_send * 100;
	return ping_info->n_recv;
}

void usage(char *program_name)
{
    fprintf(stderr, "\nUsage:  %s  hostname/IP_address ping_times\n\n", program_name);
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s www.baidu.com 1\n", program_name);
}

int main(int argc, char *argv[])
{
    int ret = 0;
    if (argc < 3) {
        usage((char*)argv[0]);
        return -1;
    }

	marshmallow_network_ping_info_st ping_info = {0};
	ret = marshmallow_network_ping(argv[1], atoi(argv[2]), &ping_info);
	if(ret > 0) {
		printf("%d packets transmitted, %d received, %d%% packet loss\n", ping_info.n_send, ping_info.n_recv, ping_info.packet_loss);
		printf("rtt min/avg/max/sum = %.3f/%.3f/%.3f/%.3f ms\n",
			ping_info.min_time, ping_info.avg_time, ping_info.max_time, ping_info.sum_time);
	} else {
		printf("ret:[%d]\n", ret);
	}

	return 0;
}
