/* 
V1.1
set_field 1 "value"
sets a given field to a given value.
if no number is give, then the last field gets set to the value
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
char phase1[20000];
char phase2[20000];
int thefield;
char thevalue[20000];
char bufcpy[20000];

int main (int argc, char *argv[]) {
int i;

for (i=1;i<argc;i++) {
  int w=0;
    w=atoi(argv[i]);
  if (w) 
    thefield=w;
  else {
    strcpy(thevalue,argv[i]);
    }
  }

while(gets(buf) != NULL) {
  strcpy(bufcpy,buf);
  if (buf[0] == '\0') return;
  if (thefield) {
    fieldspan(buf,phase1,0,thefield-2,1);
    fieldspan(buf,phase2,thefield,300,1);
    /* add extr fields if you need to */
    add_fields(phase1,thefield-1);
    if (phase2[0]) {
      printf("%s%s|%s\n",phase1,thevalue,phase2);
      }
    else {
      printf("%s%s\n",phase1,thevalue);
      }
    }
  else {
    printf("%s|%s\n",buf,thevalue);
    }
  }
return(0);
}

