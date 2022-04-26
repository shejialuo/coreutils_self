#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

void ls(char* dirname) {
  DIR* dir = opendir(dirname);
  struct dirent* entry;
  while((entry = readdir(dir)) != NULL) {
    printf("%s\n", entry->d_name);
  }
}

int main(int argc, char* argv[]) {
  if(argc == 1) {
    ls(".");
  }
  exit(0);
}