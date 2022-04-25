#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void tailBytes(char* file, int num) {
  int fd = open(file, O_RDONLY);
  if(fd < 0) {
    printf("Error: can't open the file %s\n", file);
    exit(1);
  }

  if(lseek(fd, -num ,SEEK_END) < 0) {
    printf("Error: can't seek the file %s\n", file);
    exit(1);
  }

  char *buf = malloc(num);
  if(buf == NULL) {
    printf("Error: can't allocate the memory\n");
    exit(1);
  }

  if(read(fd, buf, num) != num) {
    printf("Error: can't read result into the buffer\n");
    exit(1);
  }

  printf("====>   %s    <====\n", file);
  if(write(STDOUT_FILENO, buf, num) != num) {
    printf("Error: can't write result into the buffer\n");
    exit(1);
  }
  printf("\n\n\n");

  close(fd);
  free(buf);
}

void commandLineParse(int argc, char* argv[]) {
  int opt;
  if((opt = getopt(argc, argv, ":c:n:h")) != -1) {
    switch(opt) {
      case 'c':
        for(; optind < argc; optind++) {
          int num = atoi(optarg);
          tailBytes(argv[optind], num);
        }
        break;
      case 'n':
        // TODO: need to finish line
        break;
      case ':':
        printf("options %c need value\n", optopt);
      case '?':
        if(opt == '?')
          printf("unkown option: %c\n", optopt);
      case 'h':
        printf("Usage: tail [OPTION]... [FILE]...\n"
               "Print the last 10 lines of each FILE to standard output.\n"
               "With more than one FILE, precede each with a header giving the file name.\n"
               "\n"
               "With no FILE, or when FILE is -, read standard input\n"
               "  -c NUM        output the last NUM bytes\n"
               "  -n NUM        output the last NUM lines\n");
        break;
    }
  } else {
    // TODO: default cases
  }
}

int main(int argc, char* argv[]) {
  commandLineParse(argc, argv);
  exit(0);
}
