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

$Revision$ fsort.c

$Log$
Revision 1.2  2014/12/16 17:35:00  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


V1.7 added -Q which is for 64 bit numbers
V1.6 added -l that allows restriction to a certain number of characters for a field
V1.5 - added -q - to add an id number at the beginning of the output
V1.4 - added the indexing option -n - which makes an index file for the original
input file.
The fields are output as asked - with one additional fields added
which is a 64 bit number holding the position of the original line.
(We could have the length but this is stupid considering that we are going to be memory mapping
these suckers anyways)
Also added -d which adds a random number to the sort to make the order of the
original list impossible to discern. -r takes a seed number as an argument




V1.3 - Added a simple radix - which is good for low cardinaliry sorts,  such as by state, etc.
-r does not use internal memory. It instead makes multiple files - one per item, and writes to them.  
Added the -k option that keeps the entire text to the end of the sorted fields.
V1.2 - fixed a nasty bug that would cause fsort to crash, or do loops or other
nasty stuff.
Also fixed it so after 200 files (1000 in linux)  a error message is given
that a file cannot be opened.  The limit can be increased in linux as follows:

echo fs.file-max=100000 >>/etc/sysctl.conf
echo hib soft nofile 80000 >>/etc/security/limits.conf
echo hib hard nofile 80000 >>/etc/security/limits.conf


This sorts the data, and is more memory efficient than fielduc.  That being said, it 
can also do the same unique counting and everythign that fielduc can do-- eventually.

But right now, it just sorts.
fsort <maxsize_bytes> <fields>   <from >to
example:
fsort 2000000 <a.txt >b.txt 1 4 3


Note - afer about 200 temp files,  fsort crashes.  Havent figured out why,  but probably too many files for the system or something like that.


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "uthash.h"
#include "util.h"
#include <errno.h>

#define MAXFIELDS 1000
int fields[MAXFIELDS];
int fieldsize[MAXFIELDS];  /* 0 use the whole field size,  if non zero, use the given field size */
int ifields[MAXFIELDS];
int ifields_count=0;
int xfields[MAXFIELDS];
int xfields_count=0;
int add_id=0;
int big_add_id=0;
int fieldcount=0;

int add_random =0;
int add_index_position=0;

char *fpos[10000];

long long index_position=0;
long long last_index_position=0;

int id=1;
long long big_id=1;

struct entry_struct {
char *value;
struct entry_struct *next;
};

struct merge_struct {
struct entry_struct *head;
struct entry_struct *tail;
struct merge_struct *next;
};

struct merge_struct *head = NULL;
struct merge_struct *tail = NULL;

struct radix_struct {
char *value;
char *fname;
FILE *fs;
UT_hash_handle hh;
};


char buf[20000];
char obuf[20000];
char keepbuf[20000];


static inline strip_new_line(char *buf) {
char *x=buf;
while (*x) x++;
if ((x!=buf)&& x[-1]=='\n') x--;
*x = '\0';
}



int radix_key_sort(struct radix_struct *a,struct radix_struct *b) {
return strcmp(a->value,b->value);
}




/* in memory merge sort */
int inmemory_sort() {
while (head != tail ) {
  struct merge_struct *a,*b;
  a=head;
  b=a->next;
  head = NULL;
  tail = NULL;
  while (a) {
    if (b) {
      struct entry_struct *ehead;
      struct entry_struct *etail;
      ehead = NULL;
      etail = NULL;
      struct entry_struct *aa=a->head;
      struct entry_struct *bb=b->head;
      while (1) {
        struct entry_struct *cc;
	
        if (aa) {
	  if (bb) {
	    if (strcmp(aa->value,bb->value) <=0) {
	      /* a goes first */
	      cc=aa;
	      aa=aa->next;
	      }
	    else { 
	      /* b goes first */
	      cc=bb;
	      bb=bb->next;
	      }
	    cc->next=NULL;
	    if (etail) {
	      etail->next = cc;
	      }
	    else {
	      ehead = cc;
	      }
	    etail = cc;
	    } /* normal comparison */
	  else { /* bb is spent. link up aa to the rest and call it a day */
	    if (etail) {
	      etail->next = aa;
	      }
	    else {
	      ehead = aa;
	      }
	    etail = a->tail;
	    break;
	    }
	  } /* if we have aa */
	else { /* aa is spent*/
	  if (bb) { /* but we have some more on bb */
	    if (etail) {
	      etail->next = bb;
	      }
	    else {
	      ehead = bb;
	      }
	    etail = b->tail;
	    } 
	  break;
	  }
        } /* while looping through */
      /* we will use a, and discard b*/
      struct merge_struct *c;
      if (b) c=b->next; else c=NULL;
      a->head = ehead;
      a->tail = etail;
      a->next = NULL;
      free(b);
      if (tail) tail->next=a; else head=a;
      tail=a;
      a=c;
      if (a) b=a->next; else b=NULL;
      } /* if we have 2 lists to compare */
    else { /* just one listing */
      if (tail) tail->next=a; else head=a;
      tail=a;
      a=NULL;
      }
    }  /* while we have lists to peruse */
  } /* while we have more than one list */
return(0);      
}





