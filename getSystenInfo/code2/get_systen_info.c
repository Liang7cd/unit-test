#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "get_systen_info.h"

//======================================获取cpu信息=====================================//
static unsigned long g_cpu_total = 0;	//cpu总时间
static unsigned long g_cpu_user = 0;	//cpu用户时间
static unsigned long g_cpu_system = 0;	//cpu系统时间

//存放cpu数据结构体
struct occupy
{
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
};
 
//从“/proc/stat”文件中提取cpu数据状态
int get_occupy(struct occupy *ocpu)
{
	FILE *fd;
	char buff[256];
	struct occupy *ncpu;
	ncpu = ocpu;

	fd = fopen("/proc/stat", "r");
	if (!fd) {
		perror("cannot open /proc/stat");
		return -1;
	}

	while(fgets(buff, sizeof(buff), fd)) {
		if(strstr(buff, "cpu") != NULL) {
			sscanf(buff,"%s %u %u %u %u", ncpu->name, &(ncpu->user),
					&(ncpu->nice), &(ncpu->system), &(ncpu->idle));
			break;
		} 
	}
	fclose(fd);
	return 0;
}

//获取cpu数据，并计算使用率
int get_cpu_info(cpu_info_st *cpu_info)
{
	//获取数据存入结构体
	struct occupy cpu = {0};
	if(0!=get_occupy(&cpu))
		return -1;

	//根据数据计算cpu使用率
	unsigned long od,nd;
	unsigned long id,sd;
	od = g_cpu_total;
	nd = (unsigned long)(cpu.user + cpu.nice + cpu.system + cpu.idle);
	id = (unsigned long)(cpu.user - g_cpu_user);
	sd = (unsigned long)(cpu.system - g_cpu_system);
	if ((nd - od) != 0) {
		cpu_info->cpu_used_user = (int)(id * 100)/(nd - od);
		cpu_info->cpu_used_system = (int)(sd * 100)/(nd - od);
	} else {
		cpu_info->cpu_used_user = 0;
		cpu_info->cpu_used_system = 0;
	}

	//将计算获得数据储存
	g_cpu_total = nd;
	g_cpu_user = cpu.user;
	g_cpu_system = cpu.system;
	return 0;
}
//=====================================================================================//

//======================================获取内存信息=====================================//
//存放内存数据结构体
struct mem_occupy
{
	char name_total[20];
	unsigned int total;
	char name_free[20];
	unsigned int free;
	char name_available[20];
	unsigned int available;
};

//从“/proc/meminfo”文件中提取内存数据
int get_memory_info(memory_info_st *mem_info)
{
	FILE *fd;
	char buff[256];
	struct mem_occupy mem;

	fd = fopen("/proc/meminfo", "r");
	if (!fd) {
		perror("Cannot open /proc/meminfo");
		return -1;
	}

	fgets(buff, sizeof(buff), fd);
	if (strstr(buff, "MemTotal") != NULL) {
		sscanf(buff, "%s %u", mem.name_total, &mem.total);
	} else {
		fclose(fd);
		return -2;
	}
	fgets(buff, sizeof(buff), fd);
	if (strstr(buff, "MemFree") != NULL) {
		sscanf(buff, "%s %u", mem.name_free, &mem.free);
	} else {
		fclose(fd);
		return -2;
	}
	fgets(buff, sizeof(buff), fd);
	if (strstr(buff, "MemAvailable") != NULL) {
		sscanf(buff, "%s %u", mem.name_available, &mem.available);
	} else {
		fclose(fd);
		return -2;
	}
	mem_info->MemTotal = mem.total;
	mem_info->MemFree = mem.free;
	mem_info->MemAvailable = mem.available;

	fclose(fd);
	return 0;
}
//======================================================================================//

//======================================获取网络信息=====================================//
static unsigned int g_R_bytes = 0;
static unsigned int g_R_packets = 0;
static short int g_R_drop = 0;
static unsigned int g_T_bytes = 0;
static unsigned int g_T_packets = 0;
static short g_T_drop = 0;

//存放网口数据结构体
struct net_occupy
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

