#ifndef __GETBRIGHTNESS_H__
#define __GETBRIGHTNESS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

#include <linux/fb.h>

//==============================bmp图像读取需要结构体=============================//
struct bitmap_header
{
	int16_t type;
	int32_t size; // 图像文件大小
	int16_t reserved1;
	int16_t reserved2;
	int32_t offbits; // bmp图像数据偏移量
}__attribute__((packed));

struct bitmap_info
{
	int32_t size; // 本结构大小	
	int32_t width;
	int32_t height;
	int16_t planes; // 总为零

	int16_t bit_count; // 色深
	int32_t compression;
	int32_t size_img; // bmp数据大小，必须是4的整数倍
	int32_t X_pel;
	int32_t Y_pel;
	int32_t clrused;
	int32_t clrImportant;
}__attribute__((packed));

struct rgb_quad
{
	int8_t blue;
	int8_t green;
	int8_t red;
	int8_t reserved;
}__attribute__((packed));
//==============================bmp图像读取需要结构体=============================//

struct image_info
{
	int width;
	int height;
	int pixel_size;
};

typedef struct rgb_info
{
	unsigned char *rgb_buffer;
	struct image_info *imageinfo;
}rgb_info_name, *rgb_info_pointer;

rgb_info_pointer get_bmp_file_info(char *bmpfile);
float get_bmp_brightness(unsigned char *rgb_buffer, struct image_info *imageinfo);

rgb_info_pointer get_jpg_file_info(char *jpgfile);
float get_jpg_brightness(char *rgb_buffer, struct image_info *imageinfo);

#endif
