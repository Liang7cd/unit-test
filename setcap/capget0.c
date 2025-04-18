#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <sys/capability.h>

//arm-fullhanv3-linux-uclibcgnueabi-gcc capget0.c -o capget0 -I libcap-2.66/install/usr/include/ -L libcap-2.66/install/lib64/ -lcap -static
int main()
{
    struct __user_cap_header_struct cap_header_data;
    cap_user_header_t cap_header = &cap_header_data;

    struct __user_cap_data_struct cap_data_data;
    cap_user_data_t cap_data = &cap_data_data;

    cap_header->pid = getpid();
    cap_header->version = _LINUX_CAPABILITY_VERSION_1;

    if (capget(cap_header, cap_data) < 0) {
        perror("Failed capget");
        exit(1);
    }
    printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
        cap_data->permitted, cap_data->inheritable);

    return 0;
}
