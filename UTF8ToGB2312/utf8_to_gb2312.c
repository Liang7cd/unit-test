#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char byte;

/*
Unicode符号范围     |        UTF-8编码方式
(十六进制)        |              （二进制）
----------------------+---------------------------------------------
0000 0000-0000 007F | 0xxxxxxx
0000 0080-0000 07FF | 110xxxxx 10xxxxxx
0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
/**
 * @brief utf8编码转unicode字符集(usc4)，最大支持4字节utf8编码，(4字节以上在中日韩文中为生僻字)
 * @param *utf8 utf8变长编码字节集1~4个字节
 * @param *unicode utf8编码转unicode字符集结果，最大4个字节，返回的字节序与utf8编码序一致
 * @return length 0：utf8解码异常，others：本次utf8编码长度
 */
uint8_t UTF8ToUnicode(uint8_t *utf8, uint32_t *unicode) {
    const uint8_t lut_size = 3;
    const uint8_t length_lut[] = {2, 3, 4};
    const uint8_t range_lut[] = {0xE0, 0xF0, 0xF8};
    const uint8_t mask_lut[] = {0x1F, 0x0F, 0x07};

    uint8_t length = 0;
    byte b = *(utf8 + 0);
    uint32_t i = 0;

    if(utf8 == NULL) {
        *unicode = 0;
        return (0);
    }
    // utf8编码兼容ASCII编码,使用0xxxxxx 表示00~7F
    if(b < 0x80) {
        *unicode =  b;
        return 1;
    }
    // utf8不兼容ISO8859-1 ASCII拓展字符集
    // 同时最大支持编码6个字节即1111110X
    if(b < 0xC0 || b > 0xFD) {
        *unicode = 0;
        return (0);
    }
    for(i = 0; i < lut_size; i++) {
        if(b < range_lut[i]) {
            *unicode = b & mask_lut[i];
            length = length_lut[i];
            break;
        }
    }
    // 超过四字节的utf8编码不进行解析
    if(length == 0) {
        *unicode = 0;
        return (0);
    }
    // 取后续字节数据
    for(i = 1; i < length; i++ ) {
        b = *(utf8 + i);
        // 多字节utf8编码后续字节范围10xxxxxx~10111111
        if( b < 0x80 || b > 0xBF ) {
            break;
        }
        *unicode <<= 6;
        // 00111111
        *unicode |= (b & 0x3F);
    }
    // 长度校验
    return (i < length) ? (0) : length;
}

/**
 * @brief 4字节unicode(usc4)字符集转utf16编码
 * @param unicode unicode字符值
 * @param *utf16 utf16编码结果
 * @return utf16长度，(2字节)单位
 */
uint8_t UnicodeToUTF16(uint32_t unicode, uint16_t *utf16) {
    // Unicode范围 U+000~U+FFFF
    // utf16编码方式：2 Byte存储，编码后等于Unicode值
    if(unicode <= 0xFFFF) {
		if(utf16 != NULL) {
			*utf16 = (unicode & 0xFFFF);
		}
		return 1;
	}else if( unicode <= 0xEFFFF ) {
		if( utf16 != NULL ) {
		    // 高10位
		    *(utf16 + 0) = 0xD800 + (unicode >> 10) - 0x40;
            // 低10位
		    *(utf16 + 1) = 0xDC00 + (unicode &0x03FF);
		}
		return 2;
	}

    return 0;
}

typedef union _short {
	uint8_t bytes[2];
	uint16_t value;
} Short;

/**
 * @brief 使用utf16.lut查找表查询unicode对应的gb2312编码
 * @brief unicode小端方式输入，gb2312大端方式输出
 * @param unicode 双字节unicode码, 小端模式
 * @param *gb2312 输出的gb2312字符集，大端模式
 * @return 成功返回0
 * */
