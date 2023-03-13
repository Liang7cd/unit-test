#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>

int ovd_getRMSInfo(char *json_str_buf)
{
	lmLogDebug("");
	// 创建一个root对象
	cJSON *root = cJSON_CreateObject();
	if (NULL == root)
	{
		lmLogError("Create cJSON Object error!");
		return -1;
	}

    {
        //一机一密ID信息（既用户ID）
        char authId[128] = {0};
        //一机一密密钥信息
        char authKey[128] = {0};
        //设备cmei码信息
        char cmei[128] = {0};
        //设备mac地址信息
        char mac_Addr[64] = {0};
        lemei_nvram_get(lemei_nvram_dtype_Product_guid, authId, sizeof(authId));
	    lemei_nvram_get(lemei_nvram_dtype_login_secret, authKey, sizeof(authKey));
	    lemei_nvram_get(lemei_nvram_dtype_cmcc_cmei, cmei, sizeof(cmei));
	    lemei_nvram_get(lemei_nvram_dtype_Device_Mac_Address, mac_Addr, sizeof(mac_Addr));
	    lmLogDebug("authId:[%s], authKey:[%s], cmei:[%s], mac_Addr:[%s]", authId, authKey, cmei, mac_Addr);

	    cJSON_AddStringToObject(root, "deviceId", authId);// 必填,设备唯一标识
	    cJSON_AddStringToObject(root, "deviceType", CMCC_ANDLINK_ID);// 必填,设备真实MAC,全大写不带冒号

        //时间戳（毫秒）
        unsigned long long current_time_ms = marshmallow_GetStandardMS();
	    cJSON_AddNumberToObject(root, "timestamp", current_time_ms);

        //添加cmei,真实mac,sn,操作系统信息
        cJSON_AddStringToObject(root, "cmei", cmei);// 必填,设备唯一标识
        cJSON_AddStringToObject(root, "mac", mac_Addr);// 必填,设备真实MAC,全大写不带冒号
        cJSON_AddStringToObject(root, "sn", authId);// 必填,设备真实SN
        cJSON_AddStringToObject(root, "OS", "arm-linux32 4.9.129");	// 必填,操作系统(包含版本号)

#if defined(CURRENT_COMPILE_PRODUCT_JM1111H) || defined(CURRENT_COMPILE_PRODUCT_TAI6000W) || defined(CURRENT_COMPILE_PRODUCT_TAI6000L) || defined(CURRENT_COMPILE_PRODUCT_TAI6250W)
	    cJSON_AddStringToObject(root, "networkType", "Wi-Fi");		//必填,网络类型分RJ45（有线）,wifi, 5G, 4G, 3G, NB, ZigBee等
#elif defined(CURRENT_COMPILE_PRODUCT_JM2111H) || defined(CURRENT_COMPILE_PRODUCT_TAI2000W)
	    cJSON_AddStringToObject(root, "networkType", "RJ45");		//必填,网络类型分RJ45（有线）,wifi, 5G, 4G, 3G, NB, ZigBee等
#else
#error
#endif

        cJSON_AddStringToObject(root, "deviceVendor", CMCC_DEVICE_VENDOR_NAME);		// 必填,设备制造商
	    cJSON_AddStringToObject(root, "deviceBrand", CMCC_DEVICE_BRAND_NAME);		// 必填,设备品牌
	    cJSON_AddStringToObject(root, "deviceModel", CMCC_DEVICE_MODEL);  // 必填,设备型号
#if defined(CURRENT_COMPILE_PRODUCT_JM1111H) || defined(CURRENT_COMPILE_PRODUCT_TAI6000W) || defined(CURRENT_COMPILE_PRODUCT_TAI6000L) || defined(CURRENT_COMPILE_PRODUCT_TAI6250W)
	    cJSON_AddStringToObject(root, "wlanMac", mac_Addr);   // 可选,设备的WLAN MAC地址
	    //电池供电:battery;   POE供电:POE;   市电:220V(110V);   USB供电:USB;   其他方式:other
	    cJSON_AddStringToObject(root, "powerSupplyMode", "USB");	//  必填,供电方式

	    char wifiRssi_buf[6] = {0};
	    char wifi_ssid[128] = {0};
        lemei_nvram_get(lemei_nvram_dtype_Wifi0_SSID, wifi_ssid, sizeof(wifi_ssid));
	    int wifiRssi = marshmallow_network_get_wifi_RSSI(wifi_ssid);
	    sprintf(wifiRssi_buf, "%d", (wifiRssi/2)-100);
	    cJSON_AddStringToObject(root, "wifiRssi", wifiRssi_buf);//  可选,wifi信号场强
#elif defined(CURRENT_COMPILE_PRODUCT_JM2111H) || defined(CURRENT_COMPILE_PRODUCT_TAI2000W)
	    //电池供电:battery;   POE供电:POE;   市电:220V(110V);   USB供电:USB;   其他方式:other
	    cJSON_AddStringToObject(root, "powerSupplyMode", "POE");	//  必填,供电方式
#else
#error
#endif
        //设备ip地址信息
        char ip_addr[64] = {0};
        marshmallow_network_get_ip(ip_addr);
	    cJSON_AddStringToObject(root, "deviceIP", ip_addr); //   IP设备必填,设备IP

        //固件版本号
        char system_version[32] = {0};
        if (strlen(system_version) == 0)
            strcpy(system_version, get_version_number());

	    // 4.添加版本信息
	    // 4.1产品本身版本信息
	    cJSON_AddStringToObject(root, "firmwareVersion", system_version);
	    cJSON_AddStringToObject(root, "softwareVersion", system_version);
    }
	
	char *jsonStr = cJSON_PrintUnformatted(root);
    lmLogDebug("strlen:[%d], jsonStr:[%s]", strlen(jsonStr), jsonStr);
	strncpy(json_str_buf, jsonStr, strlen(jsonStr));
    cJSON_free(jsonStr);

	cJSON_Delete(root);
	return 0;
}

