#include "lwip/udp.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"
#include "lwip/ip4_addr.h"  // 包含 IPv4 地址相关的头文件
#include <stdio.h>
#include <string.h>

#define UDP_LOCAL_PORT 12345
#define UDP_REMOTE_PORT 54321
#define UDP_REMOTE_IP "192.168.1.1" // 远程主机的IP地址

// 接收回调函数
static void udp_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip4_addr *addr, u16_t port) {
    printf("Received data from %s:%d\n", ip4addr_ntoa(addr), port);

    // 打印接收到的数据
    char *data = (char *)p->payload;
    printf("Data: %.*s\n", p->len, data);

    // 释放pbuf资源
    pbuf_free(p);
}

// 发送UDP数据
void send_udp_data(struct udp_pcb *upcb) {
    struct pbuf *p;
    ip4_addr_t remote_ip;
    const char *message = "Hello, World!";

    // 解析远程IP地址
    if (!ip4addr_aton(UDP_REMOTE_IP, &remote_ip)) {
        printf("Invalid remote IP address\n");
        return;
    }

    // 创建包含要发送数据的pbuf
    p = pbuf_alloc(PBUF_TRANSPORT, strlen(message), PBUF_RAM);
    if (p == NULL) {
        printf("Failed to allocate pbuf\n");
        return;
    }
    memcpy(p->payload, message, strlen(message));

    // 发送数据
    err_t err = udp_sendto(upcb, &remote_ip, UDP_REMOTE_PORT, p);
    if (err != ERR_OK) {
        printf("Failed to send UDP packet: %d\n", err);
    } else {
        printf("Sent UDP packet to %s:%d\n", ip4addr_ntoa(&remote_ip), UDP_REMOTE_PORT);
    }

    // 释放pbuf资源
    pbuf_free(p);
}

int main() {
    // 初始化LwIP
    lwip_init();

    // 创建一个新的UDP控制块
    struct udp_pcb *pcb = udp_new();
    if (pcb == NULL) {
        printf("Failed to create UDP PCB\n");
        return -1;
    }

    // 绑定UDP控制块到指定的端口
    err_t err = udp_bind(pcb, IP4_ADDR_ANY, UDP_LOCAL_PORT);
    if (err != ERR_OK) {
        printf("Failed to bind UDP PCB: %d\n", err);
        udp_remove(pcb);
        return -1;
    }

    // 注册接收回调函数
    udp_recv(pcb, udp_recv_callback, NULL);

    // 模拟发送UDP数据
    send_udp_data(pcb);

    // 主循环，等待接收数据
    while (1) {
        sys_check_timeouts(); // 检查系统超时
        // 在这里可以添加更多处理逻辑
    }

    // 清理资源
    udp_remove(pcb);

    return 0;
}