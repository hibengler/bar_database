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

flip_flop.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:35:00  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


*/

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
