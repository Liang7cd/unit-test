#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "jpeg.h"

#define SNAPSHOT_JPEG_PATH    "img_snapshot.jpg"
static char *rgb24Tojpeg(char *rgb_data, unsigned int rgb_size, unsigned int *jpeg_len)
{
    //通过rgb数据生成jpg图片文件
    rgb_info_st input_rgb_info = {0};
    input_rgb_info.rgb_data = rgb_data;
    input_rgb_info.rgb_size = rgb_size;
    input_rgb_info.imageinfo.width = 512;
    input_rgb_info.imageinfo.height = 288;
    input_rgb_info.imageinfo.pixel_size = 4;
    rgb_info_st rotate_rgb_info = {0};
    //rgb数据转换为rgba
    int ret = RotateOfLeft(&input_rgb_info, &rotate_rgb_info);
    if(ret != 0) {
        printf("RotateOfLeft failed! ret:[%d]", ret);
        return NULL;
    }
    //rgba转换成jpg
    rgba_to_jpeg_encode(&rotate_rgb_info, SNAPSHOT_JPEG_PATH);
    free(rotate_rgb_info.rgb_data);

    //读取抠图的jpg文件
    FILE *jpeg_file_p = fopen(SNAPSHOT_JPEG_PATH, "r");
    fseek(jpeg_file_p, 0, SEEK_END);
    unsigned int jpeg_file_size = ftell(jpeg_file_p);
    fclose(jpeg_file_p);
    if (jpeg_file_size <= 0) {
        return NULL;
    }
    printf("jpeg size:[%u]", jpeg_file_size);

    //申请内存
    char *jpeg_buf = malloc(jpeg_file_size);
    if (!jpeg_buf) {
        printf("malloc error len:%d",jpeg_file_size);
        return NULL;
    }
    memset(jpeg_buf, 0, jpeg_file_size);

    //读取图片到内存
    jpeg_file_p = fopen(SNAPSHOT_JPEG_PATH, "r");
    unsigned int jpeg_read_len = fread(jpeg_buf, 1, jpeg_file_size, jpeg_file_p);
    if(jpeg_read_len != jpeg_file_size) {
        printf("fread error len:[%u]",jpeg_read_len);
        free(jpeg_buf);
        return NULL;
    }
    *jpeg_len = jpeg_file_size;
    fclose(jpeg_file_p);
    remove(SNAPSHOT_JPEG_PATH);
    return jpeg_buf;
}

