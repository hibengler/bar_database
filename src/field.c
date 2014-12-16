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

field.c

print out fields 

$Revision$

$Log$
Revision 1.2  2014/12/16 17:34:59  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!

Revision 1.1  2014/12/16 15:39:03  hib
initial gpu2

Revision 1.7  2013/03/27 10:35:23  hib
field.c fielduc.c - both have the -2 special check to help with initials being all over the source data for UK.

xlate.c fixed when http://burner:8889/phn/20//28 caused alot of ERROR in the log- it is because of
assumptions on the phone field, which are not always true.  This should fix the US version as well. YEAH!


V1.3 Added -l also to limit fields to a particular length
V1.2 - Added the -i, -x, -n, -d
V1.1 - oringial - the start of th hibnosql stuff yeah!
*/
#include <stdio.h>
#include <string.h>


#define MAXFIELDS 1000
int fields[MAXFIELDS];
int fieldsize[MAXFIELDS]; /* 0 use the whole field size,  if non zero, use the given field size */
int ifields[MAXFIELDS];
int ifields_count=0;
int twofields[MAXFIELDS];
int twofields_count=0;
int xfields[MAXFIELDS];
int xfields_count=0;
int ufields_count=0;   /* for unique check - to avoid duplicates where the uniqued fields are next to eachother - effectively repeating
                           This is mostly useful for sorted streams, and was added to eliminate duplicate SSN information from the SSDI sources. */
int ufields[MAXFIELDS];

int fieldcount=0;

int add_random =0;
int add_index_position=0;


long long index_position=0;
long long last_index_position=0;



char *fpos[10000];

char buf[20000];
char obuf[20000];
char ubuf[20000];
char old_ubuf[20000];

int main (int argc, char *argv[]) {
int i;
fieldsize[0]=0;
strcpy(old_ubuf,"gjfvyu26t8bufiu6fu6f3u3gffgegnu3uffgz3j24xv4353erycry#ht2h4Chd2C@dc#$8V!R&DC@RC!&CR&DR$II&ghkhyeh5kg5h78eynkeyhil7gny5iio73gi7gio5io7byios guu5 fgzf8 5ouf||REGT\eg$|g4|G|$TG|T$|GT$gt");

for (i=1;i<argc;i++) {
  long long w=0;
  if (strncmp(argv[i],"-i",2)==0) { /* include only if this field is filled in */
    w=atoi(argv[i]+2);
    ifields[ifields_count++]=w;
    continue;
    }
  if (strncmp(argv[i],"-x",2)==0) { /* exclude only if this field is filled in */
    w=atoi(argv[i]+2);
    xfields[xfields_count++]=w;
    continue;
    }
  if (strncmp(argv[i],"-2",2)==0) { /* include only if this field is filled in with two or more characters*/
    w=atoi(argv[i]+2);
    twofields[twofields_count++]=w;
    continue;
    }
  if (strncmp(argv[i],"-u",2)==0) { /* exclude if this field is not unique compared to the last record ( and other fields also specified) */
    w=atoi(argv[i]+2);
    ufields[ufields_count++]=w;
    continue;
    }
  if (strncmp(argv[i],"-n",2)==0) { /* Add index */
    add_index_position=1;
    continue;
    }
  if (strncmp(argv[i],"-d",2)==0) { /* Add random */
    srandom(atoi(argv[i]+2));
    add_random=1;
    continue;
    }
  if (strncmp(argv[i],"-t",2)==0) { /* take only the first character */
    w=atoll(argv[i]+2);
    if (w != 0) {
      fieldsize[fieldcount]=1;
      fields[fieldcount++] = w;
      }
    continue;
    }
  if (strncmp(argv[i],"-l",2)==0) {
    int w = atoi(argv[i]+2);
    fieldsize[fieldcount] = w;
    continue;
    }
  w=atoll(argv[i]);
  if (w != 0) {
    fields[fieldcount++] = w;
    fieldsize[fieldcount]=0;
    }
  }

while(gets(buf) != NULL) {
  int cols=0;
  char *q= buf;
  char *r = q;
  char *optr = obuf;
  last_index_position=index_position;
  index_position += strlen(buf) + 1;
    
  while (*r) {
    while (*r &&(*r != '|')) r++;
    fpos[cols++] =q;
    if (*r) {
      *r = '\0';
      r++;
      }
    q=r;    
    }
    
  int flag=1; /* If we process or not */
  if (ifields_count) {
    for (i=0;i<ifields_count;i++) {
      if (fpos[ifields[i]-1][0] == '\0') { /* if we are empty at this field */
        flag=0; /* we aint loadin this one */
	break;
	}
      }
    }    
  
  if (!flag) continue;  
  
  
  if (twofields_count) {
    for (i=0;i<twofields_count;i++) {
      char *pt = fpos[twofields[i]-1];
      if ((pt[0] == '\0')||(pt[1] == '\0')||(pt[2] == '\0')) { /* if we have 2 or less characters */
        flag=0; /* we aint loadin this one */
	break;
	}
      }
    }    
  
  if (!flag) continue;  
  
  if (xfields_count) {
    flag=0;
    for (i=0;i<xfields_count;i++) {
      if (fpos[xfields[i]-1][0] == '\0') { /* if we are empty at this field */
        flag=1; /* we is a loadin this one */
	break;
	}
      }
    }    
  
  if (!flag) continue;
  
  
  if (ufields_count) {
    char *uptr = ubuf;
    for (i=0;i<ufields_count;i++) {
      int pos = ufields[i]-1;
      if (pos<cols) {
        char *iptr;
        for (iptr = fpos[pos];*iptr;iptr++) {
          *uptr++ = *iptr;
  	  }
	}
      }
    if (i!= ufields_count-1) *uptr++ = '|';
    *uptr='\0';
    
    if (strcmp(ubuf,old_ubuf)==0) continue;
    strcpy(old_ubuf,ubuf);
    }
    
  for (i=0;i<fieldcount;i++) {
    int pos = fields[i]-1;
    if (pos<cols) {
      if (fieldsize[i]) { /* truncate the field to one character */
        int field_size = fieldsize[i];
	char *thepos=fpos[pos];
        char *iptr;
        for (iptr = thepos;(*iptr)&&(iptr-thepos < field_size);iptr++) {
          *optr++ = *iptr;
  	  }	
        }
      else {
        char *iptr;
        for (iptr = fpos[pos];*iptr;iptr++) {
          *optr++ = *iptr;
  	  }
	}
      }
    if (i!= fieldcount-1) *optr++ = '|';
    }
  
  *optr='\0';
  
  /* Add the random number or the index position, or both */
  if (add_random) {
    int r=random();
    if (add_index_position) {
      sprintf(optr,"|%010d|%lld",r,last_index_position);
      }
    else {
      sprintf(optr,"|%010d",r);
      }
    }
  else {
    if (add_index_position) {
      sprintf(optr,"|%lld",last_index_position);
      }
    }
    
  printf("%s\n",obuf);
  
  } 
return(0);
}
