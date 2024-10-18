#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <unistd.h>

/*====================================================================================================*/
/*=======================================curl相关接口(Start)==========================================*/
//是否存储get和post请求信息到文件
#define RECEIVE_INFO_FILE 1

/**
 * @brief get请求和post请求数据接收回调函数
 * 
 * @param ptr 指向存储从libcurl接收到的数据的缓冲区
 * @param size 数据块的大小（通常是字节大小）。这个参数表示缓冲区中每个数据单元的大小。
 * @param nmemb 数据块的数量。这个参数表示缓冲区中有多少个这样的数据单元。因此，总的数据量是size * nmemb。
 * @param userdata 用户提供的指针。在设置libcurl选项时，你可以传递一个指针给libcurl，这个指针通常是一个文件指针（如通过fopen返回的），而libcurl会将其传递给回调函数。你可以在回调函数中使用这个指针来写入数据。
 * @return size_t 已成功处理的数据量。如果返回的数值与传入的size * nmemb不相等，libcurl会认为发生了一个错误，并会终止传输。因此，在实现回调函数时，应确保返回值正确，通常是成功写入的字节数。
 */
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
#if RECEIVE_INFO_FILE
    size_t realsize = size * nmemb;
    FILE *ota_file_p = (FILE *)userdata;

    int write_len = fwrite(ptr, 1, realsize, ota_file_p);
    while(write_len < realsize) {
        printf("write_len:[%d], realsize:[%ld]\n", write_len, realsize);
        int temp_write = fwrite(ptr+write_len, 1, realsize-write_len, ota_file_p);
        write_len += temp_write;
    }
    return realsize;
#else
    size_t realsize = size * nmemb;
    memcpy((char *)userdata, ptr, realsize);
    return realsize;
#endif
}

/**
 * @brief https POST请求 获取数据。
 * 
 * @param urlAddr 需要访问的http/https地址。
 * @param postParams post请求的信息。
 * @param response 回调参数指针，提供write_callback和read_callback使用。
 * @return CURLcode 访问结果，CURLE_OK表示成功，出错时可通过curl_easy_strerror()获取错误信息。
 */
static CURLcode curl_post_receive_request(const char *urlAddr, const char *postParams, char *response)
{
    //curl初始化
    CURL *curl = curl_easy_init();
    //curl返回值
    CURLcode res;
    if (curl)
    {
        // set params
        //设置curl的请求头
        struct curl_slist* header_list = NULL;
        header_list = curl_slist_append(header_list, "Content-Type:application/json");
        header_list = curl_slist_append(header_list, "Accept-Charset:utf-8");
        //==========对应网页自定义的参数==========
        //header_list = curl_slist_append(header_list, "User-Key:rms-sdhdict");
        //header_list = curl_slist_append(header_list, "RMSToken:rms-sdhdict");
        //======================================
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        //不接收响应头数据0代表不接收 1代表接收
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        //设置请求为post请求
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        //设置请求的URL地址
        curl_easy_setopt(curl, CURLOPT_URL, urlAddr);
        //设置post请求的参数
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams);
        //设置ssl验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        //CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //设置数据接收回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        //设置线程安全
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        //设置超时时间
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

        //开启post请求
        res = curl_easy_perform(curl);
    }
    //释放curl 
    curl_easy_cleanup(curl);
    return res;
}

//是否存储get和post请求信息到文件
#define UPLOAD_INFO_FILE 1
/**
 * @brief 通过CURLOPT_READFUNCTION设置，允许你定义一个自定义回调函数来提供上传的数据
 * 
 * @param ptr 缓冲区指针，你应该将要上传的数据写入这个指针指向的位置。
 * @param size 表示每个数据块的大小。
 * @param nmemb 数据块的数量。size*nmemb这两个值的乘积是缓冲区的总大小，以字节为单位。
 * @param userdata 由 CURLOPT_READDATA 设置的指针，可以用来传递自定义数据到回调函数中。
 * @return size_t 回调函数应返回写入到 ptr 的实际字节数。如果返回值小于 size * nmemb 且不为 0，会被认为上传过程已经结束。如果返回 CURL_READFUNC_ABORT 会中止传输。
 */
static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
#if UPLOAD_INFO_FILE
    FILE *readhere = (FILE *)userdata;
    curl_off_t nread;

    /* copy as much data as possible into the 'ptr' ptr, but no more than
       'size' * 'nmemb' bytes! */
    size_t retcode = fread(ptr, size, nmemb, readhere);

    nread = (curl_off_t)retcode;

    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
            " bytes from file\n", nread);
    return retcode;
