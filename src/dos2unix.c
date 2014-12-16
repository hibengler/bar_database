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

dos2unix.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:34:59  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!

*/
#include <stdio.h>
int main ()
{
int c;
while ((c=getchar()) != EOF) {
  if (c !='\r') putchar(c);
  }
}
