#include "jpeg.h"

static unsigned char *readjpg(char const *filename, unsigned long jpg_size)//用来申请内存空间，返回指针
{
	unsigned char *jpg_buffer = (unsigned char *)calloc(1, jpg_size);
	int fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		perror("打开图片失败");
		return NULL;
	}
	unsigned long nread = 0;
	while(jpg_size-=nread)
		nread = read(fd, jpg_buffer, jpg_size);
	close(fd);
	return jpg_buffer;
}

//解码jpeg图片，读取出rgb数据
int jpeg_decode(char *in_file_path, rgb_info_st *rgb_info_out)
{	
	//把jpg的信息读取出来
	struct stat info;
	bzero(&info, sizeof(info));

	if(stat(in_file_path, &info) == -1)
	{
		fprintf(stderr, "[%d]: stat failed: "
				"%s\n",__LINE__, strerror(errno));
		return -1;
	}
	unsigned char *jpg_buffer = readjpg(in_file_path, info.st_size);

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
		free(jpg_buffer);
		return -1;
	}

	jpeg_start_decompress(&cinfo);
	rgb_info_out->imageinfo.width	= cinfo.output_width;//读取宽度
	rgb_info_out->imageinfo.height	= cinfo.output_height;//读取高度
	rgb_info_out->imageinfo.pixel_size	= cinfo.output_components;//读取深度

	int row_stride = rgb_info_out->imageinfo.width * rgb_info_out->imageinfo.pixel_size;
	unsigned long rgb_len = rgb_info_out->imageinfo.width * rgb_info_out->imageinfo.height * rgb_info_out->imageinfo.pixel_size;
	//内存内部申请外部释放
	char *rgb_buffer = (unsigned char *)calloc(1, rgb_len);//申请一块内存来存放rgb数据
	while(cinfo.output_scanline < cinfo.output_height)//一行一行的循环读出
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
			(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}
	rgb_info_out->rgb_size = rgb_len;
	rgb_info_out->rgb_data = rgb_buffer;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);
	return 0;
}