//从“/proc/net/dev”文件中提取网口数据，计算丢包率
int get_network_info(network_info_st *net_info)
{
	FILE *fd;
	int i, n = 0;
	char buff[256];
	struct net_occupy netinfo[4] = {0};

	fd = fopen("/proc/net/dev", "r+");
	if (!fd) {
		perror("Cannot open /proc/net/dev");
		return -1;
	}

	//循环读取所有网卡数据
	while(fgets(buff, sizeof(buff), fd)) {
		//过滤掉前面两行
		if((++n) < 3)
			continue;
		
		sscanf(buff,"%s %u %u %u %u %u %u %u %u %u %u %u %u",
			netinfo[n-3].name, &netinfo[n-3].R_bytes, &netinfo[n-3].R_packets, 
			&netinfo[n-3].R_errs, &netinfo[n-3].R_drop, &netinfo[n-3].fifo,
			&netinfo[n-3].frame, &netinfo[n-3].compressed, &netinfo[n-3].multicast,
			&netinfo[n-3].T_bytes, &netinfo[n-3].T_packets, &netinfo[n-3].T_errs,
			&netinfo[n-3].T_drop);
		
		if((n+1)-3 > 3)
			break;
	}

	//累加各个网卡的数据，求总和
	for(i = 0; i < n - 3; i++) {
		net_info->R_bytes += netinfo[i].R_bytes;
		net_info->R_packets += netinfo[i].R_packets;
		net_info->R_drop += netinfo[i].R_drop;
		net_info->T_bytes += netinfo[i].T_bytes;
		net_info->T_packets += netinfo[i].T_packets;
		net_info->T_drop += netinfo[i].T_drop;
	}

	//计算网络接收丢包率
	if (0 == (net_info->R_packets - g_R_packets)) {
		net_info->R_drop_per = 0;
	} else {
		net_info->R_drop_per = (unsigned short int)((net_info->R_drop - g_R_drop)* 100.0 / (net_info->R_packets - g_R_packets));
	}
	//计算网络发送丢包率
	if (0 == (net_info->T_packets - g_T_packets)) {
		net_info->T_drop_per = 0;
	} else {
		net_info->T_drop_per = (unsigned short int)((net_info->T_drop - g_T_drop)* 100.0 / (net_info->T_packets - g_T_packets));
	}

	g_R_bytes = net_info->R_bytes;
	g_R_packets = net_info->R_packets;
	g_R_drop = net_info->R_drop;
	g_T_bytes = net_info->T_bytes;
	g_T_packets = net_info->T_packets;
	g_T_drop = net_info->T_drop;

	fclose(fd);
	return 0;
}
//======================================================================================//

int main()
{
	while(1)
	{
		//获取cpu使用率
    	cpu_info_st cpu_info = {0};
    	get_cpu_info(&cpu_info);

    	//获取内存信息
    	memory_info_st mem_info = {0};
    	get_memory_info(&mem_info);
		printf("MemTotal:[%u], MemAvailable:[%u]\n", mem_info.MemTotal, mem_info.MemAvailable);

    	//获取网络信息
    	network_info_st net_info = {0};
    	get_network_info(&net_info);
		printf("R_drop:[%hd], T_drop:[%hd], R_packets:[%u], T_packets:[%u]\n", 
				net_info.R_drop, net_info.T_drop, net_info.R_packets, net_info.T_packets);
	
    	int packetLossRate = (int)((unsigned long)(net_info.R_drop+net_info.T_drop)*100/(unsigned long)(net_info.R_packets+net_info.T_packets));
		int cpuLoad = cpu_info.cpu_used_user;
		int memoryLoad = (int)((unsigned long)(mem_info.MemTotal-mem_info.MemAvailable)*100/(unsigned long)mem_info.MemTotal);
		int memoryAvailable = mem_info.MemAvailable;
		printf("packetLossRate:[%d], cpuLoad:[%d], memoryLoad:[%d], memoryAvailable:[%d]\n", 
    	            packetLossRate, cpuLoad, memoryLoad, memoryAvailable);

		sleep(10);
	}
	return 0;
}
