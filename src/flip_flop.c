/* flipflop - a program to split out a file into 2 separate jobs.
One is standard error, and one is standard output 
This takes one argument - the number of flipflops to do
This can be from 1 to 9,  default (or zero) is 2

This goes to stdout, stderr, the file 3, 4,5,6,7,8,9
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

char buf[1000000];
int main (int argc, char *argv[]) {
int degree=2;
int i;
for (i=1;i<argc;i++) {
  int x=atoi(argv[i]);
  if (!x) x=2;
  if ((x>9)||(x<1)) {
    fprintf(stderr,"flip flop: illegal degree of flipping.  1-9 is the range\n");
    exit(-1);
    }
  degree=x;
  }

FILE *des[10];
des[0]=stdout;
des[1]=stderr;
char abuf[65536];
char bbuf[65536];
int x;
x=setvbuf(stdout,abuf,_IOFBF,65536);
if (x) {fprintf(stderr,"could not buffer stdio\n");exit(-1);}
x=setvbuf(stderr,bbuf,_IOFBF,65536);
if (x) {fprintf(stderr,"could not buffer sterr\n");exit(-1);}
for (i=2;i<degree;i++) {
  char fname[20];
  int y=i+1;
  des[i]=fdopen(y,"w");
/*  fprintf(stderr,"got %d\n",y);*/
  }

i=0;
while (gets(buf)) {
  fprintf(des[i],"%s\n",buf);
  i= (i+1) % degree;
  }
exit(0);
}