#define SCALING_IMG_PATH    "scaling_img.jpg"
#define CUTTING_IMG_PATH    "cutting_img.jpg"
#define ROTATE_IMG_PATH    "rotate_img.jpg"
static int img_scaling_rotate_fill(char *img_file_name, rgb_info_st *out_rgba_info, unsigned int target_width, unsigned int target_height)
{
    //检测接收到的文件是否正常
    if (access(img_file_name, F_OK) == -1) {
        printf("File does not exist! name:[%s]\n", img_file_name);
        return -1;
    }

    FILE *file = fopen(img_file_name, "rb");
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    fclose(file);
    if (file_size <= 0) {
        return -2;
    }
    //正常则进行人脸注册
    printf("file_size:[%d], target_width:[%u], target_height:[%u]\n", file_size, target_width, target_height);

    //对图片进行等比例缩放（未旋转之前宽高相反）
    int ret = jpeg_isometric_scaling(img_file_name, SCALING_IMG_PATH, target_height, target_width);
    if(ret != 0) {
        printf(" failed! ret:[%d]\n", ret);
        return -3;
    }

    rgb_info_st input_rgb_info = {0};
    //把处理完成的图片rgba数据读取出来
    ret = jpeg_decode(SCALING_IMG_PATH, &input_rgb_info);
    if (ret != 0) {
        printf("jpeg_decode failed! ret:[%d]\n", ret);
        return -4;
    }

    //检查是否需要裁剪（未旋转之前宽高相反）
    if(input_rgb_info.imageinfo.width > target_height || input_rgb_info.imageinfo.height > target_width)
    {
        printf("resolution not support! width:[%u], height:[%u]\n", input_rgb_info.imageinfo.width, input_rgb_info.imageinfo.height);
        jpeg_rect_st rect_info = {0};
        if(input_rgb_info.imageinfo.width > target_height)
        {
            rect_info.top_left_x = (input_rgb_info.imageinfo.width-target_height)/2;
            rect_info.width = target_height;
        }else{
            rect_info.top_left_x = 0;
            rect_info.width = input_rgb_info.imageinfo.width;
        }

        if(input_rgb_info.imageinfo.height > target_width)
        {
            rect_info.top_left_y = (input_rgb_info.imageinfo.height-target_width)/2;
            rect_info.height = target_width;
        }else{
            rect_info.top_left_y = 0;
            rect_info.height = input_rgb_info.imageinfo.height;
        }
        //先释放之前申请的内存
        free(input_rgb_info.rgb_data);
        ret = jpeg_cutting_rect(SCALING_IMG_PATH, CUTTING_IMG_PATH, rect_info);
        if (ret != 0) {
            printf("jpeg_cutting_rect failed! ret:[%d]\n", ret);
            return -5;
        }

        //读取采集后的图像数据
        ret = jpeg_decode(CUTTING_IMG_PATH, &input_rgb_info);
        if (ret != 0) {
            printf("jpeg_decode failed! ret:[%d]\n", ret);
            return -4;
        }
    }
#if 1
    rgb_info_st rotate_rgb_info = {0};
    //旋转图像
    ret = RotateOfRight(&input_rgb_info, &rotate_rgb_info);
    free(input_rgb_info.rgb_data);
    if(ret != 0) {
        printf("RotateOfRight failed! ret:[%d]\n", ret);
        return -6;
    }

    char *fp_rotate_src = fopen("rgb_rotate.raw", "w+");
    if (fp_rotate_src) {
        fwrite(rotate_rgb_info.rgb_data, rotate_rgb_info.imageinfo.pixel_size, rotate_rgb_info.imageinfo.width*rotate_rgb_info.imageinfo.height, fp_rotate_src);
        fflush(fp_rotate_src);
        fclose(fp_rotate_src);
    }

    //填充图像到目标分辨率
    ret = rgb_fill_rgba(&rotate_rgb_info, out_rgba_info, target_width, target_height);
    free(rotate_rgb_info.rgb_data);
    if(ret != 0) {
        printf("rgb_fill_rgba failed! ret:[%d]\n", ret);
        return -7;
    }

    char *fp_target_src = fopen("rgb_target_512x288.raw", "w+");
    if (fp_target_src) {
        fwrite(out_rgba_info->rgb_data, 4, target_width*target_height, fp_target_src);
        fflush(fp_target_src);
        fclose(fp_target_src);
    }
#endif
    return 0;
}

//等比例缩小图像
static int img_scaling(char *img_file_name, int scaling)
{
    //检测接收到的文件是否正常
    if (access(img_file_name, F_OK) == -1) {
        printf("File does not exist! name:[%s]\n", img_file_name);
        return -1;
    }

    FILE *file = fopen(img_file_name, "rb");
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    fclose(file);
    if (file_size <= 0) {
        return -2;
    }
    //正常则进行人脸注册
    printf("jpeg size:[%d]\n", file_size);

    //对图片进行等比例缩放
    int ret = jpeg_proportiona_scaling(img_file_name, SCALING_IMG_PATH, scaling);
    if(ret != 0) {
        printf(" failed! ret:[%d]\n", ret);
        return -3;
    }

    //把处理完成的图片rgba数据读取出来
    rgb_info_st input_rgb_info = {0};
    ret = jpeg_decode(SCALING_IMG_PATH, &input_rgb_info);
    if (ret != 0) {
        printf("jpeg_decode failed! ret:[%d]\n", ret);
        return -4;
    }
    printf("width:[%u], height:[%u]\n", input_rgb_info.imageinfo.width, input_rgb_info.imageinfo.height);
    return 0;
}

