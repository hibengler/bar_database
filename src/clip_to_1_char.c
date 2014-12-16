/* Clip speciifed fiels to 1 character size
This is done primarily to handle the gender benders - where the gender is MM, MF, FF, or FM
showing that the person was derived from a person of a different gender.
This is useful for name scrambling later on,  but goofs up some code that does work based
on gender
Hib 7/21/2010
*/
#include <stdio.h>
#include <string.h>


int fields[1000];
int fieldcount=0;

char *fpos[10000];

char buf[20000];

int main (int argc, char *argv[]) {
int i;
for (i=1;i<argc;i++) {
  int w=0;
  w=atoi(argv[i]);
  if (w != 0) {
    fields[fieldcount++] = w;
    }
  }

while(gets(buf) != NULL) {
  int cols=0;
  char *q= buf;
  char *r = q;
  while (*r) {
    while (*r &&(*r != '|')) r++;
    fpos[cols++] =q;
    if (*r) {
      *r = '\0';
      r++;
      }
    q=r;    
    }
  for (i=0;i<cols;i++) {
    int j;
    for (j=0;j<fieldcount;j++) {
      if (fields[j]-1 == i) {
        if (fpos[i][1]) {
	  fpos[i][1] = '\0'; // clip
	  }
        break;
	}
      }
    if (i) printf("|%s",fpos[i]);
    else printf("%s",fpos[i]);
    }
  putchar('\n');
  } 
return(0);
}