//传入rgba数据，编码生成jpeg图片
int rgba_to_jpeg_encode(rgb_info_st *in_rgb_info, const char *file_path_out)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *outfile = fopen(file_path_out, "wb");
    if (!outfile) {
        printf("Error opening output file\n");
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = in_rgb_info->imageinfo.width;
    cinfo.image_height = in_rgb_info->imageinfo.height;
    cinfo.input_components = 3; // 3个通道（R、G、B）
    cinfo.in_color_space = JCS_RGB; // RGB色彩空间

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = cinfo.image_width * cinfo.input_components; // 一行的字节数

    unsigned char rgb_data[cinfo.image_width * cinfo.image_height * cinfo.input_components]; // 用于存储RGB数据

    // 将RGBA数据转换为RGB数据
    for (int i = 0; i < cinfo.image_width * cinfo.image_height; i++) {
        rgb_data[i * 3] = in_rgb_info->rgb_data[i * 4];     // R
        rgb_data[i * 3 + 1] = in_rgb_info->rgb_data[i * 4 + 1]; // G
        rgb_data[i * 3 + 2] = in_rgb_info->rgb_data[i * 4 + 2]; // B
    }

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &rgb_data[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    fclose(outfile);

    jpeg_destroy_compress(&cinfo);

    return 0;
}

//传入rgba数据，编码生成jpeg图片
int rgb_to_jpeg_encode(rgb_info_st *in_rgb_info, const char *file_path_out)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *outfile = fopen(file_path_out, "wb");
    if (!outfile) {
        printf("Error opening output file\n");
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = in_rgb_info->imageinfo.width;
    cinfo.image_height = in_rgb_info->imageinfo.height;
    cinfo.input_components = in_rgb_info->imageinfo.pixel_size; // 3个通道（R、G、B）
    cinfo.in_color_space = JCS_RGB; // RGB色彩空间

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = cinfo.image_width * cinfo.input_components; // 一行的字节数

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &in_rgb_info->rgb_data[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    fclose(outfile);

    jpeg_destroy_compress(&cinfo);

    return 0;
}

// 计算两个整数的最大公约数
int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

#define TARGET_WIDTH 512
#define TARGET_HEIGHT 288

//rgb缩放到512*288
int rgba_scaling(rgb_info_st *in_rgb_info, rgb_info_st *rgb_info_out)
{
    printf("in_width:[%u], in_height:[%u], in_pixel_size:[%u]\n", in_rgb_info->imageinfo.width, in_rgb_info->imageinfo.height, in_rgb_info->imageinfo.pixel_size);

    rgb_info_out->imageinfo.width = TARGET_WIDTH;
    rgb_info_out->imageinfo.height = TARGET_HEIGHT;
    rgb_info_out->imageinfo.pixel_size = in_rgb_info->imageinfo.pixel_size;

    if(in_rgb_info->imageinfo.width < TARGET_WIDTH) 
    {
        //小图像返回
        return -1;
    }

    //计算缩放裁剪系数
    float scale_factor_w = (float)rgb_info_out->imageinfo.width/TARGET_WIDTH;
    float scale_factor_h = (float)rgb_info_out->imageinfo.height/TARGET_HEIGHT;
    //采集偏移，表示每隔scale_factor个点采集一个rgb数据
    unsigned int scale_factor = 0;//(scale_factor_w%10)+1;
    //行偏移补充位
    unsigned int row_offset_post = rgb_info_out->imageinfo.width/scale_factor;
    printf("scale_factor:[%u], scale_factor_w:[%f], scale_factor_h:[%f]\n", scale_factor, scale_factor_w, scale_factor_h);

    //为每行图像数据分配新的缓冲区
    rgb_info_out->rgb_size = rgb_info_out->imageinfo.width * rgb_info_out->imageinfo.height * rgb_info_out->imageinfo.pixel_size;
    rgb_info_out->rgb_data = malloc(rgb_info_out->rgb_size);
    if (!rgb_info_out->rgb_data) {
        printf("malloc error len:%lu\n", rgb_info_out->rgb_size);
        return 1;
    }

    unsigned long out_rgb_offset = 0;
    for (int y = 0; y < in_rgb_info->imageinfo.height; y++)
    {
        
        for (int x = 0; x < in_rgb_info->imageinfo.width; x+scale_factor)
        {
            unsigned long in_rgb_offset = (in_rgb_info->imageinfo.width*y+x)*in_rgb_info->imageinfo.pixel_size;
            memcpy(&rgb_info_out->rgb_data[out_rgb_offset], &in_rgb_info->rgb_data[in_rgb_offset], in_rgb_info->imageinfo.pixel_size);
            out_rgb_offset+=in_rgb_info->imageinfo.pixel_size;
        }
        out_rgb_offset+=row_offset_post;
    }
}


int jpeg_resizeImage(const char *input_file, const char *output_file) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile = fopen(input_file, "rb");
    if (!infile) {
        printf("Error opening input file\n");
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int target_width = TARGET_WIDTH;
    int target_height = TARGET_HEIGHT;

    int output_width = cinfo.output_width;
    int output_height = cinfo.output_height;

    // 计算调整后的目标尺寸，保持图像等比例缩放
    if ((float)output_width / output_height > (float)TARGET_WIDTH / TARGET_HEIGHT) {
        target_height = output_height * TARGET_WIDTH / output_width;
    } else {
        target_width = output_width * TARGET_HEIGHT / output_height;
    }

    // 分配缓冲区内存
    JSAMPLE *image_buffer = (JSAMPLE *)malloc(target_width * target_height * cinfo.output_components);
    JSAMPROW row_pointer[1];

    // 读取原始图像数据
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &image_buffer[cinfo.output_scanline * target_width * cinfo.output_components];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);

    fclose(infile);
    jpeg_destroy_decompress(&cinfo);

    // 初始化 JPEG 压缩对象
    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr_out;

    FILE *outfile = fopen(output_file, "wb");
    if (!outfile) {
        printf("Error opening output file\n");
        return -1;
    }

    cinfo_out.err = jpeg_std_error(&jerr_out);
    jpeg_create_compress(&cinfo_out);

    jpeg_stdio_dest(&cinfo_out, outfile);
    cinfo_out.image_width = TARGET_WIDTH;
    cinfo_out.image_height = TARGET_HEIGHT;
    cinfo_out.input_components = cinfo.output_components;
    cinfo_out.in_color_space = cinfo.out_color_space;
    jpeg_set_defaults(&cinfo_out);
    jpeg_set_quality(&cinfo_out, 100, TRUE);

    jpeg_start_compress(&cinfo_out, TRUE);

    // 缩放图像并写入输出
    for (int i = 0; i < TARGET_HEIGHT; i++) {
        row_pointer[0] = &image_buffer[i * target_width * cinfo.output_components];
        jpeg_write_scanlines(&cinfo_out, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo_out);

    fclose(outfile);
    jpeg_destroy_compress(&cinfo_out);

    free(image_buffer);
    return 0;
}

int jpeg_cropAndScaleImage(const char *input_file, const char *output_file) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile = fopen(input_file, "rb");
    if (!infile) {
        printf("Error opening input file\n");
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // 计算目标图像适应16:9比例的宽度和高度
    int target_width, target_height;
    if ((float)cinfo.output_width / cinfo.output_height > 16.0 / 9.0) {
        target_width = cinfo.output_height * 16 / 9;
        target_height = cinfo.output_height;
    } else {
        target_width = cinfo.output_width;
        target_height = cinfo.output_width * 9 / 16;
    }

    // 分配缓冲区内存
    JSAMPLE *image_buffer = (JSAMPLE *)malloc(target_width * target_height * cinfo.output_components);
    JSAMPROW row_pointer[1];

    // 读取原始图像数据
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &image_buffer[cinfo.output_scanline * target_width * cinfo.output_components];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);

    // 初始化 JPEG 压缩对象
    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr_out;

    FILE *outfile = fopen(output_file, "wb");
    if (!outfile) {
        printf("Error opening output file\n");
        return -1;
    }

    cinfo_out.err = jpeg_std_error(&jerr_out);
    jpeg_create_compress(&cinfo_out);

    jpeg_stdio_dest(&cinfo_out, outfile);
    cinfo_out.image_width = TARGET_WIDTH;
    cinfo_out.image_height = TARGET_HEIGHT;
    cinfo_out.input_components = cinfo.output_components;
    cinfo_out.in_color_space = cinfo.out_color_space;
    jpeg_set_defaults(&cinfo_out);
    jpeg_set_quality(&cinfo_out, 100, TRUE);

    jpeg_start_compress(&cinfo_out, TRUE);

    // 缩放图像并写入输出
    for (int i = 0; i < TARGET_HEIGHT; i++) {
        row_pointer[0] = &image_buffer[i * target_width * cinfo.output_components];
        jpeg_write_scanlines(&cinfo_out, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo_out);

    fclose(outfile);

    jpeg_destroy_compress(&cinfo_out);
    jpeg_destroy_decompress(&cinfo);

    free(image_buffer);
    return 0;
}

//向右旋转90度
int RotateOfRight(rgb_info_st *in_rgb_info, rgb_info_st *rgb_info_out)
{
    printf("%s@%d in_width:[%u], in_height:[%u], in_pixel_size:[%u]\n", __FUNCTION__, __LINE__, 
            in_rgb_info->imageinfo.width, in_rgb_info->imageinfo.height, in_rgb_info->imageinfo.pixel_size);
    int i, j;
    rgb_info_out->imageinfo.width = in_rgb_info->imageinfo.height;
    rgb_info_out->imageinfo.height = in_rgb_info->imageinfo.width;
    rgb_info_out->imageinfo.pixel_size = in_rgb_info->imageinfo.pixel_size;

    //为输出图像数据分配新的缓冲区
    rgb_info_out->rgb_size = rgb_info_out->imageinfo.width * rgb_info_out->imageinfo.height * rgb_info_out->imageinfo.pixel_size;
    rgb_info_out->rgb_data = malloc(rgb_info_out->rgb_size);
    if (!rgb_info_out->rgb_data) {
        printf("malloc error len:%lu\n", rgb_info_out->rgb_size);
        return 1;
    }

    for (i = 0; i < rgb_info_out->imageinfo.height; i ++)
    {
        for (j = 0;j < rgb_info_out->imageinfo.width; j ++)      
        {
            unsigned long in_rgb_offset = ((in_rgb_info->imageinfo.height - j - 1)*in_rgb_info->imageinfo.width + i)*in_rgb_info->imageinfo.pixel_size;
            unsigned long out_rgb_offset = (i*rgb_info_out->imageinfo.width + j)*rgb_info_out->imageinfo.pixel_size;
            memcpy(rgb_info_out->rgb_data + out_rgb_offset, in_rgb_info->rgb_data + in_rgb_offset, rgb_info_out->imageinfo.pixel_size);
        }
    }
    return 0;
}

//unsigned char *pImgData: 输入图像指针
//int WidthIn, int HeightIn:输入图像宽、高
//unsigned char *pImgOut: 旋转后图像指针
//向左旋转90度
int RotateOfLeft(rgb_info_st *in_rgb_info, rgb_info_st *rgb_info_out)
{
    printf("%s@%d in_width:[%u], in_height:[%u], in_pixel_size:[%u]\n", __FUNCTION__, __LINE__, 
            in_rgb_info->imageinfo.width, in_rgb_info->imageinfo.height, in_rgb_info->imageinfo.pixel_size);
    int i, j;
    rgb_info_out->imageinfo.width = in_rgb_info->imageinfo.height;
    rgb_info_out->imageinfo.height = in_rgb_info->imageinfo.width;
    rgb_info_out->imageinfo.pixel_size = in_rgb_info->imageinfo.pixel_size;

    //为输出图像数据分配新的缓冲区
    rgb_info_out->rgb_size = rgb_info_out->imageinfo.width * rgb_info_out->imageinfo.height * rgb_info_out->imageinfo.pixel_size;
    rgb_info_out->rgb_data = malloc(rgb_info_out->rgb_size);
    if (!rgb_info_out->rgb_data) {
        printf("malloc error len:%lu\n", rgb_info_out->rgb_size);
        return 1;
    }

    for (i = 0; i < rgb_info_out->imageinfo.height; i ++)
    {
        for (j = 0;j < rgb_info_out->imageinfo.width; j ++)  
        {
            unsigned long in_rgb_offset = (in_rgb_info->imageinfo.width*(j+1) - i)*in_rgb_info->imageinfo.pixel_size;
            unsigned long out_rgb_offset = (i*rgb_info_out->imageinfo.width + j)*rgb_info_out->imageinfo.pixel_size;
            memcpy(rgb_info_out->rgb_data + out_rgb_offset, in_rgb_info->rgb_data + in_rgb_offset, rgb_info_out->imageinfo.pixel_size);
        }
    }
    return 0;
}

int rgb_fill_rgba(rgb_info_st *in_rgb_info, rgb_info_st *out_rgba_info, unsigned int out_width, unsigned int out_height)
{
    printf("%s@%d in_width:[%u], in_height:[%u], in_pixel_size:[%u]\n", __FUNCTION__, __LINE__, 
            in_rgb_info->imageinfo.width, in_rgb_info->imageinfo.height, in_rgb_info->imageinfo.pixel_size);

    out_rgba_info->imageinfo.width = out_width;
    out_rgba_info->imageinfo.height = out_height;
    out_rgba_info->imageinfo.pixel_size = 4;

    //为输出图像数据分配新的缓冲区
    out_rgba_info->rgb_size = out_rgba_info->imageinfo.width * out_rgba_info->imageinfo.height * out_rgba_info->imageinfo.pixel_size;
    out_rgba_info->rgb_data = malloc(out_rgba_info->rgb_size);
    if (!out_rgba_info->rgb_data) {
        printf("malloc error len:%lu\n", out_rgba_info->rgb_size);
        return 1;
    }

    unsigned int row_stride = in_rgb_info->imageinfo.width * in_rgb_info->imageinfo.pixel_size; // 一行的字节数
    unsigned int row_rgba_offset = (out_width-in_rgb_info->imageinfo.width) * out_rgba_info->imageinfo.pixel_size; // 一行的字节数

    printf("row_stride:[%u], row_rgba_offset:[%u]\n", row_stride, row_rgba_offset);

    char white_rgba[4] = {255,255,255,1};
    unsigned long out_rgba_offset = 0;
    for (int i = 0; i < in_rgb_info->imageinfo.height; i++)
    {
        for (int j = 0; j < in_rgb_info->imageinfo.width; j++)
        {
            unsigned long in_rgb_offset = (i*in_rgb_info->imageinfo.width+j)*in_rgb_info->imageinfo.pixel_size;
            out_rgba_info->rgb_data[out_rgba_offset] = in_rgb_info->rgb_data[in_rgb_offset];
            out_rgba_info->rgb_data[out_rgba_offset+1] = in_rgb_info->rgb_data[in_rgb_offset+1];
            out_rgba_info->rgb_data[out_rgba_offset+2] = in_rgb_info->rgb_data[in_rgb_offset+2];
            out_rgba_info->rgb_data[out_rgba_offset+3] = 1;
            out_rgba_offset += out_rgba_info->imageinfo.pixel_size;
        }
        //memcpy(&out_rgba_info->rgb_data[out_rgba_offset], &in_rgb_info->rgb_data[i*row_stride], row_stride);
        //out_rgba_offset+=row_stride;
        //for (int z = 0; z < )
        memset(&out_rgba_info->rgb_data[out_rgba_offset], 255, row_rgba_offset);
        out_rgba_offset+=row_rgba_offset;
    }
    unsigned long height_fill_size = out_width*out_rgba_info->imageinfo.pixel_size*(out_height-in_rgb_info->imageinfo.height);
    printf("height_fill_size:[%lu]\n", height_fill_size);
    memset(&out_rgba_info->rgb_data[out_rgba_offset], 255, height_fill_size);
    out_rgba_offset+=height_fill_size;
    printf("out_rgba_offset:[%lu]\n", out_rgba_offset);

    return 0;
}

int get_jpeg_info(char *in_file_path, unsigned int *out_width, unsigned int *out_height)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile = fopen(in_file_path, "rb");
    if (!infile) {
        printf("Error opening input file\n");
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    *out_width = cinfo.image_width;
    *out_height = cinfo.image_height;

    printf("JPEG image width: %d\n", cinfo.image_width);
    printf("JPEG image height: %d\n", cinfo.image_height);

    fclose(infile);
    jpeg_destroy_decompress(&cinfo);
    return 0;
}

//把w1*h1缩小到w2*h2以下，计算缩放的次数和倍数
//libjpeg库，一次缩放调用只支持：放大2倍，缩小2倍，缩小3倍，缩小4倍，这几种缩放操作。
int calc_scale_times(int w1, int h1, int w2, int h2, int times[2])
{
    int x[] = {2, 1, 1, 1, 1, 1, 1, 1,  1};
    int y[] = {3, 2, 3, 4, 6, 8, 9, 12, 16};
    int time[sizeof(x)/sizeof(int)][2] = {
                     {-3, 2},
                     {-2},
                     {-3},
                     {-4},
                     {-2, -3},
                     {-2, -4},
                     {-3, -3},
                     {-3, -4},
                     {-4, -4},
                   };
    int index = -1;
    for (int i=0; i<sizeof(x)/sizeof(float); i++)
    {
        int ww = (int)((float)w2 * (float)y[i] / (float)x[i]);
        int hh = (int)((float)h2 * (float)y[i] / (float)x[i]);
        if (ww >= w1 && hh >= h1) {
            printf("%s@%d, %d*%d scale by %d/%d to %d*%d(%d*%d)\n", __FUNCTION__, __LINE__,
                           w1, h1, x[i], y[i], w1*x[i]/y[i], h1*x[i]/y[i], w2, h2);
            index = i;
            break;
        }
    }
    if (index != -1) {
        memset(times, 0, 2*sizeof(int));
        for (int i=0; i<sizeof(time[index])/sizeof(int); i++) {
            if (time[index][i])
                times[i] = time[index][i];
        }
        printf("%s@%d,[%d %d]\n", __FUNCTION__, __LINE__, times[0], times[1]);
        return 0;
    }
    return -1;
}

/**
 * @brief 把图像缩放到指定的分辨率，如果不能恰好压缩到指定分辨率，则进行粗略压缩
 * @param in_file_path 输入文件
 * @param file_path_out 输出文件
 * @param out_width 目标缩放宽
 * @param out_height 目标缩放高
 * @return int 0：成功
 */
int jpeg_isometric_scaling(char *in_file_path, char *file_path_out, unsigned int out_width, unsigned int out_height)
{
    printf("in_file_path:[%s], file_path_out:[%s], out_width:[%u], out_height:[%u]\n", in_file_path, file_path_out, out_width, out_height);
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *input_file; // 用于读取 JPEG 图像的文件指针
    JSAMPARRAY buffer; // 用于存放解码后的图像数据

    unsigned int in_width = 0;
    unsigned int in_height = 0;
    get_jpeg_info(in_file_path, &in_width, &in_height);
    printf("in_width:[%u], in_height:[%u]\n", in_width, in_height);

    if (in_width <= out_width && in_height <= out_height) {
        char cmd[128] = {0};
        snprintf(cmd, sizeof(cmd)-1, "cp -rf %s %s", in_file_path, file_path_out);
        system(cmd);
        printf("%s,%d no need scale\n", __FUNCTION__, __LINE__);
        return 0;
    }

    int times[2] = {0};
    int ret = calc_scale_times(in_width, in_height, out_width, out_height, times);
    if (ret != 0) {
        printf("%s,%d cannot scale\n", __FUNCTION__, __LINE__);
        return -1;
    }

    char * file_in = in_file_path;
    #define file_tmp "tmp_scale_tmp.jpg"
    #define file_out "tmp_scale_out.jpg"
    system("rm -rf "file_tmp);
    system("rm -rf "file_out);

    for (int i=0; i<sizeof(times)/sizeof(int); i++)
    {
        if (times[i] == 0)
            break;

        if (access(file_out, F_OK) == 0) {
            char cmd[128] = {0};
            snprintf(cmd, sizeof(cmd)-1, "mv %s %s", file_out, file_tmp);
            system(cmd);
            file_in = file_tmp;
        }

        printf("%s@%d i:%d %d\n", __FUNCTION__, __LINE__, i, times[i]);
    
        // 设置错误处理
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        // 打开输入的 JPEG 图像文件
        input_file = fopen(file_in, "rb");
        if (input_file == NULL) {
            fprintf(stderr, "Error: cannot open image file.\n");
            return 1;
        }

        // 设置数据源为文件
        jpeg_stdio_src(&cinfo, input_file);

        // 读取 JPEG 文件的头并开始解码
        jpeg_read_header(&cinfo, TRUE);

        // 设置等比例缩放参数，宽高各减半
        if (times[i] > 0) {
            cinfo.scale_num = times[i];    // 缩放分子
            cinfo.scale_denom = 1;  // 缩放分母
        } else {
            cinfo.scale_num = 1;    // 缩放分子
            cinfo.scale_denom = -times[i];  // 缩放分母
        }
        printf("scale_num:[%u], scale_denom:[%u]\n", cinfo.scale_num, cinfo.scale_denom);

        // 重新计算输出宽度和高度
        jpeg_calc_output_dimensions(&cinfo);
        printf("output_width:[%u], output_height:[%u]\n", cinfo.output_width, cinfo.output_height);

        // 开始解码
        jpeg_start_decompress(&cinfo);

        // 分配内存以存储解码的行
        buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE,
                                           cinfo.output_width * cinfo.output_components, 1);

        // 创建输出文件
        FILE *output_file = fopen(file_out, "wb");
        if (output_file == NULL) {
            fprintf(stderr, "Error: cannot open file for writing.\n");
            jpeg_destroy_decompress(&cinfo);
            fclose(input_file);
            return 1;
        }

        // 编码并写入输出文件
        struct jpeg_compress_struct ccompress;
        struct jpeg_error_mgr jcerr;
        ccompress.err = jpeg_std_error(&jcerr);
        jpeg_create_compress(&ccompress);

        jpeg_stdio_dest(&ccompress, output_file);
        ccompress.image_width = cinfo.output_width;
        ccompress.image_height = cinfo.output_height;
        ccompress.input_components = cinfo.output_components;
        ccompress.in_color_space = cinfo.output_components == 3 ? JCS_RGB : JCS_GRAYSCALE;

        jpeg_set_defaults(&ccompress);
        jpeg_set_quality(&ccompress, 100, TRUE);
        jpeg_start_compress(&ccompress, TRUE);

        while (cinfo.output_scanline < cinfo.output_height) {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            jpeg_write_scanlines(&ccompress, buffer, 1);
        }

        // 结束压缩和解压
        jpeg_finish_compress(&ccompress);
        jpeg_finish_decompress(&cinfo);

        // 清理工作
        jpeg_destroy_compress(&ccompress);
        jpeg_destroy_decompress(&cinfo);
        fclose(input_file);
        fclose(output_file);

    }

    if (access(file_out, F_OK) == 0) {
        char cmd[128] = {0};
        snprintf(cmd, sizeof(cmd)-1, "mv %s %s", file_out, file_path_out);
        system(cmd);
        return 0;
    }
    return -1;
}

//支持的缩放比例
typedef enum enum_zj_os_type
{
    EN_JPEG_SCALING_1_1 = 1,
    EN_JPEG_SCALING_1_2 = 2,
    EN_JPEG_SCALING_1_3 = 3,//jpeg不能准确压缩1/3，实际压缩1/2.66
    EN_JPEG_SCALING_1_4 = 4,
    EN_JPEG_SCALING_1_8 = 8
}EN_JPEG_SCALING;

/**
 * @brief 把图像缩放到指定的比例
 * @param in_file_path 输入文件
 * @param file_path_out 输出文件
 * @param scaling 缩放比例枚举EN_JPEG_SCALING
    1：不缩放，但会重新调整输入文件的大小
    2：缩放1/2
    3：缩放1/2.66，jpeg不能准确压缩1/3，实际压缩1/2.66
    4：缩放1/4
    8：缩放1/8
 * @return int 0：成功
 */
int jpeg_proportiona_scaling(char *in_file_path, char *file_path_out, unsigned int scaling)
{
    printf("in_file_path:[%s], file_path_out:[%s], scaling:[%u]\n", in_file_path, file_path_out, scaling);
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *input_file; // 用于读取 JPEG 图像的文件指针

    unsigned int in_width = 0;
    unsigned int in_height = 0;
    get_jpeg_info(in_file_path, &in_width, &in_height);
    printf("in_width:[%u], in_height:[%u]\n", in_width, in_height);
    
    // 设置错误处理
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // 打开输入的 JPEG 图像文件
    input_file = fopen(in_file_path, "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Error: cannot open image file.\n");
        return 1;
    }

    // 设置数据源为文件
    jpeg_stdio_src(&cinfo, input_file);

    // 读取 JPEG 文件的头并开始解码
    jpeg_read_header(&cinfo, TRUE);


    // 设置等比例缩放参数
    switch (scaling)
    {
    case EN_JPEG_SCALING_1_1:
        cinfo.scale_num = 1;    // 缩放分子
        cinfo.scale_denom = 1;  // 缩放分母
        break;
    case EN_JPEG_SCALING_1_2:
        cinfo.scale_num = 1;    // 缩放分子
        cinfo.scale_denom = 2;  // 缩放分母
        break;
    case EN_JPEG_SCALING_1_3:
        cinfo.scale_num = 1;    // 缩放分子
        cinfo.scale_denom = 3;  // 缩放分母
        break;
    case EN_JPEG_SCALING_1_4:
        cinfo.scale_num = 1;    // 缩放分子
        cinfo.scale_denom = 4;  // 缩放分母
        break;
    case EN_JPEG_SCALING_1_8:
        cinfo.scale_num = 1;    // 缩放分子
        cinfo.scale_denom = 8;  // 缩放分母
        break;
    default:
        printf("scaling not support! scaling:[%u]\n", scaling);
        return 1;
        break;
    }
    printf("scale_num:[%u], scale_denom:[%u]\n", cinfo.scale_num, cinfo.scale_denom);

    // 重新计算输出宽度和高度
    jpeg_calc_output_dimensions(&cinfo);
    printf("output_width:[%u], output_height:[%u]\n", cinfo.output_width, cinfo.output_height);

    // 开始解码
    jpeg_start_decompress(&cinfo);

    // 分配内存以存储解码的行
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE,
                                           cinfo.output_width * cinfo.output_components, 1);

    // 创建输出文件
    FILE *output_file = fopen(file_path_out, "wb");
    if (output_file == NULL) {
        fprintf(stderr, "Error: cannot open file for writing.\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(input_file);
        return 1;
    }

    // 编码并写入输出文件
    struct jpeg_compress_struct ccompress;
    struct jpeg_error_mgr jcerr;
    ccompress.err = jpeg_std_error(&jcerr);
    jpeg_create_compress(&ccompress);

    jpeg_stdio_dest(&ccompress, output_file);
    ccompress.image_width = cinfo.output_width;
    ccompress.image_height = cinfo.output_height;
    ccompress.input_components = cinfo.output_components;
    ccompress.in_color_space = cinfo.output_components == 3 ? JCS_RGB : JCS_GRAYSCALE;

    jpeg_set_defaults(&ccompress);
    jpeg_set_quality(&ccompress, 100, TRUE);
    jpeg_start_compress(&ccompress, TRUE);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        jpeg_write_scanlines(&ccompress, buffer, 1);
    }

    // 结束压缩和解压
    jpeg_finish_compress(&ccompress);
    jpeg_finish_decompress(&cinfo);

    // 清理工作
    jpeg_destroy_compress(&ccompress);
    jpeg_destroy_decompress(&cinfo);
    fclose(input_file);
    fclose(output_file);

    return 0;
}

