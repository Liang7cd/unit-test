#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ptz_preset.h"

int main(int argc, char *argv[])
{
    char inputCmd[80]={0};
    //ptz机位链表初始化
	ptz_preset_init();
    do{
        printf("请输入命令([search],[add],[del],[print],[quit]):");
        scanf("%s", inputCmd);
        while(getchar() != '\n');//清空缓存区
        if(0 == strcmp(inputCmd, "search")){
            int speed = 0, pos_h = 0, pos_v = 0;
            printf("请输入要查找的索引号:");
	        while(!(scanf("%d", &speed) && getchar() == '\n')) {
	        	while(getchar() != '\n');//清空缓存区
	        	printf("你输入的数据有误，请再输一遍！\n");
	        }
            if (ptz_preset_search(speed, &pos_h, &pos_v) != 0) {
				printf("preset: %d not found!\n", speed);
			} else {
                printf("speed:[%d], pos_h:[%d], pos_v:[%d]\n", speed, pos_h, pos_v);
			}
            break;
        }
        else if(0 == strcmp(inputCmd, "add")){
            int speed = 0, pos_h = 0, pos_v = 0;
            printf("请输入要添加的索引号:");
	        while(!(scanf("%d", &speed) && getchar() == '\n')) {
	        	while(getchar() != '\n');//清空缓存区
	        	printf("你输入的数据有误，请再输一遍！\n");
	        }
            printf("请输入坐标[h,v]:");
            while(!(2==scanf("%d,%d", &pos_h, &pos_v) && getchar() == '\n')) {
	        	while(getchar() != '\n');//清空缓存区
	        	printf("你输入的数据有误，请再输一遍！\n");
	        }
			printf("set speed:[%d], pos_h:[%d], pos_v:[%d]\n", speed, pos_h, pos_v);
			ptz_preset_add(speed, pos_h++, pos_v++);
        }
		else if (0 == strcmp(inputCmd, "del")) {
            int speed = 0;
            printf("请输入要删除的索引号:");
	        while(!(scanf("%d", &speed) && getchar() == '\n')) {
	        	while(getchar() != '\n');//清空缓存区
	        	printf("你输入的数据有误，请再输一遍！\n");
	        }
			printf("del speed:[%d]\n", speed);
			ptz_preset_del(speed);
		}
        else if (0 == strcmp(inputCmd, "print")) {
            #define PTZ_PRESET_MAX	256
	        for (int speed = 0; speed < PTZ_PRESET_MAX; speed++)
	        {
                int ret = 0, pos_h = 0, pos_v = 0;
	        	ret = ptz_preset_search(speed, &pos_h, &pos_v);
	        	if (ret == 0) {
                    printf("speed:[%d], pos_h:[%d], pos_v:[%d]\n", speed, pos_h, pos_v);
	        	}
	        }
        }
        else if (0 == strcmp(inputCmd, "quit")) {
            break;
        }
        else {
            printf("=======in error======\n\n");
        }
    }while(1);
    return 0;
}