static inline process_input(char *buf,char *obuf, int fieldcount,int ifields_count, int xfields_count,char *keepbuf,int *plength,int keep_rest,int *pcols)
{
char *optr;
int cols=0;
int i;
last_index_position=index_position;
index_position += strlen(buf) + 1;

if (!fieldcount) {
  strcpy(obuf,buf);
  }
if (fieldcount || ifields_count || xfields_count) {
  if (keep_rest) {
    strcpy(keepbuf,buf);
    }
  /* split it out if we have to */
  char *q= buf;
  char *r = q;
  optr = obuf;
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
  
  if (!flag) return flag;  
  
  
  if (xfields_count) {
    flag=0;
    for (i=0;i<xfields_count;i++) {
      if (fpos[xfields[i]-1][0] == '\0') { /* if we are empty at this field */
        flag=1; /* we is a loadin this one */
        break;
        }
      }
    }    
  
  if (!flag) return flag;
  }
  
/* formulate the fields */
if (fieldcount) {   
  for (i=0;i<fieldcount;i++) {
    int pos = fields[i]-1;
    if (i) *optr++ = '|';
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
    }
  if (keep_rest) {
    *optr++ = '|'; /* field demiliter - should be a constant or a code or something. Oh well */
    strcpy(optr,keepbuf); /* add the original to tis baby */
    *plength = optr-obuf + strlen(keepbuf);
    }
  else {
    *optr='\0'; /* terminate */
    *plength = optr-obuf;
    }
  }
else *plength = strlen(obuf);  

/* Add the random number or the index position, or both */
if (add_random) {
  int r=random();
  if (add_index_position) {
    sprintf(obuf+ *plength,"|%010d|%lld",r,last_index_position);
    }
  else {
    sprintf(obuf+ *plength,"|%010d",r);
    }
  *plength += strlen(obuf+ *plength);
  }
else {
  if (add_index_position) {
    sprintf(obuf+ *plength,"|%lld",last_index_position);
    *plength += strlen(obuf+ *plength);
    }
  }
*pcols = cols;
return 1;
}




















