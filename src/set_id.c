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


int populate=0;
int locked=0;

int id=1000000001;




int init (int argc, char *argv[]) {
return 0;
}


int main (int argc, char *argv[]) {
init(argc,argv);
char buf[20000];
char out[20000];
char phase1[20000];
char phase2[20000];
char gen[20000];

char lname[20000];
char listed_gender[10];
char preferred_gender[10];
while (gets(buf)) {
  fieldspan(buf,phase1,2,300,1);
  printf("|%d|%s\n",id,phase1);
  id++;
  }
return(0);
}


