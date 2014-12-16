/* 
V1.1
is_it_filled_in - send to stdout if the field is filled in
send to stderr otherwise
-n - reverse
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "uthash.h"
#include "util.h"


#define MAXFIELDS 1000
int fields[MAXFIELDS];
int ifields[MAXFIELDS];
int ifields_count=0;

int fieldcount=0;

char *fpos[10000];

char buf[20000];
char obuf[20000];
int reverse_matching=0;
char bufcpy[20000];
FILE *yesf;
FILE *nof;

int main (int argc, char *argv[]) {
int i;

for (i=1;i<argc;i++) {
  if (strncmp(argv[i],"-n",2)==0) {
    reverse_matching=1;
    continue;
    }
  int w=0;
    w=atoi(argv[i]);
    ifields[ifields_count++]=w;
  }
if (reverse_matching) {
  yesf=stderr;nof=stdout;
  }
else {
  yesf=stdout;nof=stderr;
  }

while(gets(buf) != NULL) {
  strcpy(bufcpy,buf);
  /* split it out */
  int cols=0;
  char *q= buf;
  char *r = q;
  char *optr = obuf;
  while (*r) {
    while (*r &&(*r != '|')) r++;
    fpos[cols++] =q;
    if (*r) {
      *r = '\0';
      r++;
      }
    q=r;    
    }
  int flag=1; /* If we process or not */
  if (ifields_count) {
    for (i=0;i<ifields_count;i++) {
      if ((ifields[i]-1>=cols) || (fpos[ifields[i]-1][0] == '\0')) { /* if we are empty at this field or if thge field is too high */
        flag=0; /* we aint loadin this one */
	break;
	}
      }
    }    
  if (flag) {
    fprintf(yesf,"%s\n",bufcpy);
    }
  else {
    fprintf(nof,"%s\n",bufcpy);
    }
  }
return(0);
}



