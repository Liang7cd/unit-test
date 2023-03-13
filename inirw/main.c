#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inirw.h"

int printf_test(const char *section)
{
    int Text1 = 0, Text2 = 0, Text3 = 0;
    //autoreboot（自动维护）
   	Text1=iniGetInt(section, "TestText1", 0);
	Text2=iniGetInt(section, "TestText2", 0);
    Text3=iniGetInt(section, "TestText3", 0);
    printf("Text1:[%d], Text2:[%d], Text3:[%d]\n", 
            Text1, Text2, Text3);

    return 0;
}

int main(int argc, char *argv[])
{
    //初始化
    iniFileLoad("./config.ini");

    char *deviceAlarms = "deviceAlarms";
    printf_test(deviceAlarms);
	iniSetInt(deviceAlarms, "TestText1", 0, 10);
	iniSetInt(deviceAlarms, "TestText2", 1, 10);
    iniSetInt(deviceAlarms, "TestText3", 2, 10);
    printf_test(deviceAlarms);
    iniSetInt(deviceAlarms, "TestText1", 3, 10);
	iniSetInt(deviceAlarms, "TestText2", 4, 10);
    iniSetInt(deviceAlarms, "TestText3", 5, 10);
    printf_test(deviceAlarms);

    char *deviceOSD = "deviceOSD";
    printf_test(deviceOSD);
	iniSetInt(deviceOSD, "TestText1", 0, 10);
	iniSetInt(deviceOSD, "TestText2", 1, 10);
    iniSetInt(deviceOSD, "TestText3", 2, 10);
    printf_test(deviceOSD);

    //释放资源
    iniFileFree();

    return 0;
}