int bsearch_gb2312(uint16_t unicode, Short *gb2312) {
	uint16_t readin = 0;
	uint32_t pack, group;
	int32_t start, middle, end;

    FILE *file_p = fopen("./fontmap/utf16.bin", "rb");
	if(file_p != NULL) {
        fseek(file_p, 0, SEEK_END);
        unsigned long long file_len = ftell(file_p);
        printf("file_len:[%lld]\n", file_len);

		// 四字节一组，低两字节为unicode，高两字节为gb2312
		group = (file_len / sizeof(uint32_t));
		middle = group / 2;
		end = group;
		start = -1;
		do {
			// 二分法，从中间查起
            fseek(file_p, middle*sizeof(uint32_t), SEEK_SET);
            int read_len = fread((uint8_t *)&pack, 1, sizeof(uint32_t), file_p);
			if(!read_len || middle <= 0 || middle >= (group - 1)) {
                printf("Lookup failed!\n");
				return -1;
			}
			readin = (uint16_t)(pack & 0xFFFF);
			if(unicode < readin) {
				// 向左查询
				end = middle;
				middle -= ((end - start) / 2);
			}else if(unicode > readin) {
				// 向右查询
				start = middle;
				middle += ((end - start) / 2);
			}
		}while(unicode != readin);

        printf("pack:[%x]\n", pack);

		gb2312->bytes[0] = (uint8_t)((pack >> 24) & 0xFF);
		gb2312->bytes[1] = (uint8_t)((pack >> 16) & 0xFF);
	}else{
        return -2;
    }
	return 0;
}

int UTF8ToGB2312(const char *in_utf8, int iInLen, char *out_gb2312, int iMaxOutLen)
{
    char *text_p = NULL;
    if (in_utf8 == NULL || out_gb2312 == NULL) {
        return -1;
    } else {
        text_p = (char *)in_utf8;
    }

	for (int i=0, j=0; i<iInLen&&j<iMaxOutLen; i++, j++) 
    {
		if(*(text_p+i) & 0x80){
            uint32_t buffer;
            uint16_t utf16[2] = {0};
            uint8_t utf8[3] = {0};
            utf8[0] = *(text_p+i);
            utf8[1] = *(text_p+i+1);
            utf8[2] = *(text_p+i+2);
			printf("utf81:[%x], utf82:[%x], utf83:[%x]\n", utf8[0], utf8[1], utf8[2]);

            uint8_t len = UTF8ToUnicode(utf8, &buffer);
            printf("len:%d\n", len);
            printf("buffer:%x\n", buffer);

            len = UnicodeToUTF16(buffer, utf16);
            printf("len:%d\n", len);
            printf("utf16[0]:%x\n", utf16[0]);
            printf("utf16[1]:%x\n", utf16[1]);

            Short gb2312 = {0};
            int ret = bsearch_gb2312(utf16[0], &gb2312);
            if(ret != 0) {
                printf("bsearch_gb2312 ret:[%d]\n", ret);
            }
            printf("gb2312[0]:%x\n", gb2312.bytes[0]);
            printf("gb2312[1]:%x\n", gb2312.bytes[1]);

            out_gb2312[j] = gb2312.bytes[1];
            out_gb2312[j+1] = gb2312.bytes[0];
            i+=2;
            j+=1;
		}else{
			printf("english %x\n",*(text_p+i));
            out_gb2312[j] = *(text_p+i);
		}
    }
    return 0;
}

int main() {

    uint8_t text1[256] = "aaa测试文本1";
    uint8_t text2[256] = "bbb测试文本2";
    uint8_t text3[256] = "ccc测试文本3";
    
    uint8_t text_in[256] = {0};
    sprintf((void*)text_in, "%s\n%s\n%s", text1, text2, text3);
    printf("text_len:[%ld]\n", strlen(text_in));

    uint8_t text_out[256] = {0};
    UTF8ToGB2312(text_in, strlen(text_in), text_out, sizeof(text_out));
    printf("text_out:[%s], len:[%ld]\n", text_out, strlen(text_out));

    FILE *file_p = NULL;
    file_p = fopen("./out.txt", "w+");
    fwrite(text_out, strlen(text_out) , 1, file_p);
    fclose(file_p);

    return 0;
}
