/* print out fields */
#include <stdio.h>
#include <string.h>
#include "util.h"



char buf[20000];

int main (int argc, char *argv[]) {
int number_of_fields=1;
int i;
for (i=1;i<argc;i++) {
  int w=0;
  w=atoi(argv[i]);
  if (w != 0) {
    number_of_fields = w;
    }
  }

while(gets(buf) != NULL) {
  set_number_of_fields(buf,number_of_fields);
  printf("%s\n",buf);
  } 
return(0);
}