int main (int argc, char *argv[]) {
struct entry_struct *c;
size_t total_memory = 0;
int all_in_memory_flag;
int sortflag=0;
char * tmp = getenv("TMP");
if (!tmp || (tmp[0]=='\0')) {
  tmp="/tmp";
  }
/* tmp2 is good for multiple passes,  such as if we are doing radix sort on multiple radicies 
so we read from one temp and write to another temp
We can also use this with the regular merge sort to distribute temp files on multiple disks
*/
char * tmp2 = getenv("TMP2");
if (!tmp2 ||(tmp2[0]=='\0')) tmp2=tmp;
char *tmps[2];
tmps[0]=tmp;
tmps[1]=tmp2;
int num_tmps = 2;

int i;
size_t maxsize;

int markerfields=-1;
int memposition=1;
int memfiles = 0;
int sort_type = 1; /* 1 for merge sort, 2 for radix sort */
int keep_rest = 0; /* add the rest to the field */
FILE *xfs[10000];
maxsize=7000000000;/* this could be bigger if we had a bigger machine */
all_in_memory_flag = 1;

fieldsize[0]=0;

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
  if (strncmp(argv[i],"-m",2)==0) { /* merge sort (default) */
    sort_type=1;
    continue;
    }
  if (strncmp(argv[i],"-q",2)==0) { /* add an id number */
    add_id=1;
    big_add_id=0;
    continue;
    }
  if (strncmp(argv[i],"-Q",2)==0) { /* add a big id number */
    big_add_id=1;
    add_id = 0;
    continue;
    }
  if (strncmp(argv[i],"-r",2)==0) { /* radix sort (default) */
    sort_type=2;
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
  if (strncmp(argv[i],"-l",2)==0) {
    int w = atoi(argv[i]+2);
    fieldsize[fieldcount] = w;
    continue;
    }
  if (strncmp(argv[i],"-k",2)==0) { /* keep the rest at the end */
    keep_rest = 1;
    continue;
    }
  w=atoll(argv[i]);
  if (w != 0) {
    if (w>=10000) {
      maxsize=w;
      }
    else {
      fields[fieldcount++] = w;
      fieldsize[fieldcount] = 0;
      }
    }
  } /* while parsing arguments */
if (!fieldcount) keep_rest = 0; /* doesn't make sense if we are sorting the whole damn thing */

if (sort_type==1) {
  while(gets(buf) != NULL) {
    int length;
    int cols=0;
    int flag=process_input(buf,obuf,fieldcount,ifields_count,xfields_count,keepbuf,&length,keep_rest,&cols);
    if (!flag) continue;
  
    /* here obuf is set up */
    size_t new_size =  sizeof(struct merge_struct) + sizeof(struct entry_struct) + length+1;
    if (total_memory + new_size > maxsize) { /* too big */
      all_in_memory_flag = 0; /* switch modes */
      inmemory_sort(head,tail);
      sprintf(buf,"%s/temp%d_%d",tmps[memfiles % num_tmps],getpid(),memfiles); /* should open in tmp folder */
      FILE *xf = fopen(buf,"w");
      if (!xf) {
        fprintf(stderr,"error writing to %s %d\n",buf,errno);
        exit(-1);
        }
      xfs[memfiles] = xf; /* save the file descriptor, close at the end. :) */
      struct entry_struct *c;
      c=head->head;
      while (c) {
        struct entry_struct *cc = c;
        fprintf(xf,"%s\n",c->value);
        c=c->next;
        free(cc);
        }
      if (head) free(head);
      head = NULL;
      tail = NULL;
      memfiles++;
      total_memory=0;
      } /* if we had to flush to disk */
  
    c = malloc( sizeof(struct entry_struct) + length+1);
    if (!c) {
      fprintf(stderr,"Out of memory! darn!\n");
      exit(-1);
      }
    total_memory += sizeof(struct entry_struct) + length+1;
    c->value = (char *)(c+1);
    c->next = NULL;
  
    strcpy(c->value,obuf);
    
    /* now see if we should add to an existing run */
    if ((tail) && (tail->tail) && (strcmp(tail->tail->value,c->value)<=0)) {
      tail->tail->next = c;
      tail->tail = c;
      }
    else {
      struct merge_struct *cm;
      cm = malloc(sizeof(struct merge_struct));
      if (!cm) {
        fprintf(stderr,"Out of memory! darn!\n");
        exit(-1);
        }
      total_memory += sizeof(struct merge_struct);
      cm->head = c;
      cm->tail = c;
      cm->next = NULL;
      if (tail) {
        tail->next = cm;
        }
      else {
        head = cm;
        }
      tail = cm;
      } /* if we need a new run */
    }  

  int mc = 0;
  char prevmarkerstring[20000];
  char markerstring[20000];
  prevmarkerstring[0]='\0';

  if (all_in_memory_flag) {
    inmemory_sort(head,tail);
      if (head) {
        for (c=head->head; c!= NULL;) {
          struct entry_struct *cc = c;
	  if (add_id) {
            printf("%010d|%s\n",id++,c->value);
	    }
	  else if (big_add_id) {
	    printf("%013qd|%s\n",big_id++,c->value);
	    }
	  else {
            printf("%s\n",c->value);
	    }
          c=c->next;
          free(cc);
          }
        free(head);
        }
    }
  else {
    inmemory_sort(head,tail);
    sprintf(buf,"%s/temp%d_%d",tmps[memfiles % num_tmps],getpid(),memfiles); /* should open in tmp folder */
    FILE *xf = fopen(buf,"w");
    xfs[memfiles] = xf; /* save the file descriptor, close at the end. :) */
    if (head) {
      for (c=head->head; c!= NULL;) {
        struct entry_struct *cc = c;
        fprintf(xf,"%s\n",c->value);
        c=c->next;
        free(cc);
        }
      free(head);
      }
    head = NULL;
    tail = NULL;
    memfiles++;
  
    for (i=0;i<memfiles;i++) {
      fclose(xfs[i]); /* close them here, now, after we are done reading the input file
               This is so that we don't wait for the slow io of writing the temp files
	       if we are reading from the source file, which usually is on another
	       disk device. */
      }
  
  
    /* OK, now lets merge these files together */
    FILE *files[10000];
    char *bufs[10000]; 
    int counts[10000];
    for (i=0;i<memfiles;i++) {
      sprintf(buf,"%s/temp%d_%d",tmps[i % num_tmps],getpid(),i);
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
	    int l=strlen(buf);if (l && (buf[l-1]=='\n')) buf[l-1]='\0';
            strcpy(bufs[i],buf);
	    }
	  else {
	    fclose(files[i]);
            files[i]=NULL;
            sprintf(buf,"%s/temp%d_%d",tmps[i % num_tmps ],getpid(),i); /* should open in tmp folder */
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
   	        int l=strlen(buf);if (l && (buf[l-1]=='\n')) buf[l-1]='\0';
	        strcpy(bufs[i],buf);
	        }
	      else {
                fclose(files[i]);
                files[i]=NULL;
                sprintf(buf,"%s/temp%d_%d",tmps[i % num_tmps] ,getpid(),i); /* should open in tmp folder */
	        remove(buf);
                }
	      }
            /* c>0 - ignore */
            }
          }
        if (lowest!=-1) {
          /* we got the lowest. print it out and sort again */ 
	  if (add_id) {
  	    printf("%010d|%s\n",id++,bufs[lowest]);
 	    }
	  else {
  	    printf("%s\n",bufs[lowest]);
	    }
          
	  if (fgets(buf,19999,files[lowest])) {
   	    int l=strlen(buf);if (l && (buf[l-1]=='\n')) buf[l-1]='\0';
	    strcpy(bufs[lowest],buf);
	    }
	  else {
            fclose(files[lowest]);
            files[lowest]=NULL;
            sprintf(buf,"%s/temp%d_%d",tmps[lowest % num_tmps],getpid(),lowest); /* should open in tmp folder */
	    remove(buf);
            }
 	  }
        else {
          break; /* need to fill more lines */
	  }
        } /* while */ 
      } /* block */
    } /* if we could not do it all in memory */  
  } /* if we are merge sort */
