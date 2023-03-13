#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <errno.h>

int main ()
{
    int ret;
    ret = mount("/dev/mmcblk0p1", "/mnt/", "vfat", MS_MGC_VAL, NULL);
    if(ret < 0){
       perror("mount error");
       return 0;
    }
    return 0;
}