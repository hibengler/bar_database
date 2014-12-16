/*
bar_database-lib - libraries to manipulate vertical bar delimited fields as a NOSql style database.
Copyright (C) 2011-2014 Hibbard M. Engler of Killer Cool Development, LLC.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

util.h $Revision$

$Log$
Revision 1.2  2014/12/16 17:35:01  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


*/

char *temp_dir();

/* pull one field out of a bunch */
char *field(char *x, char *y,int fieldnum);


/* pull one numerical field out of a bunch */
int fieldi(char *x,int fieldnum);


/* pull a range of fields out of a bunch
source,
destination,
from field number (starts with 0)
to field number (starts with 0)
withend - include the sentenniel
*/
char *fieldspan(char *x, char *y,int fromfieldnum,int tofieldnum,int withend);








int getcount(char *out,char *in,int *pcount);
int get2counts(char *out,char *in,int *pcount1,int *pcount2,int *pfields);

/* make a marker identification string

Say we have
Engler|Hibbard
Engler|Micelle
Engler|Tim
And all are the same length,
we would produce:
Engler|
If markerfields is 1

Markerfields of 0 returns a null string
*/
int make_marker(char *source,char *dest, int markerfields);



int add_fields(char *x, int numfields);



int date_to_code(char *date);
char * code_to_date(int code,char*date);




char *set_number_of_fields(char *x,int number_of_fields);



char * initcap(char *dest,char *src);
char * upper(char *dest,char *src);
char * lower(char *dest,char *src);
int date_to_mdy(char *date,int *pmonth,int *pday, int *pyear);

char *decode_url(char *out,char *in,int max);
char *encode_url(char *out,char *in,int max);

char *encode_html(char *out,char *in, int max);