// 等比例缩放jpeg图像
int jpeg_isometric_scaling_1(char *in_file_path, char *file_path_out, unsigned int out_width, unsigned int out_height)
{
    printf("in_file_path:[%s], file_path_out:[%s], out_width:[%u], out_height:[%u]\n", in_file_path, file_path_out, out_width, out_height);
    // 打开输入文件
    FILE *infile = fopen(in_file_path, "rb");
    if (!infile) {
        printf("Error opening input file\n");
        return 1;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // 设置输入源为文件
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // 计算缩放因子，判断是否可以等比例缩放到指定分辨率
    float scale_factor_w = (float)out_width/(float)cinfo.output_width;
    float scale_factor_h = (float)out_height/(float)cinfo.output_height;
    printf("scale_factor_w:[%f], scale_factor_h:[%f]\n", scale_factor_w, scale_factor_h);

    //if (scale_factor_w != scale_factor_h) {
        //printf("Error scale factor not same!\n");
        //return 1;
    //}

    // 打开输出文件
    FILE *outfile = fopen(file_path_out, "wb");
    if (!outfile) {
        printf("Error opening output file\n");
        return 1;
    }

    // 初始化压缩结构体
    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr_out;

    cinfo_out.err = jpeg_std_error(&jerr_out);
    jpeg_create_compress(&cinfo_out);

    // 设置输出目标为文件
    jpeg_stdio_dest(&cinfo_out, outfile);
    cinfo_out.image_width = out_width;
    cinfo_out.image_height = out_height;
    cinfo_out.input_components = cinfo.output_components;
    cinfo_out.in_color_space = cinfo.out_color_space;
    jpeg_set_defaults(&cinfo_out);
    jpeg_set_quality(&cinfo_out, 100, TRUE);
    jpeg_start_compress(&cinfo_out, TRUE);

    // 为每行图像数据分配新的缓冲区
    unsigned int row_stride = cinfo.output_width * cinfo.output_components; // 一行的字节数
    printf("row_stride:[%u]\n", row_stride);
    JSAMPLE *row_buffer = (JSAMPLE *)malloc(row_stride);
    if (!row_buffer) {
        printf("malloc error len:%u\n", row_stride);
        return 1;
    }
    printf("jpeg_scanlines start.\n");
    JSAMPROW row_pointer[1];
    // 逐行读取输入文件，缩放后写入输出文件
    while (cinfo.output_scanline < cinfo.output_height) {
        printf("output_scanline:[%u], output_height:[%u]\n", cinfo.output_scanline, cinfo.output_height);
        jpeg_read_scanlines(&cinfo, &row_buffer, 1);
        row_pointer[0] = row_buffer;
        jpeg_write_scanlines(&cinfo_out, row_pointer, 1);
    }
    printf("jpeg_scanlines end.\n");
    // 释放每行图像数据的缓冲区内存
    free(row_buffer);
    jpeg_finish_compress(&cinfo_out);
    fclose(outfile);
    fclose(infile);
    jpeg_destroy_compress(&cinfo_out);
    jpeg_destroy_decompress(&cinfo);
    return 0;
}

//裁剪指定区域
int jpeg_cutting_rect(char *in_file_path, char *file_path_out, jpeg_rect_st rect_info)
{
    struct jpeg_decompress_struct cinfo_in;
    struct jpeg_error_mgr jerr;

    printf("%s@%d top_left:[%d,%d], width:[%d], height:[%d]\n", __FUNCTION__, __LINE__, 
            rect_info.top_left_x, rect_info.top_left_y, rect_info.width, rect_info.height);

    FILE *infile = fopen(in_file_path, "rb");
    if (!infile) {
        printf("Error opening input file\n");
        return 1;
    }

    cinfo_in.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo_in);

    jpeg_stdio_src(&cinfo_in, infile);
    jpeg_read_header(&cinfo_in, TRUE);
    jpeg_start_decompress(&cinfo_in);
    printf("image_width:[%d], image_height:[%d], output_components:[%d]\n", cinfo_in.image_width, cinfo_in.image_height, cinfo_in.output_components);

    FILE *outfile = fopen(file_path_out, "wb");
    if (!outfile) {
        printf("Error opening output file\n");
        return 1;
    }

    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr_out;

    cinfo_out.err = jpeg_std_error(&jerr_out);
    jpeg_create_compress(&cinfo_out);

    jpeg_stdio_dest(&cinfo_out, outfile);
    cinfo_out.image_width = rect_info.width;
    cinfo_out.image_height = rect_info.height;
    cinfo_out.input_components = cinfo_in.output_components;
    cinfo_out.in_color_space = cinfo_in.out_color_space;
    jpeg_set_defaults(&cinfo_out);
    jpeg_set_quality(&cinfo_out, 100, TRUE);
    
    // 为每行图像数据分配新的缓冲区
    unsigned int in_row_stride = cinfo_in.output_width * cinfo_in.output_components; // 一行的字节数
    JSAMPLE *in_row_buffer = (JSAMPLE *)malloc(in_row_stride);
    if (!in_row_buffer) {
        printf("malloc error len:%u\n", in_row_stride);
        return 1;
    }
   
    unsigned int out_row_stride = cinfo_out.image_width * cinfo_out.input_components;
    JSAMPLE *out_row_buffer = (JSAMPLE *)malloc(out_row_stride);
    if (!out_row_buffer) {
        printf("malloc error len:%u\n", out_row_stride);
        return 1;
    }

    unsigned int row_offset_post = rect_info.top_left_x*cinfo_out.input_components;
    unsigned int line_offset_post = rect_info.top_left_y+rect_info.height;
    printf("in_row_stride:[%u], out_row_stride:[%u], row_offset_post:[%u], line_offset_post:[%u]\n", 
        in_row_stride, out_row_stride, row_offset_post, line_offset_post);

    //开始生成新图像
    jpeg_start_compress(&cinfo_out, TRUE);
    // 逐行读取输入文件，缩放后写入输出文件
    while (cinfo_in.output_scanline < cinfo_in.output_height && cinfo_in.output_scanline < line_offset_post) {
        jpeg_read_scanlines(&cinfo_in, &in_row_buffer, 1);
        if(cinfo_in.output_scanline <= rect_info.top_left_y) {
            //printf("output_scanline:[%u]\n", cinfo_in.output_scanline);
            continue;
        }
        memcpy(out_row_buffer, in_row_buffer+row_offset_post, out_row_stride);
        jpeg_write_scanlines(&cinfo_out, &out_row_buffer, 1);
    }
    jpeg_finish_compress(&cinfo_out);
    free(in_row_buffer);
    free(out_row_buffer);

    fclose(outfile);
    fclose(infile);

    jpeg_destroy_compress(&cinfo_out);
    jpeg_destroy_decompress(&cinfo_in);
    return 0;
}
#if 0
// 函数：旋转图像
void rotate_image(struct jpeg_decompress_struct *cinfo) {
    int i, j;
    JSAMPLE *buffer;
    buffer = (JSAMPLE *)malloc(cinfo->image_width * cinfo->num_components * sizeof(JSAMPLE));
    
    for (i = 0; i < cinfo->image_width; i++) {
        for (j = 0; j < cinfo->image_height; j++) {
            int source_index = j * cinfo->image_width + i;
            int dest_index = ((cinfo->image_height - j - 1) * cinfo->image_width + i) * cinfo->num_components;
            for (int k = 0; k < cinfo->num_components; k++) {
                buffer[dest_index + k] = cinfo->image_buffer[source_index * cinfo->num_components + k];
            }
        }
    }

    free(cinfo->image_buffer);
    cinfo->image_buffer = buffer;
    
    int temp = cinfo->image_width;
    cinfo->image_width = cinfo->image_height;
    cinfo->image_height = temp;
}

