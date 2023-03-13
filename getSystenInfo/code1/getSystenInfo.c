#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
 
#include <time.h>
#include <pthread.h>
 
static int g_cpu_used_user = 0;		//表示cpu用户使用率
static int g_cpu_used_system = 0;	//表示cpu系统使用率
static unsigned long cpu_total = 0;	
static unsigned long cpu_user = 0;
static unsigned long cpu_system = 0;//分别cpu总时间，用户时间，系统时间
static unsigned int mem_free;		//存放未使用内存
static unsigned int mem_used;		//存放已使用内存
static unsigned int R_bytes = 0;
static unsigned int R_packets = 0;
static short int R_drop = 0;
static unsigned int T_bytes = 0;
static unsigned int T_packets = 0;
static short T_drop = 0;
static short int R_drop_per = 0;	//网络接收丢包率
static short int T_drop_per = 0;	//网络接收丢包率 
 
struct occupy						//存放cpu数据结构体
{
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
};
 
struct mem_occupy					//存放内存数据结构体
{
	char name_total[20];
	unsigned int total;
	char name_free[20];
	unsigned int free;
};
 
struct net_occupy					//存放网口数据结构体
{
	char name[20];
	unsigned int R_bytes;
	unsigned int R_packets;
	unsigned int R_errs;
	unsigned int R_drop;
	unsigned int fifo;
	unsigned int frame;
	unsigned int compressed;
	unsigned int multicast;
	unsigned int T_bytes;
	unsigned int T_packets;
	unsigned int T_errs;
	unsigned int T_drop;
};
 
int cal_occupy(struct occupy *ncpu);
void get_occupy(struct occupy *ocpu);
int get_cpu_info(void);
int get_mem_info(struct mem_occupy *mem);
int get_net_info(void);
static void get_opera_data(void);
 
 
/*******获取cpu数据，存入静态全局变量
g_cpu_used_eser和g_cpu_used_system中*/
int get_cpu_info()
{
	struct occupy cpu;
	get_occupy(&cpu);		//获取数据存入结构体
	cal_occupy(&cpu);		//根据数据计算cpu使用率
	
	printf("cpu_num:[1], user_used:[%d], system_used:[%d]\n",
			 g_cpu_used_user, g_cpu_used_system);
}
 
int cal_occupy(struct occupy *ncpu)
{
	unsigned long od,nd;
	unsigned long id,sd;
	od = cpu_total;
	nd = (unsigned long)(ncpu->user + ncpu->nice + ncpu->system + ncpu->idle);
	id = (unsigned long)(ncpu->user - cpu_user);
	sd = (unsigned long)(ncpu->system - cpu_system);
	if ((nd - od) != 0) {
		g_cpu_used_user = (int)(id * 100)/(nd - od);
		g_cpu_used_system = (int)(sd * 100)/(nd - od);
	} else {
		g_cpu_used_user = 0;
		g_cpu_used_system = 0;
	}
	
	//将计算获得数据储存
	cpu_total = nd;
	cpu_user = ncpu->user;
	cpu_system = ncpu->system;
	return (1);
}
 
/*从/proc/stat文件中提取cpu数据状态*/
void get_occupy(struct occupy *ocpu)
{
	FILE *fd;
	char buff[256];
	struct occupy *ncpu;
	ncpu = ocpu;
 
	fd = fopen("/proc/stat", "r");
	if (!fd) {
		perror("cannot open /proc/stat");
		exit(EXIT_FAILURE);
	}
 
	while(fgets(buff, sizeof(buff), fd)) {
		if(strstr(buff, "cpu") != NULL) {
			sscanf(buff,"%s %u %u %u %u", ncpu->name, &(ncpu->user),
					&(ncpu->nice), &(ncpu->system), &(ncpu->idle));
			break;
		} 
	}
	printf("user:[%u], nice:[%u], system:[%u], idle:[%u]\n",ncpu->user,ncpu->nice,ncpu->system,ncpu->idle); 	
	fclose(fd);
}
 
