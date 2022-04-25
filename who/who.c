#include <utmp.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

enum option{
  OPTION_A = 'a',
  OPTION_B = 'b',
  OPTION_D = 'd',
  OPTION_L = 'l',
  OPTION_U = 'u'
};

typedef struct infoArray{
  struct utmp* info;
  int size;
}infoArray;

infoArray* p;

void freeMemory() {

  if(p->info != NULL) {
    free(p->info);
  }

  if(p != NULL) {
    free(p);
  }
}

void printInfoOptionB(struct utmp* info) {
  if(info->ut_type == BOOT_TIME) {
    long time = info->ut_tv.tv_sec;
    printf("      system boot %s", ctime(&time) + 4);
  }
}

void printInfoOptionD(struct utmp* info) {
  if(info->ut_type == DEAD_PROCESS) {
    long time = info->ut_tv.tv_sec;
    printf("%s    %s   %d\n",
      info->ut_line, ctime(&time) + 4, info->ut_exit.e_exit);
  }
}

void printInfoOptionLU(struct utmp* info, int type) {
  long time = info->ut_tv.tv_sec;
  char* s = ctime(&time) + 4;
  s[strlen(s) - 1] = '\0';
  if(info->ut_type == type) {
    printf("%s   %s   %s      %d\n", info->ut_user, info->ut_line, s,
      info->ut_pid);
  } else if(info->ut_type == type) {
    printf("%s   %s   %s      %d %d\n", info->ut_user, info->ut_line, s,
      info->ut_pid, info->ut_addr_v6[0]);
  }
}

void printInfo(enum option opt) {
  if(opt == OPTION_B) {
    printInfoOptionB(&p->info[0]);
  } else if(opt == OPTION_D) {
    for(int i = 0; i < p->size; ++i) {
      printInfoOptionD(&p->info[i]);
    }
  } else if(opt == OPTION_L) {
    for(int i = 0; i < p->size; ++i) {
      printInfoOptionLU(&p->info[i], LOGIN_PROCESS);
    }
  } else if(opt == OPTION_U) {
    for(int i = 0; i < p->size; ++i) {
      printInfoOptionLU(&p->info[i], USER_PROCESS);
    }
  } else if(opt == OPTION_A) {
    printInfoOptionB(&p->info[0]);
    for(int i = 1; i < p->size; ++i) {
      printInfoOptionD(&p->info[i]);
      printInfoOptionLU(&p->info[i], LOGIN_PROCESS);
      printInfoOptionLU(&p->info[i], USER_PROCESS);
    }
  }
}

/*
  * Read the `utmp` file to store each row
*/
void processFile() {
  const char* utmpFile = "/var/run/utmp";
  int fd;
  if((fd = open(utmpFile, O_RDONLY)) < 0) {
    printf("Error: can't open the %s\n", utmpFile);
    exit(1);
  }

  p = malloc(sizeof(infoArray));
  if(p == NULL) {
    printf("Error: can't allocate the memory\n");
    exit(1);
  }

  p->size = 1;
  p->info = malloc(sizeof(struct utmp) * p->size);
  if(p->info == NULL) {
    printf("Error: can't allocate the memory\n");
    exit(1);
  }

  while(read(fd, &p->info[p->size - 1], sizeof(struct utmp))) {
    p->size++;
    p->info = realloc(p->info, sizeof(struct utmp) * p->size);
    if(p->info == NULL) {
      printf("Error: can't reallocate the memory\n");
      exit(1);
    }
  }

  atexit(freeMemory);
}

/*
  * Use `getopt` to parse the command line
*/
void commandLineParse(int argc, char* argv[]) {
  int opt;
  if((opt = getopt(argc, argv, ":abdhlprTu")) != -1) {
    switch(opt) {
      case OPTION_A:
      case OPTION_B:
      case OPTION_D:
      case OPTION_L:
      case OPTION_U:
        processFile();
        printInfo(opt);
        break;
      case ':':
        printf("options needs value");
      case '?':
        printf("unkown option: %c\n", optopt);
      case 'h':
        printf("Usage: who [OPTION]\n"
               "Print information about users who are currently logged in."
               "\n"
               "  -a         same as -b -d -l -u\n"
               "  -b         time of last system boot\n"
               "  -d         print the dead processes\n"
               "  -l         print system login processes\n"
               "  -u         list users logged in\n"
               "  -h         display this help and exit\n");
        break;
    }
  }
}

int main(int argc, char* argv[]) {

  processFile();
  commandLineParse(argc, argv);
  exit(0);

}
