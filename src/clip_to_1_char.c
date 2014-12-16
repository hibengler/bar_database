/*
bar_database - commands to manipulate vertical bar delimited fields as a NOSql style database.
Copyright (C) 2010-2014  Hibbard M. Engler of Killer Cool Development, LLC.

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

clip_to_1_char.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:34:59  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!

*/

/* Clip specifed fields to 1 character size
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
