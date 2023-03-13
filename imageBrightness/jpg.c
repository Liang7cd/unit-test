#include "getBrightness.h"
#include "jpeglib.h"

static unsigned char *readjpg(char const *filename, unsigned long jpg_size)//用来申请内存空间，返回指针
{
	unsigned char *jpg_buffer = (unsigned char *)calloc(1, jpg_size);
	int fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		perror("打开图片失败");
		exit(0);
	}
	unsigned long nread = 0;
	while(jpg_size-=nread)
		nread = read(fd, jpg_buffer, jpg_size);
	close(fd);
	return jpg_buffer;
}

rgb_info_pointer get_jpg_file_info(char *jpgfile)
{	
	//把jpg的信息读取出来
	struct stat info;
	bzero(&info, sizeof(info));

	if(stat(jpgfile, &info) == -1)
	{
		fprintf(stderr, "[%d]: stat failed: "
				"%s\n",__LINE__, strerror(errno));
		exit(1);
	}
	unsigned char *jpg_buffer = readjpg(jpgfile, info.st_size);


	//开始解码
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpg_buffer, info.st_size);//把jpg_buffer的数据读出

	int ret = jpeg_read_header(&cinfo, true);//判断文件头是否合法
	if(ret != 1)
	{
		fprintf(stderr, "[%d]: jpeg_read_header failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}

	jpeg_start_decompress(&cinfo);
	struct image_info imageinfo;
	imageinfo.width		= cinfo.output_width;//读取宽度
	imageinfo.height	= cinfo.output_height;//读取高度
	imageinfo.pixel_size	= cinfo.output_components;//读取深度

	int row_stride = imageinfo.width * imageinfo.pixel_size;

	unsigned long rgb_size;
	unsigned char *rgb_buffer;
	rgb_size   = imageinfo.width * imageinfo.height * imageinfo.pixel_size;
	rgb_buffer = (unsigned char *)calloc(1, rgb_size);//申请一块内存来存放rgb数据
	while(cinfo.output_scanline < cinfo.output_height)//一行一行的循环读出
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
			(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);
	
	
	rgb_info_pointer rgbinfo = calloc(1, sizeof(rgb_info_name));
	rgbinfo->rgb_buffer = rgb_buffer;
	rgbinfo->imageinfo = &imageinfo;
	return rgbinfo;
}

float get_jpg_brightness(char *rgb_buffer, struct image_info *imageinfo)
{
	//公式：Y＝R * 0.299 + G * 0.587 + B * 0.114
	float sum = 0;
	
	printf("img_w:%d\n",imageinfo->width);
	printf("img_h:%d\n",imageinfo->height);
	printf("img_p:%d\n",imageinfo->pixel_size);
	for(int y=0; y<imageinfo->height; y++)
	{
		for(int x=0; x<imageinfo->width; x++)
		{
			unsigned long rgb_offset = (imageinfo->width*y+x)*imageinfo->pixel_size;

			unsigned char red   = *(rgb_buffer+ rgb_offset+0);
			unsigned char green = *(rgb_buffer+ rgb_offset+1);
			unsigned char blue  = *(rgb_buffer+ rgb_offset+2);
			sum = sum + (blue*0.114 + green*0.587 + red*0.299);
		}
	}
	printf("sum:[%f]\n", sum);
	return sum / (imageinfo->width*imageinfo->height);
}
