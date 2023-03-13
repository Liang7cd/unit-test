#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#include "getBrightness.h"

//gcc getBrightness.c bmp.c jpg.c -o getBrightness -ljpeg
int main(int argc, char *argv[])
{
	float bright = 0;
	rgb_info_pointer rgbinfo;
	if(strstr(argv[1], ".bmp")) {
		rgbinfo = get_bmp_file_info(argv[1]);
		bright = get_bmp_brightness(rgbinfo->rgb_buffer, rgbinfo->imageinfo);
	}else if(strstr(argv[1], ".jpg")) {
		rgbinfo = get_jpg_file_info(argv[1]);
		bright = get_jpg_brightness(rgbinfo->rgb_buffer, rgbinfo->imageinfo);
	}
	printf("bright:[%f]\n", bright);

	free(rgbinfo->rgb_buffer);
	free(rgbinfo);
	return 0;
}
