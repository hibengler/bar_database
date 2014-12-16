/* $Revision$

$Log$
Revision 1.1  2014/12/16 15:39:03  hib
initial gpu2

Revision 1.12  2013/03/27 10:35:23  hib
field.c fielduc.c - both have the -2 special check to help with initials being all over the source data for UK.

xlate.c fixed when http://burner:8889/phn/20//28 caused alot of ERROR in the log- it is because of
assumptions on the phone field, which are not always true.  This should fix the US version as well. YEAH!

Revision 1.11  2013/03/27 09:07:16  hib
util.c - xlate.c - cleaning up for UK.
fielduc.c - added the -2 option to reject fields with less than 2 characters.
This is used to make the names better for first name and last name pickings


V1.5
V1.5 - added -t ( -t4 is the same as -l1 4)
V1.4 - added -l<num> which will limit the size of the next specified field to <num> characters
        This is used to make partial indexes - like all names that start with the letter H
V1.3 - added -i<num> to only include if not null the give field.  All -i's must be satisfied for the record to be included.
       nd -x<num> to exclude numbers where this is filled in. Any -x's that are null will cause the record to be included.
V1.2 - Don't close the temo files till the end- so that they ar flushed out
during the read of the input file. :)

V1.1 - pretty good
same as fielduc - but we also have a marker field.

-s - force sorting.  This avoids sorting if it can

If -M(number) is specified
Than the first m fields are considered a marker indicator
We have to sort then, by the way.
But we enter a marker number - the marker number is used to "pick"
a value randomly later on.
-M0 is a special case, we marker everything

Well, -M works,  but if you want it sorted by the m, we need anohter 
option.   -m  - works the same - except the index number is sent out
as a 000000000001 and added right after the merge field.
This way, the system sorts:
abalos|00000000| -- etc.  And we can search that way.

print out fields - if they are unique.
the order that fields get printed out to is arbitrary.
This uses memory to store the hash table. So it cannot be used fro real big ones
But perhaps fielduv can

If the first number is bigger than or equal to 10000, this is assumed to be the number of max
fields to read into memory -- this is good for really large sets of data.
For these real large sets of data,  we will peridocially do the following:
1. sort the data.
2. dump the data to a temp file.
3. call sort to merge the temp files together
4. read the result from sort, and sum up the counts

The sort goes to disk, so it will take longer of coarse,  but not much longer, I hope.

Anyways this also does a count - an additional field.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "uthash.h"
#include "util.h"


struct value_struct {
char *value;
int count;
UT_hash_handle hh;
};

#define MAXFIELDS 1000
int fields[MAXFIELDS];
int fieldsize[MAXFIELDS]; /* 0 use the whole field size,  if non zero, use the given field size */
int ifields[MAXFIELDS];
int ifields_count=0;
int twofields[MAXFIELDS];
int twofields_count=0;
int xfields[MAXFIELDS];
int xfields_count=0;

int fieldcount=0;

char *fpos[10000];

char buf[20000];
char obuf[20000];

int key_sort(struct value_struct *a,struct value_struct *b) {
return strcmp(a->value,b->value);
}