static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    lmLogDebug("");
#if 1
    static int read_len = 0;//已读长度
    size_t realsize = size * nitems;

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
    memcpy(buffer, (char *)userdata, realsize);
    read_len += realsize;
    lmLogDebug("str_len:[%d], realsize:[%d], read_len:[%d], userdata:[%s]", str_len, realsize, read_len, userdata);

    return realsize;
#else
    FILE *readhere = (FILE *)userdata;
    curl_off_t nread;

    /* copy as much data as possible into the 'buffer' buffer, but no more than
       'size' * 'nitems' bytes! */
    size_t retcode = fread(buffer, size, nitems, readhere);

    nread = (curl_off_t)retcode;

    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
            " bytes from file\n", nread);
    return retcode;
#endif
}

//get请求和post请求数据响应函数
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    memcpy((char *)userdata, ptr, realsize);
    return realsize;
}

//http POST请求  
static CURLcode curl_post_req(const char *url, const char *postParams, char *response)
{
    // curl初始化  
    CURL *curl = curl_easy_init();
    // curl返回值 
    CURLcode res;
    if (curl)
    {
        // set params
        //设置curl的请求头
        struct curl_slist* header_list = NULL;
        header_list = curl_slist_append(header_list, "Content-Type:application/json");
        header_list = curl_slist_append(header_list, "Accept-Charset:utf-8");
        header_list = curl_slist_append(header_list, "User-Key:rms-sdhdict");
        header_list = curl_slist_append(header_list, "RMSToken:rms-sdhdict");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        //不接收响应头数据0代表不接收 1代表接收
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        //设置请求为post请求
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        //设置请求的URL地址
        curl_easy_setopt(curl, CURLOPT_URL, url);

        //设置post请求的参数
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams);

        //设置ssl验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

        //CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

/*
        FILE *json_file_p = NULL;
        json_file_p = fopen("/tmp/RMS_info.json", "w+");
        int write_size = strlen(postParams);
        if(write_size > 0)
        {
            int write_len = fwrite(postParams, 1, write_size, json_file_p);
            while(write_len < write_size) {
                lmLogWarn("write_len:[%d], write_size:[%d]", write_len, write_size);
                int temp_write = fwrite(postParams+write_len, 1, write_size-write_len, json_file_p);
                write_len += temp_write;
            }
        }else{
            lmLogError("write_size:[%d]", write_size);
        }
        fclose(json_file_p);
*/
        //FILE *file_p = fopen("/tmp/RMS_info.json", "rb");

/*
        //设置数据接收和写入函数
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, (void *)postParams);
        //上传使能 
	    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
*/

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

        //设置线程安全
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        //设置超时时间
        //curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6);
        //curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

        // 开启post请求
        res = curl_easy_perform(curl);

        //fclose(file_p);
    }
    //释放curl 
    curl_easy_cleanup(curl);
    return res;
}

int main(void) 
{
    lmLogDebug("");
#if 1
    // 初始化winsock的内容
    curl_global_init(CURL_GLOBAL_DEFAULT);

    //char *url_post = "https://www.wangsansan.com/mydir/test/HttpsPostTest.php";
    //char *url_post = "https://180.168.70.186:10070";
    char *url_post = "https://180.168.70.186:10070/device-manage/device/inform/dmReport";
    
    char *paramsLogin = calloc(1, 5*1024);
    char *resPost = calloc(1, 5*1024);
    while(1)
    {
        sleep(30);
        lmLogDebug("$$$$$");
        memset(paramsLogin, 0, 5*1024);
        memset(resPost, 0, 5*1024);

        ovd_getRMSInfo(paramsLogin);
        CURLcode res = curl_post_req(url_post, paramsLogin, resPost);
        if (res == CURLE_OK) {
            lmLogDebug("resPost:[%s]", resPost);
        } else {
            lmLogError("curl_easy_perform() failed:[%s]", curl_easy_strerror(res));
        }
    }
    curl_global_cleanup();
#endif

#if 0
    char *url_post0 = "http://httpbin.org/post";
    char *paramsLogin0 = "key1=value1&key2=value2";
    char resPost0[4096] = {0};
    CURLcode res3 = curl_post_req(url_post0, paramsLogin0, resPost0);
    if (res3 == CURLE_OK) {
        lmLogDebug("resPost0:[%s]", resPost0);
    }
#endif

#if 0
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");

#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who is not using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you are connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* always cleanup */
    curl_easy_cleanup(curl);
    }
#endif

    return 0;
}