int jpeg_rotate_image(char *in_file_path, char *file_path_out)
{
    printf("in_file_path:[%s], file_path_out:[%s]\n", in_file_path, file_path_out);

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *input_file;
    FILE *output_file;
    
    if ((input_file = fopen(in_file_path, "rb")) == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    if ((output_file = fopen(file_path_out, "wb")) == NULL) {
        printf("Error opening output file\n");
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, input_file);
    jpeg_read_header(&cinfo, TRUE);
    
    rotate_image(&cinfo);
    
    struct jpeg_compress_struct cinfo_dest;
    jpeg_create_compress(&cinfo_dest);
    jpeg_stdio_dest(&cinfo_dest, output_file);

    cinfo_dest.image_width = cinfo.image_width;
    cinfo_dest.image_height = cinfo.image_height;
    cinfo_dest.input_components = cinfo.num_components;
    cinfo_dest.in_color_space = cinfo.out_color_space;
    jpeg_copy_critical_parameters((j_decompress_ptr)&cinfo, (j_compress_ptr)&cinfo_dest);

    jpeg_start_compress(&cinfo_dest, TRUE);
    
    JSAMPROW row_pointer[1];
    while (cinfo_dest.next_scanline < cinfo_dest.image_height) {
        row_pointer[0] = &cinfo.image_buffer[cinfo_dest.next_scanline * cinfo_dest.image_width * cinfo_dest.input_components];
        jpeg_write_scanlines(&cinfo_dest, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo_dest);
    jpeg_destroy_compress(&cinfo_dest);
    jpeg_destroy_decompress(&cinfo);

    fclose(input_file);
    fclose(output_file);
    
    return 0;
}

#else

// 函数：旋转图像
void rotate_image(JSAMPLE *image_data, int width, int height, int num_components) {
    JSAMPLE *buffer = (JSAMPLE *)malloc(width * height * num_components * sizeof(JSAMPLE));
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int source_index = j * width + i;
            int dest_index = ((height - j - 1) * width + i) * num_components;
            for (int k = 0; k < num_components; k++) {
                buffer[dest_index + k] = image_data[source_index * num_components + k];
            }
        }
    }

    // 将旋转后的图像数据拷贝回原始数据
    for (int i = 0; i < width * height * num_components; i++) {
        image_data[i] = buffer[i];
    }

    free(buffer);
}

