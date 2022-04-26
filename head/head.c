#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void headBytes(char* file, int num) {

  int fd = open(file, O_RDONLY);
  if(fd < 0) {
    printf("Error: can't open the file %s\n", file);
    exit(1);
  }

  int endPosition;
  if((endPosition = lseek(fd, 0, SEEK_END)) < 0) {
    printf("Error: can't seek the file %s\n", file);
    exit(1);
  }

  if(num > endPosition) {
    num = endPosition;
  }

  if(lseek(fd, 0, SEEK_SET)) {
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
  printf("\n\n");

  close(fd);
  free(buf);
}

void headLine(char* file, int num) {
  int fd = open(file, O_RDONLY);
  if(fd < 0) {
    printf("Error: can't open the file %s\n", file);
    exit(1);
  }

  int endPosition = 0;
  if((endPosition = lseek(fd, 0 , SEEK_END)) < 0) {
    printf("Error: can't seek the file %s\n", file);
    exit(1);
  }

  if(lseek(fd, 0, SEEK_SET)) {
    printf("Error: can't seek the file %s\n", file);
    exit(1);
  }

  char *buf = malloc(1);
  if(buf == NULL) {
    printf("Error: can't allocate the memory\n");
    exit(1);
  }

  int i = 0;
  int size = 0;
  while(1) {

    if(lseek(fd, 0, SEEK_CUR) == endPosition) {
      break;
    }

    if(read(fd, buf, 1) != 1) {
      printf("Error: can't read the file %s\n", file);
      exit(1);
    }

    if(*buf == '\n') {
      i++;
      if(i == num) {
        break;
      }
    }
    size++;
  }

  if(lseek(fd, 0, SEEK_SET)) {
    printf("Error: can't seek the file %s\n", file);
    exit(1);
  }

  buf = realloc(buf, size);
  if(buf == NULL) {
    printf("Error: can't allocate the memory\n");
    exit(1);
  }

  if(read(fd, buf, size) != size) {
    printf("Error: can't read result into the buffer\n");
    exit(1);
  }

  printf("====>   %s    <====\n", file);
  if(write(STDOUT_FILENO, buf, size) != size) {
    printf("Error: can't write result into the buffer\n");
    exit(1);
  }

  printf("\n\n");

  free(buf);
  close(fd);
}

void commandLineParse(int argc, char* argv[]) {
  int opt;
  if((opt = getopt(argc, argv, ":c:n:h")) != -1) {
    switch(opt) {
      case 'c':
        for(; optind < argc; optind++) {
          int num = atoi(optarg);
          headBytes(argv[optind], num);
        }
        break;
      case 'n':
        for(; optind < argc; optind++) {
          int num = atoi(optarg);
          headLine(argv[optind], num);
        }
        break;
      case ':':
        printf("options %c need value\n", optopt);
      case '?':
        if(opt == '?')
          printf("unkown option: %c\n", optopt);
      case 'h':
        printf("Usage: head [OPTION]... [FILE]...\n"
               "Print the first 10 lines of each FILE to standard output.\n"
               "With more than one FILE, precede each with a header giving the file name.\n"
               "\n"
               "  -c NUM        output the last NUM bytes\n"
               "  -n NUM        output the last NUM lines\n");
        break;
    }
  } else {
    for(; optind < argc; optind++) {
      headLine(argv[optind], 10);
    }
  }
}

int main(int argc, char* argv[]) {
  commandLineParse(argc, argv);
  exit(0);
}
