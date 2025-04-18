#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/capability.h>
 
extern int errno;
  
void whoami(void)
{
  printf("uid=%i  euid=%i  gid=%i\n", getuid(), geteuid(), getgid());
}
 
void listCaps()
{
  cap_t caps = cap_get_proc();
  ssize_t y = 0;
  printf("The process %d was give capabilities %s\n",
        (int) getpid(), cap_to_text(caps, &y));
  fflush(0);
  cap_free(caps);
}

void usage(char *program_name)
{
  fprintf(stderr, "\nUsage:  %s  [file_name]\n\n", program_name);
  fprintf(stderr, "Example:\n");
  fprintf(stderr, "    %s test\n", program_name);
}

//arm-fullhanv3-linux-uclibcgnueabi-gcc set_read_search.c -o set_read_search -I libcap-2.66/install/usr/include/ -L libcap-2.66/install/lib64/ -lcap -static
int main(int argc, char **argv)
{
  int stat;
  whoami();
  stat = setuid(geteuid());
  pid_t parentPid = getpid();

  if(!parentPid)
    return 1;

  if (argc < 2)
  {
    usage((char*)argv[0]);
    return -1;
  }
  
  cap_t caps = cap_init();

  cap_value_t capList[2] =
  { CAP_DAC_READ_SEARCH, CAP_SYS_ADMIN };
  unsigned num_caps = 2;
  cap_set_flag(caps, CAP_EFFECTIVE, num_caps, capList, CAP_SET);
  cap_set_flag(caps, CAP_INHERITABLE, num_caps, capList, CAP_SET);
  cap_set_flag(caps, CAP_PERMITTED, num_caps, capList, CAP_SET);

  if (cap_set_file(argv[1], caps)) {
    perror("capset()");
    return EXIT_FAILURE;
  }
  listCaps();

  cap_free(caps);

  struct __user_cap_header_struct cap_header_data;
  cap_user_header_t cap_header = &cap_header_data;

  struct __user_cap_data_struct cap_data_data;
  cap_user_data_t cap_data = &cap_data_data;

  cap_header->pid = getpid();
  cap_header->version = _LINUX_CAPABILITY_VERSION_1;

  if (capget(cap_header, cap_data) < 0) {
    perror("Failed capget");
    exit(1);
  }
  printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
    cap_data->permitted, cap_data->inheritable);
  return 0;
}
