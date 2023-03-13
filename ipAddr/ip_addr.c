#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netinet/in.h>

int GetIP_v4_and_v6_linux(char *interface, int family, char *address, int size)
{
	struct ifaddrs *ifap0, *ifap;
	char buf[NI_MAXHOST];
	struct sockaddr_in *addr4;
	struct sockaddr_in6 *addr6;
	int ret;
	
	if( NULL == address ) {
		return -1;
	}
	
	if(getifaddrs(&ifap0)) {
		return -2;
	}
	
	for( ifap = ifap0; ifap != NULL; ifap=ifap->ifa_next){
		if(strcmp(interface, ifap->ifa_name) != 0) continue;
		if(ifap->ifa_addr==NULL) continue;
		if((ifap->ifa_flags & IFF_UP) == 0) continue;
		if(family != ifap->ifa_addr->sa_family) continue;
		if(AF_INET == ifap->ifa_addr->sa_family) {
			addr4 = (struct sockaddr_in *)ifap->ifa_addr;
			if ( NULL != inet_ntop(ifap->ifa_addr->sa_family,
				(void *)&(addr4->sin_addr), buf, NI_MAXHOST) ){
				if(size <= strlen(buf) ) break;
				strcpy(address, buf);
				freeifaddrs(ifap0);
				return 0;
			}
			else break;
		}
		else if(AF_INET6 == ifap->ifa_addr->sa_family) {
			addr6 = (struct sockaddr_in6 *)ifap->ifa_addr;
			if(IN6_IS_ADDR_MULTICAST(&addr6->sin6_addr)){
				continue;
			}
			
			if(IN6_IS_ADDR_LINKLOCAL(&addr6->sin6_addr)){
				continue;
			}
			
			if(IN6_IS_ADDR_LOOPBACK(&addr6->sin6_addr)){
				continue;
			}
			
			if(IN6_IS_ADDR_UNSPECIFIED(&addr6->sin6_addr)){
				continue;
			}
			
			if(IN6_IS_ADDR_SITELOCAL(&addr6->sin6_addr)){
				continue;
			}
			
			if ( NULL != inet_ntop(ifap->ifa_addr->sa_family,
			(void *)&(addr6->sin6_addr), buf, NI_MAXHOST) ){
				if(size <= strlen(buf) ) break;
				strcpy(address, buf);
				freeifaddrs(ifap0);
				return 0;
			}
			else break;
		}
	}
	
	freeifaddrs(ifap0);
	return -3;
}

int main(int argc, char *argv[])
{
	char *interface = "eth0";
	char ipv4[64] = {0};
	char ipv6[64] = {0};

	int ret1 = GetIP_v4_and_v6_linux(interface, AF_INET, ipv4, sizeof(ipv4));
	int ret2 = GetIP_v4_and_v6_linux(interface, AF_INET6, ipv6, sizeof(ipv6));
	
	if (ret1 != 0 || ret2 != 0) {
		printf("error! ret1:[%d], ret2;[%d]\r\n", ret1, ret2);
	}
	printf("ipv4:[%s], ipv6:[%s].\r\n", ipv4, ipv6);
	
	return 0;
}