#else
    static int read_len = 0;//已读长度
    size_t realsize = size * nmemb;

    int str_len = 0;
    if(read_len > 0) {
        str_len = strlen(userdata) - read_len;
        if(str_len == 0){
            read_len = 0;
            return 0;
        }
    } else {
        str_len = strlen(userdata);
    }
    if(realsize > str_len) {
        realsize = str_len;
    }
    memcpy(ptr, (char *)userdata+read_len, realsize);
    read_len += realsize;
    printf("str_len:[%d], realsize:[%d], read_len:[%d], userdata:[%s]\n", str_len, realsize, read_len, (char*)userdata);

    return realsize;
#endif
}

/**
 * @brief https 上传文件。
 * 
 * @param urlAddr 需要访问的http/https地址。
 * @param response 回调参数指针，提供write_callback和read_callback使用。
 * @param file_name 需要上传的文件名字
 * @return CURLcode 访问结果，CURLE_OK表示成功，出错时可通过curl_easy_strerror()获取错误信息。
 */
static CURLcode curl_upload_file(const char *urlAddr, char *response, const char *file_name)
{
	FILE *file_p = fopen(file_name, "rb");
    if (file_p == NULL) {
		printf("fopen %s failed\n", file_name);
        return -1;
    }

    fseek(file_p, 0, SEEK_END);
    unsigned long long file_size = ftell(file_p);
    fseek(file_p, 0, SEEK_SET);

    if (file_size <= 0) {		
		printf("%s file_size is: %lld\n", file_name, file_size);
        fclose(file_p);
        return -1;
    }

	CURL *curl=NULL;        
	CURLcode res; 
	//初始化libcurl
	//得到 easy interface型指针
	curl = curl_easy_init();
	if (curl) {
        //设置curl的请求头
        struct curl_slist* header_list = NULL;
        header_list = curl_slist_append(header_list, "Content-Disposition:attachment");
#if 0
        //示例代码，具体格式规则由平台确定
        char header_buf[256] = {0};
        sprintf(header_buf, "filename=%s", file_name);
        header_list = curl_slist_append(header_list, header_buf);
#endif
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        //CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //设置不验证CA和host
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 
		//设置访问URL
		curl_easy_setopt(curl, CURLOPT_URL, urlAddr); 
        //设置数据上传回调函数
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, (void *)file_p);
        //上传使能 
	    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	    //启用 PUT 请求
	    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	    //提供上传文件的大小 必须确保使用正确的数据大小 */
	    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) file_size);

        //设置线程安全
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        //设置超时时间
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

        //执行 PUT 请求
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {			
        	printf("curl_easy_perform failed: %s\n", curl_easy_strerror(res));
    	}
	}
	curl_easy_cleanup(curl);

    fclose(file_p);
	return file_size;
}

/**
 * @brief 下载指定链接中的文件
 * 
 * @param urlAddr 下载链接地址
 * @param file_name 下载后的文件存储路径和名字
 * @return int 成功0
 */
static int curl_download_file(const char *urlAddr, const char *file_name)
{
    //打开写入文件
    FILE *file_p = NULL;
    file_p = fopen(file_name, "w+");
    if(file_p == NULL) {
        printf("fopen file:[%s] error!\n", file_name);
        return -1;
    }
    
    // curl初始化  
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("curl_easy_init() Failed\n");
        return -2;
    }

    //先使用HEAD请求获取文件大小，
    double getfileSize;
    CURLcode res;
    //设置访问URL
    curl_easy_setopt(curl, CURLOPT_URL, urlAddr);
    //设置ssl验证
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    //CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    //只请求资源的元数据（即响应头），而不是整个响应内容。这意味着响应体不会被下载
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -3;
    } else {
        res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &getfileSize);
        if(res == CURLE_OK) {
            printf("File size: %.0f bytes\n", getfileSize);
        } else {
            fprintf(stderr, "Failed to get file size: %s\n", curl_easy_strerror(res));
            return -4;
        }
    }

    //循环下载，直到成功
    while(1)
    {
        // set params
        //不接收响应头数据0代表不接收 1代表接收
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        //使用GET请求下载文件
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        //设置数据接收和写入函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file_p);
        //设置线程安全
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        //设置超时时间
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

        // 开启get请求
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            printf("curl_easy_perform() succeed.\n");
            break;
        } else {
            printf("curl_easy_perform() failed:[%s]\n", curl_easy_strerror(res));
        }
    }
    //释放curl 
    curl_easy_cleanup(curl);
    fclose(file_p);

    //文件大小验证
    if (access(file_name, F_OK) == 0) {
        FILE *out_file_p = fopen(file_name, "rb");
        if (out_file_p == NULL) {
	    	printf("fopen %s failed\n", file_name);
            return -5;
        }

        fseek(out_file_p, 0, SEEK_END);
        unsigned long long file_size = ftell(out_file_p);
        fseek(out_file_p, 0, SEEK_SET);

        if (file_size != getfileSize) {		
	    	printf("%s file_size is: %lld\n", file_name, file_size);
            fclose(out_file_p);
            return -6;
        }
        fclose(out_file_p);
    }
    return 0;
}
/*=======================================curl相关接口(End)============================================*/
/*====================================================================================================*/

