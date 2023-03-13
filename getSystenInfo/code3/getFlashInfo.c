#include <stdio.h>                                                                                                                  
#include <stdlib.h>
#include <unistd.h>
#include <sys/vfs.h>
 
//unit: 0-MBytes, 1-KBytes, default MBytes
int getPartitionUse(const char *dir, unsigned long long *totleSize, unsigned long long *freeSize, int unit)
{
    struct statfs diskInfo;  
    int ret = statfs(dir, &diskInfo);  
	if ( ret != 0 )
	{
		perror("getPartitionUse statfs error ");
		return ret;
	}
	
    unsigned long long allBlocks = diskInfo.f_bsize;  
    unsigned long long tmpTotalSize = allBlocks * diskInfo.f_blocks;  
    unsigned long long tmpFreeDisk = diskInfo.f_bfree*allBlocks;  
    *totleSize = tmpTotalSize>>20;  
	*freeSize  = tmpFreeDisk>>20;
	
	if(unit == 0)
	{	
		*totleSize	= tmpTotalSize>>20;  
		*freeSize  = tmpFreeDisk>>20;  
	}
	else if( unit == 1 )
	{
		*totleSize	= tmpTotalSize>>10;  
		*freeSize  = tmpFreeDisk>>10; 
	}
	
	return 0;
}

void usage(char *program_name)
{
    fprintf(stderr, "\nUsage:  %s  [mounted_dir_name]\n\n", program_name);
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s /dev\n", program_name);
}
 
int main(int argc, char **argv)
{
    int ret = 0;
    if (argc < 2) {
        usage((char*)argv[0]);
        return -1;
    }

	unsigned long long totleSize = 0;
	unsigned long long freeSize = 0;

    getPartitionUse(argv[1], &totleSize, &freeSize, 1);
    printf ("system  total=%lldKB, free=%lldKB\n", totleSize, freeSize);

    return 0;
}