else if (sort_type==2) { /* simple one pass radix sort */
  struct radix_struct *radix_hash = NULL;
  int radix_count = 0;
  if (fieldcount != 1) {
    fprintf(stderr,"Sorry, the -r radix sort option only works with one field,  at least for now...\n");
    exit(-1);
    }
  if (keep_rest) {
    fprintf(stderr,"Sorry, the -k option to keep the rest does not make sense if -r is on.  -r returns the original text string, and does not have the fields specified anyways.\n");
    exit(-1);
    }  
  while(gets(buf) != NULL) {
    int length;
    int cols=0;
    strcpy(keepbuf,buf);
    int flag=process_input(buf,obuf,fieldcount,ifields_count,xfields_count,keepbuf,&length,keep_rest,&cols); 
    if (!flag) continue;
    struct radix_struct *c;
    /* we use buf for the input, and obuf for the output - different than merge sort */
    HASH_FIND_STR(radix_hash,obuf,c);
    if (!c) {
      /* new file */
      char value[20000];
      char fname[20000];
      strcpy(value,obuf);
      sprintf(fname,"%s/temp%d_%d",tmps[radix_count%num_tmps],getpid(),radix_count);
      FILE *xf = fopen(fname,"w");
      if (!xf) {
        fprintf(stderr,"error writing to %s %d\n  This means you have too many possible values for the sort - keep it under 1000 buddy. Code is %s!",fname,errno,value);
        exit(-1);
        }
      c = malloc(sizeof(struct radix_struct) + (strlen(value) + 1 + strlen(fname) + 1) * sizeof(char));
      if (!c) {
        fprintf(stderr,"Out of memory! darn!\n");
        exit(-1);
        }
      c->value = (char *)(c+1);
      c->fs = xf;
      c->fname = c->value + strlen(value) + 1;
      strcpy(c->value,value);
      strcpy(c->fname,fname);
      HASH_ADD_KEYPTR(hh,radix_hash,c->value,strlen(value),c);
      radix_count++;
      }
    fprintf(c->fs,"%s\n",keepbuf);
    } /* while we have rows to sort */
  HASH_SORT(radix_hash,radix_key_sort);
  struct radix_struct *c;

  for (c=radix_hash;c!=NULL;c=c->hh.next) {
    fclose(c->fs);
    c->fs = NULL;
    }
  for (c=radix_hash;c!=NULL;c=c->hh.next) {
    FILE *xf=fopen(c->fname,"r");
    if (!xf) {
      fprintf(stderr,"cannot openfile %s. Bummer\n",c->fname);
      }
    while (fgets(buf,19999,xf)) {
      if (add_id) {
        printf("%010d|%s",id++,buf);
 	}
      else {
        fputs(buf,stdout);
	}
      }
    fclose(xf);
    remove(c->fname);

    } /* for each temp file */
  } /* if we are real simple radix sort */
return(0);
}



