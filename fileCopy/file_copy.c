#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static int file_copy(char *in_file_name, char *out_file_name)
{
    const unsigned int CRYPTO_BUF_SIZE = 64*1024;

    FILE *in_file_p = NULL;
    in_file_p = fopen(in_file_name, "r");
    if (!in_file_p) {
        printf("fopen file [%s] error!: %s\n", in_file_name, strerror(errno));
        return -1;
    }

    FILE *out_file_p = NULL;
    out_file_p = fopen(out_file_name, "w");
    if (!out_file_p) {
        printf("fopen file [%s] error!: %s\n", out_file_name, strerror(errno));
        fclose(in_file_p);
        return -1;
    }

    char *buf = calloc(1, CRYPTO_BUF_SIZE);
    if (buf == NULL) {
        printf("calloc buf size[%u] error!: %s\n", CRYPTO_BUF_SIZE, strerror(errno));
        fclose(in_file_p);
        fclose(out_file_p);
    	return -2;
    }

    fseek(in_file_p, 0, SEEK_END);
    unsigned long long in_file_size = ftell(in_file_p);
    printf("in_file_size:[%llu]\n", in_file_size);
    fseek(in_file_p, 0, SEEK_SET);

    unsigned long long temp_len = in_file_size;
    while(temp_len > 0)
    {
        bzero(buf, CRYPTO_BUF_SIZE);
        int read_len = fread(buf, 1, temp_len>=CRYPTO_BUF_SIZE?CRYPTO_BUF_SIZE:temp_len, in_file_p);
        if(read_len > 0)
        {
            int write_len = fwrite(buf, 1, read_len, out_file_p);
            while(write_len < read_len) {
                printf("write_len:[%d], read_len:[%d]\n", write_len, read_len);
                int temp_write = fwrite(buf+write_len, 1, read_len-write_len, out_file_p);
                write_len += temp_write;
            }
            temp_len -= read_len;
        }else{
            printf("fread error! ret:[%d]\n", read_len);
            continue;
        }
    }

    free(buf);
	fclose(in_file_p);
    fclose(out_file_p);
    return 0;
}

void usage(char *program_name)
{
    fprintf(stderr, "\nUsage:  %s  [in_file_name]  [out_file_name]\n\n", program_name);
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s in.txt out.txt\n", program_name);
}

int main(int argc, char **argv)
{
    int ret = 0;
    if (argc < 2) {
        usage((char*)argv[0]);
        return -1;
    }

/*
    if (access(argv[1], F_OK) == -1) {
        printf("File does not exist! name:[%s]\n", argv[1]);
        return -2;
    }
*/

    ret = file_copy(argv[1], argv[2]);
    if (ret != 0) {
        printf("file_copy failed! ret:[%d]\n", ret);
    }
    return 0;
}