int get_scale_factor(unsigned int in_width, unsigned int in_height, unsigned int out_width, unsigned int out_height)
{
    // 计算缩放因子，判断是否可以等比例缩放到指定分辨率
    float scale_factor_w = (float)in_width/(float)out_width;
    float scale_factor_h = (float)in_height/(float)out_height;
    printf("scale_factor_w:[%f], scale_factor_h:[%f]\n", scale_factor_w, scale_factor_h);
    unsigned int scale_factor = ((int)scale_factor_w)+1;
    printf("scale_factor:[%u]\n", scale_factor);

    /*
        int numerator, denominator;
        // 计算分子和分母
        numerator = cinfo.output_width - out_width;
        denominator = out_width;
        // 计算最大公约数
        int common_divisor = gcd(numerator, denominator);
        // 简化分数得到最终结果
        numerator /= common_divisor;
        denominator /= common_divisor;
        // 输出结果
        printf("分子: %d\n", numerator);
        printf("分母: %d\n", denominator);
    */
   return scale_factor;
}

void usage(char *program_name)
{
    fprintf(stderr, "\nUsage:  %s  [in_file_name]  [target_width] [target_height]\n\n", program_name);
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s 1.jpg 2\n", program_name);
    fprintf(stderr, "    %s 1.jpg 288 512\n", program_name);
}

//两种用法，第一种只传图片路径和一个参数，表示压缩比例
//第二种用法，传入图片路径和两个参数，表示压缩到指定分辨率
int main(int argc, char **argv)
{
    int ret = 0;
    if (argc < 2) {
        usage((char*)argv[0]);
        return -1;
    }

    if(argc == 3)
    {
        char img_file_name[256] = {0};
        memset(img_file_name, 0, sizeof(img_file_name));
        memcpy(img_file_name, argv[1], strlen(argv[1]));
        int scaling = atoi(argv[2]);
        printf("img_file_name:[%s], scaling:[%d]\n", img_file_name, scaling);
        ret = img_scaling(img_file_name, scaling);
        if (ret != 0) {
            printf("img_scaling failed! ret:[%d]\n", ret);
        }
    }else if (argc == 4){
        char img_file_name[256] = {0};
        memset(img_file_name, 0, sizeof(img_file_name));
        memcpy(img_file_name, argv[1], strlen(argv[1]));
        int target_width = atoi(argv[2]);
        int target_height = atoi(argv[3]);
        printf("img_file_name:[%s], target_width:[%d], target_height:[%d]\n", img_file_name, target_width, target_height);
        rgb_info_st target_rgba_info = {0};
        ret = img_scaling_rotate_fill(img_file_name, &target_rgba_info, target_width, target_height);
        if (ret != 0) {
            printf("img_scaling_rotate_fill failed! ret:[%d]\n", ret);
        }
        printf("width:[%u], height:[%u], rgba_size:[%lu]\n", target_rgba_info.imageinfo.width, target_rgba_info.imageinfo.height, target_rgba_info.rgb_size);
    }

#if 0
        //读取jpeg数据解码给tgba数据
        rgb_info_st input_rgb_info = {0};
        int ret = jpeg_decode(FACE_REGISTER_IMG_PATH, &input_rgb_info);
        if (ret != 0) {
            lmLogError("jpeg_decode failed! ret:[%d]", ret);
            send_face_reg_result(g_face_reg_type, 0, g_face_reg_id, 4);
            g_tel_face_download_thread_pid = 0;
            return NULL;
        }
        rgb_info_st target_rgb_info = {0};
        ret = rgba_scaling(&input_rgb_info, &target_rgb_info);
        if(ret != 0) {
            lmLogError("rgba_scaling failed! ret:[%d]", ret);
            send_face_reg_result(g_face_reg_type, 0, g_face_reg_id, 4);
            g_tel_face_download_thread_pid = 0;
            return NULL;
        }
#endif

    return 0;
}
