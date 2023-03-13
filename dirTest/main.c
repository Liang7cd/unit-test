#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define  RECORD_DIR   "/home/l/share/temp/dirTest/"

static int record_delete_empty_dir()
{
    DIR *dp = opendir(RECORD_DIR);
	if(dp == NULL)
	{
		printf("open dir fail![%s]\n", RECORD_DIR);
		return -1;
	}
	struct dirent *ep = NULL;
    while((ep = readdir(dp)) != NULL)
	{
		if(ep->d_type == DT_DIR && ep->d_name[0] != '.')
		{
            char dir_name[256] = {0};
            sprintf(dir_name, "%s%s", RECORD_DIR, ep->d_name);
            DIR *dir = opendir(dir_name);
            if(dir == NULL)
            {
                printf("open dir fail![%s]\n", dir_name);
                continue;
            }
            struct dirent *ptr = NULL;
            while((ptr=readdir(dir)) != NULL)
            {
                if(strcmp(".",ptr->d_name)!=0 && strcmp("..",ptr->d_name)!= 0) {
                    printf("skip dir, not null.[%s]\n", dir_name);
                    break;
                }
            }
            if(ptr == NULL) {
                printf("del dir[%s]\n", dir_name);
                remove(dir_name);
            }
        }else{
            printf("skip file:[%s]\n", ep->d_name);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    record_delete_empty_dir();
    return 0;
}