int jpeg_rotate_image(char *in_file_path, char *file_path_out)
{
    printf("in_file_path:[%s], file_path_out:[%s]\n", in_file_path, file_path_out);

    FILE *input_file;
    FILE *output_file;
    
    if ((input_file = fopen(in_file_path, "rb")) == NULL) {
        printf("Error opening input file\n");
        return 1;
    }

    if ((output_file = fopen(file_path_out, "wb")) == NULL) {
        printf("Error opening output file\n");
        return 1;
    }

    // 初始化JPEG解压缩对象
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, input_file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    
    // 读取图像数据
    int row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
    
    JSAMPLE *image_data = (JSAMPLE *)malloc(row_stride * cinfo.output_height * sizeof(JSAMPLE));
    
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        for (int i = 0; i < row_stride; i++) {
            image_data[(cinfo.output_scanline - 1) * row_stride + i] = buffer[0][i];
        }
    }

    // 旋转图像
    rotate_image(image_data, cinfo.output_width, cinfo.output_height, cinfo.output_components);
    
    // 初始化JPEG压缩对象
    struct jpeg_compress_struct cinfo_dest;
    struct jpeg_error_mgr jerr_dest;
    
    cinfo_dest.err = jpeg_std_error(&jerr_dest);
    jpeg_create_compress(&cinfo_dest);
    jpeg_stdio_dest(&cinfo_dest, output_file);
    
    // 设置压缩对象参数
    cinfo_dest.image_width = cinfo.output_height;
    cinfo_dest.image_height = cinfo.output_width;
    cinfo_dest.input_components = cinfo.output_components;
    cinfo_dest.in_color_space = cinfo.out_color_space;
    
    jpeg_set_defaults(&cinfo_dest);
    cinfo_dest.optimize_coding = TRUE;
    jpeg_set_quality(&cinfo_dest, 100, TRUE);
    
    jpeg_start_compress(&cinfo_dest, TRUE);
    
    // 将旋转后的图像数据写入输出文件
    JSAMPROW row_pointer[1];
    while (cinfo_dest.next_scanline < cinfo_dest.image_height) {
        row_pointer[0] = &image_data[cinfo_dest.next_scanline * cinfo_dest.image_width * cinfo_dest.input_components];
        jpeg_write_scanlines(&cinfo_dest, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo_dest);
    
    // 释放资源
    jpeg_destroy_compress(&cinfo_dest);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(image_data);

    fclose(input_file);
    fclose(output_file);
    
    return 0;
}



#endif




