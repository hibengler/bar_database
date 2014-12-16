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


set_id $Revision$

$Log$
Revision 1.2  2014/12/16 17:35:00  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


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


int populate=0;
int locked=0;

int id=1000000001;




int init (int argc, char *argv[]) {
return 0;
}


int main (int argc, char *argv[]) {
init(argc,argv);
char buf[20000];
char out[20000];
char phase1[20000];
char phase2[20000];
char gen[20000];

char lname[20000];
char listed_gender[10];
char preferred_gender[10];
while (gets(buf)) {
  fieldspan(buf,phase1,2,300,1);
  printf("|%d|%s\n",id,phase1);
  id++;
  }
return(0);
}


