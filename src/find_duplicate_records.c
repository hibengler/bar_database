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

find_duplicate_records.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:35:00  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!

*/
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


