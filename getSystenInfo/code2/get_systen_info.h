#ifndef __GET_SYSTEN_INFO_H__
#define __GET_SYSTEN_INFO_H__

#ifdef __cplusplus 
extern "C"
{
#endif

//获取CPU信息结构体
typedef struct {
    unsigned int cpu_used_user;	    //表示cpu用户使用率
    unsigned int cpu_used_system;	//表示cpu系统使用率
} cpu_info_st;

//获取内存信息结构体
typedef struct {
    unsigned int MemTotal;      //内存总数
    unsigned int MemFree;       //空闲内存数
    unsigned int MemAvailable;  //可用内存数（可回收使用内存）
} memory_info_st;

//获取网络信息结构体
typedef struct {
    unsigned int R_bytes;   
    unsigned int R_packets;
    short int R_drop;
    unsigned int T_bytes;
    unsigned int T_packets;
    short T_drop;
    short int R_drop_per;	//网络接收丢包率
    short int T_drop_per;	//网络发送丢包率
} network_info_st;
 
//获取cpu信息
int get_cpu_info(cpu_info_st *cpu_info);
//获取内存信息
int get_memory_info(memory_info_st *mem_info);
//获取网络信息
int get_network_info(network_info_st *net_info);
//获取wifi强度接口
int get_wifi_RSSI(char *wifi_ssid);

#ifdef __cplusplus
}
#endif


#endif	// end of __GET_SYSTEN_INFO_H__