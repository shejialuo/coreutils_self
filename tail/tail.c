#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
  * tailBytes is simple, just use `lseek` to
  * do the offset, read the content to the `buf`,
  * and write the content to the STDOUT
*/
void tailBytes(char* file, int num) {

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

  if(lseek(fd, -num ,SEEK_CUR) < 0) {
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

/*
  * `tailLine`, just put the offset to the end, and iteratively
  * to finish the functionality. However, there is som
*/
void tailLine(char* file, int num) {
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

  char *buf = malloc(1);
  if(buf == NULL) {
    printf("Error: can't allocate the memory\n");
    exit(1);
  }

  int i = 0;
  int startPosition = 0;
  while(1) {

    if(lseek(fd, 0, SEEK_CUR) == 0) {
      break;
    }

    // First, we offset the pointer to read the char
    if(lseek(fd, -1 ,SEEK_CUR) < 0) {
      printf("Error: can't seek the file %s\n", file);
      exit(1);
    }

    // And we read it, remember when we read it the offset
    // will plus 1
    if(read(fd, buf, 1) != 1) {
      printf("Error: can't read the file %s\n", file);
      exit(1);
    }

    // It is important to restore the file offsert
    if(lseek(fd, -1 ,SEEK_CUR) < 0) {
      printf("Error: can't seek the file %s\n", file);
      exit(1);
    }

    if(*buf == '\n') {
      i++;
      if(i == num) {
        // When the loop is done, the offset is pointed to the
        // '\n', We need it to be put to the head of the first
        // line
        if((startPosition = lseek(fd, 1 ,SEEK_CUR)) < 0) {
          printf("Error: can't seek the file %s\n", file);
          exit(1);
        }
        break;
      }
    }
  }

  int size = endPosition - startPosition;

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
          tailBytes(argv[optind], num);
        }
        break;
      case 'n':
        for(; optind < argc; optind++) {
          int num = atoi(optarg);
          tailLine(argv[optind], num);
        }
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
               "  -c NUM        output the last NUM bytes\n"
               "  -n NUM        output the last NUM lines\n");
        break;
    }
  } else {
    for(; optind < argc; optind++) {
      tailLine(argv[optind], 10);
    }
  }
}

int main(int argc, char* argv[]) {
  commandLineParse(argc, argv);
  exit(0);
}
