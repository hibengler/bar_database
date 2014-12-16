#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "util.h"
#include "bsearch.h"



long long id=000000001;




int init (int argc, char *argv[]) {
return 0;
}


int main (int argc, char *argv[]) {
init(argc,argv);
char buf[20000];

while (gets(buf)) {
/*  printf("%012qd|%s\n",id,buf);*/
  printf("%010qd|%s\n",id,buf);
  id++;
  }
return(0);
}


