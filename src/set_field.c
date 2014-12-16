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

set_field.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:35:00  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


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

