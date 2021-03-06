/* 
bar_database - commands to manipulate vertical bar delimited fields as a NOSql style database.
Copyright (C) 2011-2014  Hibbard M. Engler of Killer Cool Development, LLC.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

fieldu.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:35:00  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


V1.2 - Added -l
-s - sort before printing
print out fields - if they are unique.
the order that fields get printed out to is arbitrary.
This uses memory to store the hash table. So it cannot be used fro real big ones
But perhaps fielduv can!
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "uthash.h"



struct value_struct {
char *value;
int id;
UT_hash_handle hh;
};

int fieldsize[1000]; /* 0 use the whole field size,  if non zero, use the given field size */
int fields[1000];
int fieldcount=0;

char *fpos[10000];

char buf[20000];
char obuf[20000];


int key_sort(struct value_struct *a,struct value_struct *b) {
return strcmp(a->value,b->value);
}

int main (int argc, char *argv[]) {
int sortflag=0;
struct value_struct *values;
struct value_struct *c;
values = NULL;
int i;
fieldsize[0]=0;
for (i=1;i<argc;i++) {
  int w=0;
  if (strcmp(argv[i],"-s")) {
    sortflag=1;
    }
  if (strncmp(argv[i],"-l",2)==0) {
    int w = atoi(argv[i]+2);
    fieldsize[fieldcount] = w;
    continue;
    }  
  w=atoi(argv[i]);
  if (w != 0) {
    fields[fieldcount++] = w;
    fieldsize[fieldcount] = 0;
    }
  }

while(gets(buf) != NULL) {
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
    
  for (i=0;i<fieldcount;i++) {
    int pos = fields[i]-1;
    if (pos<cols) {
      if (i) *optr++ ='|';
      char *iptr;
      if (fieldsize[i]) { /* restrict the field to less characters */
        int field_size = fieldsize[i];
	char *thepos=fpos[pos];
        for (iptr = thepos;(*iptr)&&(iptr-thepos < field_size);iptr++) {
          *optr++ = *iptr;
  	  }	
        }
      else {
        for (iptr = fpos[pos];*iptr;iptr++) {
          *optr++ = *iptr;
  	  }
	}
      }
    else {
      if (i) *optr++ = '|';
      }
    }
  *optr='\0';
  HASH_FIND_STR(values,obuf,c);
  if (!c) {
    c = malloc(sizeof(struct value_struct) + (optr-obuf+1) * sizeof(char));
    if (!c) {
      fprintf(stderr,"Out of memory! darn!\n");
      exit(-1);
      }
    c->value = (char *)(c+1);
    strcpy(c->value,obuf);
    HASH_ADD_KEYPTR(hh,values,c->value,(optr-obuf),c);
    }  
  } 
if (sortflag) {
  HASH_SORT(values,key_sort);
  };
for (c=values; c!= NULL; c=c->hh.next) {
  puts(c->value);
  }
return(0);
}