int main (int argc, char *argv[]) {
struct value_struct *values;
struct value_struct *c;
int all_in_memory_flag;
int sortflag=0;
values = NULL;
char * tmp = getenv("TMP");
if (!tmp || (tmp[0]=='\0')) {
  tmp="/tmp";
  }
int i;
int maxsize;
int cursize;
int markerfields=-1;
int memposition=1;
int memfiles = 0;
FILE *xfs[100];
maxsize=2000000000;
cursize = 0;
all_in_memory_flag = 1;

fieldsize[0]=0;

for (i=1;i<argc;i++) {
  int w=0;
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
  if (strncmp(argv[i],"-m",2)==0) {
    markerfields = atoi(argv[i]+2);
    continue;
    }
  if (strncmp(argv[i],"-M",2)==0) {
    markerfields = atoi(argv[i]+2);
    memposition=0; /* at the very end */
    continue;
    }
  if (strncmp(argv[i],"-s",2)==0) {
    sortflag=1;
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
  w=atoi(argv[i]);
  if (w != 0) {
    if (w>=10000) {
      maxsize=w;
      }
    else {
      fields[fieldcount++] = w;
      fieldsize[fieldcount]=0;  /* clean up the next field size */
      }
    }
  }

while(gets(buf) != NULL) {
  /* split it out */
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
    
  for (i=0;i<fieldcount;i++) {
    int pos = fields[i]-1;
    if (pos<cols) {
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
    *optr++ = '|';
    }
  
  *optr='\0';
  HASH_FIND_STR(values,obuf,c);
  if (c) {
    c->count++;
    }
  else {
    cursize++;
    if (cursize >= maxsize) { /* It is too big */
      all_in_memory_flag = 0; /* switch modes */
      HASH_SORT(values,key_sort);
      sprintf(buf,"%s/temp%d_%d",tmp,getpid(),memfiles); /* should open in tmp folder */
      FILE *xf = fopen(buf,"w");
      if (!xf) { fprintf(stderr,"Error- Could not open temp file at %s\n",buf);
        exit(-1);
	}
      xfs[memfiles] = xf; /* save the file descriptor, close at the end. :) */
      for (c=values; c!= NULL; c=values) {
        fprintf(xf,"%s|%d\n",c->value,c->count);
        HASH_DELETE(hh,values,c);
        free(c);
        }      
      values=NULL;
      memfiles++;
      cursize = 1;
      }
    c = malloc(sizeof(struct value_struct) + (optr-obuf+1) * sizeof(char));
    if (!c) {
      fprintf(stderr,"Out of memory! darn!\n");
      exit(-1);
      }
    c->value = (char *)(c+1);
    c->count = 1;
    strcpy(c->value,obuf);
    HASH_ADD_KEYPTR(hh,values,c->value,(optr-obuf),c);
    }  
  }

int mc = 0;
char prevmarkerstring[20000];
char markerstring[20000];
prevmarkerstring[0]='\0';

if (all_in_memory_flag) {
  
  if (sortflag ||(markerfields>0)) HASH_SORT(values,key_sort);
  /* We need to sort if we are doing markers grouped by something
  but for a standard grouping,  we can just run the markers regular.
  or for no marker, we also do not have to sort.
  */
  for (c=values; c!= NULL; c=c->hh.next) {
    if (markerfields!= -1) {
      make_marker(c->value,markerstring,markerfields);
      if (strcmp(markerstring,prevmarkerstring)) {
        strcpy(prevmarkerstring,markerstring);
	mc=0;
	}
      mc += c->count;
      if (memposition) 
        printf("%s%010d|%s%d\n",markerstring,mc,c->value+strlen(markerstring),c->count);
      else
        printf("%s%d|%d\n",c->value,c->count,mc);
      }
    else 
      printf("%s%d\n",c->value,c->count);
    
    }
  }
else {
  HASH_SORT(values,key_sort);
  sprintf(buf,"%s/temp%d_%d",tmp,getpid(),memfiles); /* should open in tmp folder */
  FILE *xf = fopen(buf,"w");
  if (!xf) { fprintf(stderr,"Error- Could not open temp file at %s\n",buf);
    exit(-1);
    }
  for (c=values; c!= NULL; c=values) {
    fprintf(xf,"%s|%d\n",c->value,c->count);
    HASH_DELETE(hh,values,c);
    free(c);
    }      
  fclose(xf);
  
  for (i=0;i<memfiles;i++) {
    fclose(xfs[i]); /* close them here, now, after we are done reading the input file
               This is so that we don't wait for the slow io of writing the temp files
	       if we are reading from the source file, which usually is on another
	       disk device. */
    }
  
  memfiles++;

  FILE *files[10000];
  char *bufs[10000]; 
  int counts[10000];
  for (i=0;i<memfiles;i++) {
    sprintf(buf,"%s/temp%d_%d",tmp,getpid(),i);
    files[i]=fopen(buf,"r");
    if (!files[i]) {
      fprintf(stderr,"error: cannot open temp file %s\n",buf);
      exit(-1);
      }
    bufs[i] = malloc(20000);
    if (!bufs[i]) {
      fprintf(stderr,"error: out of memory\n");
      exit(-1);
      }
    counts[i]=0;
    }
  
  
    /* get the first lines of all files for comparison */
    for (i=0;i<memfiles;i++) {
      if (files[i]) {
        if (fgets(buf,19999,files[i])) {
	  getcount(bufs[i],buf,counts+i);
          }
	else {
	  fclose(files[i]);
          files[i]=NULL;
          sprintf(buf,"%s/temp%d_%d",tmp,getpid(),i); /* should open in tmp folder */
	  remove(buf);
          }
        }
      }
  {
    while (1) {
      int lowest = -1;
      /* find the first record  - selection sort :( */
      for (i=0;i<memfiles;i++) {
        if (files[i]) {
          if (lowest==-1) {
            lowest=i;
   	    continue;
	    }
          int c = strcmp(bufs[i],bufs[lowest]);
          if (c<0) {
            lowest=i;
	    continue;
	    }
          if (c==0) { /* equal to our current lowest - consume it and add the count */
            counts[lowest] += counts[i];
            if (fgets(buf,19999,files[i])) {
	      getcount(bufs[i],buf,counts+i);
	      }
	    else {
              fclose(files[i]);
              files[i]=NULL;
              sprintf(buf,"%s/temp%d_%d",tmp,getpid(),i); /* should open in tmp folder */
	      remove(buf);
              }
	    }
          /* c>0 - ignore */
          }
        }
      if (lowest!=-1) {
        /* we got the lowest. print it out and sort again */
        if (markerfields!=-1) {
          make_marker(bufs[lowest],markerstring,markerfields);
          if (strcmp(markerstring,prevmarkerstring)) {
            strcpy(prevmarkerstring,markerstring);
	    mc=0;
	    }
          mc += counts[lowest];
          if (memposition) 
            printf("%s%010d|%s%d\n",markerstring,mc,bufs[lowest]+
	           strlen(markerstring),counts[lowest]);
          else
	    printf("%s%d|%d\n",bufs[lowest],counts[lowest],mc);
	  }
	else
	  printf("%s%d\n",bufs[lowest],counts[lowest]);
        if (fgets(buf,19999,files[lowest])) {
	  getcount(bufs[lowest],buf,counts+lowest);
	  }
	else {
          fclose(files[lowest]);
          files[lowest]=NULL;
          sprintf(buf,"%s/temp%d_%d",tmp,getpid(),lowest); /* should open in tmp folder */
	  remove(buf);
          }
	}
      else {
        break; /* need to fill more lines */
	}
      } /* while */ 
    } /* block */
  } /* if we could not do it all in memory */  
return(0);
}