/*从 /proc/meminfo 文件中提取内存数据*/
int get_mem_info(struct mem_occupy *mem)
{
	FILE *fd;
	char buff[256];
 
	fd = fopen("/proc/meminfo", "r");
	if (!fd) {
		perror("Cannot open /proc/meminfo");
		return(0);
	}
	fgets(buff, sizeof(buff), fd);
	if (strstr(buff, "MemTotal") != NULL) {
		sscanf(buff, "%s %u", mem->name_total, &mem->total);
	} else return(0);
	fgets(buff, sizeof(buff), fd);
	if (strstr(buff, "MemFree") != NULL) {
		sscanf(buff, "%s %u", mem->name_free, &mem->free);
	} else return(0);
	mem_free = mem->free;
	mem_used = mem->total - mem->free;
	fclose(fd);
 
	printf("mem_free:[%u], mem_used:[%u]\n",mem_free,mem_used);
	return (1);
}
 
/*提取网口数据，计算丢包率*/
int get_net_info()
{
	FILE *fd;
	int i, n = 0;
	char buff[256];
	unsigned int R_bytes_new = 0;
	unsigned int R_packets_new = 0;
	unsigned int R_drop_new = 0;
	unsigned int T_bytes_new = 0;
	unsigned int T_packets_new = 0;
	unsigned int T_drop_new = 0;
	struct net_occupy netinfo[4];
 
	fd = fopen("/proc/net/dev", "r+");
	if (!fd) {
		perror("Cannot open /proc/net/dev");
		return(0);
	}
	while(fgets(buff, sizeof(buff), fd)) {
		n++;
		if(n > 2) {
			sscanf(buff,"%s %u %u %u %u %u %u %u %u %u %u %u %u",
				netinfo[n-3].name, &netinfo[n-3].R_bytes, &netinfo[n-3].R_packets, 
				&netinfo[n-3].R_errs, &netinfo[n-3].R_drop, &netinfo[n-3].fifo,
				&netinfo[n-3].frame, &netinfo[n-3].compressed, &netinfo[n-3].multicast,
				&netinfo[n-3].T_bytes, &netinfo[n-3].T_packets, &netinfo[n-3].T_errs,
				&netinfo[n-3].T_drop);
		}
	}
 
	for(i = 0; i < n - 3; i++) {
		R_bytes_new += netinfo[i].R_bytes;
		R_packets_new += netinfo[i].R_packets;
		R_drop_new += netinfo[i].R_drop;
		T_bytes_new += netinfo[i].T_bytes;
		T_packets_new += netinfo[i].T_packets;
		T_drop_new += netinfo[i].T_drop;
		if(strcmp(netinfo[i].name, "wlan0:") == 0)
		{
			printf("R_bytes:[%u], R_packets:[%u], R_drop:[%u], T_bytes:[%u], T_packets:[%u], T_drop:[%u]\n",
					netinfo[i].R_bytes, netinfo[i].R_packets, netinfo[i].R_drop, netinfo[i].T_bytes, netinfo[i].T_packets, netinfo[i].T_drop);
		}
	}
	if (0 == (R_packets_new - R_packets)) {
		R_drop_per = 0;
	}
	else 
		R_drop_per = (unsigned short int)((R_drop_new - R_drop)* 100.0 / 
									(R_packets_new - R_packets));
	if (0 == (T_packets_new - T_packets)) {
		T_drop_per = 0;
	}
	else 
		T_drop_per = (unsigned short int)((T_drop_new - T_drop)* 100.0 / 
									(T_packets_new - T_packets));
 
	printf("R_drop:[%u], T_drop:[%u]\n",R_drop_per,T_drop_per);
	R_bytes = R_bytes_new;
	R_drop = R_drop_new;
	R_packets = R_packets_new;
	T_bytes = T_bytes_new;
	T_drop = T_drop_new;
	T_packets = T_packets_new;
	return(1);
}
 
static void get_opera_data(void)
{	
	struct mem_occupy mem;
	get_cpu_info();
	get_mem_info(&mem);
	get_net_info();
} 
 
int main()
{
	while(1) {
		printf("======================\n");
        get_opera_data();
		sleep(10);
    }
	return(1);
}