/*====================================================================================================*/
/*=======================================接口测试代码(Start)==========================================*/
//上传文件名称
#define UPLOAD_FILE_NAME "upload_file.txt"
//接收信息存储文件名称
#define RECEIVE_FILE_NAME "receive_info.txt"
static int curl_post_test()
{
    printf("start send post...\n");
    char *url_post = "https://reqres.in";
    //char *url_post = "https://www.wangsansan.com/mydir/test/HttpsPostTest.php";
    //char *url_post = "https://180.168.70.186:10070";
    //char *url_post = "https://223.99.141.196:9008/device-manage/device/inform/dmReport";
    //char *url_post = "https://180.168.70.186:10070/device-manage/device/inform/dmReport";

    char *paramsLogin = calloc(1, 5*1024);
    sprintf(paramsLogin, "{\"name\":\"morpheus\",\"job\":\"leader\"}", NULL);

#if RECEIVE_INFO_FILE
    //打开写入文件
    FILE *file_p = NULL;
    file_p = fopen(RECEIVE_FILE_NAME, "w+");
    if(file_p == NULL) {
        printf("fopen file:[/tmp/alertarea_speech.wav] error!\n");
        return -1;
    }
    CURLcode res = curl_post_receive_request(url_post, paramsLogin, file_p);
    if (res == CURLE_OK) {
        if (access(RECEIVE_FILE_NAME, F_OK) == 0) {
            printf("file %s exists.", RECEIVE_FILE_NAME);
        }
    } else {
        printf("curl_easy_perform() failed:[%s]\n", curl_easy_strerror(res));
    }
    free(paramsLogin);
    fclose(file_p);
#else
    char *resPost = calloc(1, 5*1024);
    CURLcode res = curl_post_receive_request(url_post, paramsLogin, resPost);
    if (res == CURLE_OK) {
        printf("resPost:[%s]\n", resPost);
    } else {
        printf("curl_easy_perform() failed:[%s]\n", curl_easy_strerror(res));
    }
    free(paramsLogin);
    free(resPost);
#endif
    printf("stop post Reconnect!\n");
    return 0;
}

//下载文件名称
#define DOWNLOAD_FILE_NAME "download_file.txt"
static int curl_download_test()
{
    printf("start download...\n");
    //char *url_addr = "http://shres.189smarthome.com:7777/image/Ota/Ota_5b3ced556c86bede2c7af712222cd844_VD-2_5.4.5.18_240920.bin";
    char *url_addr = "https://staticcdn.189smarthome.com/image/Ota/Ota_84dd23105365328113a5b206f23c58b7_VD-2_5.4.5.19_241010.bin";
    curl_download_file(url_addr, DOWNLOAD_FILE_NAME);
    printf("stop download.\n");
}

/*=======================================接口测试代码(End)============================================*/
/*====================================================================================================*/


void usage(char *program_name)
{
    fprintf(stderr, "\nUsage: %s type_num\n\n", program_name);
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s 1\n", program_name);
}

//gcc curl_main.c -o curl_main -lcurl
int main(int argc, char **argv)
{
    int ret = 0;
    if (argc < 2) {
        usage((char*)argv[0]);
        return -1;
    }

    int type_num = atoi(argv[1]);
    printf("%s@%d, type_num:[%d]\n", __FUNCTION__, __LINE__, type_num);

    /**
     * @brief 用于对整个 libcurl 库进行全局初始化
     * curl_global_init必须在任何其他 libcurl 函数调用之前进行，并且在你程序进程的生命周期中至少调用一次。
     * 如果你的程序在多线程环境中使用 libcurl，确保 curl_global_init 是进程中第一个调用的 libcurl 函数，以防止多线程竞态。
     * 在程序结束时或者不再需要使用 libcurl 功能时，应调用 curl_global_cleanup，以释放分配的全局资源。
     */
    curl_global_init(CURL_GLOBAL_DEFAULT);

    switch (type_num)
    {
    case 1:
        curl_post_test();
        break;
    case 2:
        curl_download_test();
        break;
    default:
        break;
    }

    //对应于 curl_global_init
    curl_global_cleanup();
    return 0;
}
