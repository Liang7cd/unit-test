#ifndef __JPEG_H__
#define __JPEG_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "jpeglib.h"

//矩形区域信息结构体
typedef struct {
    int top_left_x;          /*!< 矩形区域左上点水平方向上的坐标 */
    int top_left_y;          /*!< 矩形区域左上点垂直方向上的坐标 */
    unsigned int width;    /*!< 矩形区域的宽度 */
    unsigned int height;   /*!< 矩形区域的高度 */
} jpeg_rect_st;

typedef struct{
	int left_up_x;
	int left_up_y;
	int left_bottom_x;
	int left_bottom_y;
	int right_up_x;
	int right_up_y;
	int right_bottom_x;
	int right_bottom_y;
} jpeg_zone_st;

typedef struct 
{
	unsigned int width;
	unsigned int height;
	unsigned int pixel_size;
} image_info;

typedef struct rgb_info
{
	unsigned long rgb_size;
	unsigned char *rgb_data;
	image_info imageinfo;
}rgb_info_st;

int jpeg_decode(char *in_file_path, rgb_info_st *rgb_info_out);
#endif
