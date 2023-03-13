#include "getBrightness.h"

static unsigned char *readbmp(char const *bmpfile, struct image_info *imageinfo)//读取rgb数据
{
	int fd = open(bmpfile, O_RDONLY);
	if(fd == -1)
	{
		fprintf(stderr, "opening \"%s\" failed: %s\n",
				bmpfile, strerror(errno));
		exit(0);
	}

	struct stat fileinfo;
	fstat(fd, &fileinfo);
	unsigned char *rgb_buffer = calloc(1, fileinfo.st_size);

	struct bitmap_header header;//读取bmp文件的头部参数
	struct bitmap_info info;
	struct rgb_quad quad;
	read(fd, &header, sizeof(header));
	read(fd, &info, sizeof(info));
	if(info.compression != 0)
	{
		read(fd, &quad, sizeof(quad));
		fprintf(stderr, "read quad!\n");
	}
	read(fd, rgb_buffer, fileinfo.st_size);//读出数据
	
	imageinfo->width = info.width;
	imageinfo->height = info.height;
	imageinfo->pixel_size = info.bit_count/8;//获取长宽高

	close(fd);
	return rgb_buffer;
}

rgb_info_pointer get_bmp_file_info(char *bmpfile)
{
	struct image_info *imageinfo = calloc(1, sizeof(struct image_info));
	unsigned char *rgb_buffer = readbmp(bmpfile, imageinfo);
	
	rgb_info_pointer rgbinfo = calloc(1, sizeof(rgb_info_name));
	rgbinfo->rgb_buffer = rgb_buffer;
	rgbinfo->imageinfo = imageinfo;

	return rgbinfo;
}

//计算图像的平均亮度
float get_bmp_brightness(unsigned char *rgb_buffer, struct image_info *imageinfo)
{
	//公式：Y＝R * 0.299 + G * 0.587 + B * 0.114
	float sum = 0;

	//bmp图片的行偏移量
	unsigned int pad = ((4-(imageinfo->width*imageinfo->pixel_size)%4))%4;
	rgb_buffer += (imageinfo->width * imageinfo->pixel_size + pad) * (imageinfo->height-1);
	
	printf("img_w:%d\n",imageinfo->width);
	printf("img_h:%d\n",imageinfo->height);
	printf("img_p:%d\n",imageinfo->pixel_size);
	for(int y=0; y<imageinfo->height; y++)
	{
		for(int x=0; x<imageinfo->width; x++)
		{
			unsigned char blue  = *(rgb_buffer);
			unsigned char green = *(rgb_buffer+1);
			unsigned char red   = *(rgb_buffer+2);
			sum = sum + (blue*0.114 + green*0.587 + red*0.299);
			rgb_buffer += imageinfo->pixel_size;
		}
		rgb_buffer += pad;
		rgb_buffer -= (imageinfo->width*imageinfo->pixel_size + pad)*2;
	}
	printf("sum:[%f]\n", sum);
	return sum / (imageinfo->width*imageinfo->height);
}
