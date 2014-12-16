/* 
This is used to find duplicate records in the first n fields.
then it is output to standard output if regular, stderr, if duplicate
n is hardcoded to 15 in this case - but can be expanded.
*/

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

int n=15;


int init (int argc, char *argv[]) {
int i;
for (i=1;i<argc;i++) {
  long long w=0;
  w=atoll(argv[i]);
  if (w != 0) {
    n=w;
    }
  }
return 0;
}




int main (int argc, char *argv[]) {
init(argc,argv);
char buf[20000];
char address[20000];
char id[20000];
char prev_address[20000];
char prev_id[20000];

prev_address[0]='\0';
prev_id[0]='\0';

while (gets(buf)) {
  fieldspan(buf,address,0,n-1,1);
  field(buf,id,n);
  if ((strcmp(address,prev_address)==0)) {
    fprintf(stderr,"%s\n",buf);
    }
  else {
    fprintf(stdout,"%s\n",buf);
    }
  strcpy(prev_address,address);
  strcpy(prev_id,id);
  }
return(0);
}


