#include "bsearch.h"
#include <unistd.h>
#include <sys/stat.h>

/* V1.3 Added sup and super searchers

V1.2 Jul 20 - added the ability to search and traverse through multiple selections
*/


static size_t read_big(int fd,void *buf, size_t nbytes) {  
size_t retval=nbytes;
while(nbytes>1000000000) {
  size_t cnt=read(fd,buf,1000000000);
  if (cnt==0) {
    perror("Error reading file\n");
    exit(EXIT_FAILURE);
    }
  nbytes = nbytes - cnt;
  buf += cnt;
  }
if (nbytes)
  retval = read(fd,buf,nbytes);
return(retval);
}


struct searcher *new_searcher(char *filename,int searchfields,int populate,int locked) {
  struct searcher *s;
  s = malloc(sizeof(struct searcher));
  s->filename = strdup(filename);
    int flags;
    int i;
    int fd;
    char *map;  /* mmapped array of int's */
    size_t size;
    s->fd = fd = open(filename, O_RDONLY);
    if (fd == -1) {
	fprintf(stderr,"Error opening file %s for reading\n",filename);
	exit(EXIT_FAILURE);
    }
    struct stat statbuf;
    fstat(fd,&statbuf);
    s->size = size = statbuf.st_size;
/*
    if (populate) {
      s->map= map = malloc(size);
      if (!map) {
//        fprintf(stderr,"Error memory\n");
	exit(0);
	}
      read_big(fd,map,size);      
      close(fd);

      }    
    else */
      {
      flags = MAP_SHARED;
      if (locked) flags |= MAP_LOCKED;
//      if (populate) flags |= MAP_POPULATE; /*??? why are flags here if not used ? */
      s->map = map = mmap(NULL,size, PROT_READ,MAP_SHARED,fd, 0);
      if (map == MAP_FAILED) {
  	  close(fd);
	  fprintf(stderr,"Error mmapping the file %s\n",filename);
	  exit(EXIT_FAILURE);
      }
      madvise(map,size,MADV_RANDOM); /* added dontfork so system calls will work
                because when we do a system call the fork was running out of memory
		But THEN removed it again, because it was making the system not cache memory so much
		At least I think it was.
		  */
      if (populate) {
        long *x;
	long isize = size / sizeof(long);
	x = (long *)s->map;
	long i;
	long j;
	j=0;
	for (i=0;i<isize;i+=512) {
	  j += x[i];
	  }
	}
	
      }
    
    s->searchfields=searchfields;
    
    if (locked) {
      int x=mlock(map,size);
      if (x!= 0)
        {
//	fprintf(stderr,"error locking\n");
	}
      }
      return(s);
    }
    
int dealloc_searcher(struct searcher *s) {
    if (munmap(s->map, s->size) == -1) {
	perror("Error un-mmapping the file");
    }
    if (s->fd) close(s->fd);
    return 0;
}




int search(struct searcher *s,char *string,char *buf) { 
int searchfields = s->searchfields;
unsigned long long hi = s->size;
unsigned long long lo = 0;
char *x = s->map;
char *y=NULL;
long long l=0;

while (hi>lo) {
  int curfields=0;
  unsigned long long mid=(hi+lo)>>1;
  unsigned long long realmid=mid;
  l=0; /* find the beginning of the line */
  while (x[realmid] != '\n' && (realmid)) realmid--;
  if (realmid) realmid++; /* start just after the last new line */
  if (realmid <lo) {
    /* we are too low.  Go up one */
    while (x[realmid] && x[realmid] != '\n') realmid++;
    if (x[realmid]) realmid++;
    }
  if ((realmid >= hi)||(lo==hi)) {
    char *z = x+realmid;
    while (z[l] && (z[l] != '\n') ) {
        buf[l] = z[l];
        l++;
        }
    buf[l]='\0';
    return(0);
    }
  
  y = x+realmid;
  while ((curfields < searchfields) &&y[l] && (y[l] != '\n') ) {
    if ((y[l] == '|')||(y[l]=='	')) {
      curfields++;
      }
    buf[l] =y[l]; 
    l++;
    }
  buf[l]='\0';
  
  int flag=strcmp(string,buf);
  if (flag==0) {
    while (y[l] && (y[l] != '\n') ) {
      buf[l] = y[l];
      l++;
      }
    buf[l]='\0';
    return 1;
    }
  if (flag>0) {
    lo=realmid+l+1;
    }
  else {
    hi = realmid;
    }
  }
/* clean it up even if we don't match. learned this in the uk phone system
for the very first record match */
if (y) {
  while (y[l] && (y[l] != '\n') ) {
    buf[l] = y[l];
    l++;
    }
  buf[l]='\0';
  }
  
return(0);
}






/* search_first is good if there are duplicates - like in elaborate_houses
then we can do next. to search_next to find the next one that is equal -
note that search_first could be for a range scan,  but search_next 
is not set up for that 
*/
int search_first(struct searcher *s,char *string,char *buf,unsigned long long *nextline) { 
int searchfields = s->searchfields;
unsigned long long hi = s->size;
unsigned long long lo = 0;
char *x = s->map;
int found=0;
unsigned long long fl=0;

while (hi>lo) {
  int curfields=0;
  unsigned long long mid=(hi+lo)>>1;
  unsigned long long realmid=mid;
  long long l=0; /* find the beginning of the line */
  while (x[realmid] != '\n' && (realmid)) realmid--;
  if (realmid) realmid++; /* start just after the last new line */
  if (realmid <lo) {
    /* we are too low.  Go up one */
    while (x[realmid] && x[realmid] != '\n') realmid++;
    if (x[realmid]) realmid++;
    }
  if ((realmid >= hi)||(lo==hi)) {
    if (found) break;
    char *z = x+realmid;
    while (z[l] && (z[l] != '\n') ) {
        buf[l] = z[l];
        l++;
        }
    buf[l]='\0';
    return(0);
    }
  
  char *y = x+realmid;
  while ((curfields < searchfields) &&y[l] && (y[l] != '\n') ) {
    if ((y[l] == '|')||(y[l]=='	')) {
      curfields++;
      }
    buf[l] =y[l]; 
    l++;
    }
  buf[l]='\0';
  
  int flag=strcmp(string,buf);
  if (flag==0) {
    found=1;
    fl = realmid;
    hi = realmid; /* go less in case we have more like us  - then we go beyond it */
    }
  else if (flag>0) {
    lo=realmid+l+1;
    }
  else {
    hi = realmid;
    }
  }
if (found) {   
  char *y = x + fl;
  long long l = 0;
  while (y[l] && (y[l] != '\n') ) {
    buf[l] = y[l];
    l++;
    }
  buf[l]='\0';
  /* go beyond the new line if we have a new line */
  if (y[l]) l++;
  *nextline= fl + l;
  return 1;
  }  
return 0;
}




int search_next(struct searcher *s,char *string,char *buf,unsigned long long *nextline) { 
int searchfields = s->searchfields;
char *x = s->map;
int found=0;
unsigned long long fl= *nextline;
if (x[fl]=='\0') return(0);

long long l=0; /* find the beginning of the line */
int curfields=0;
char *y = x+fl;
while ((curfields < searchfields) &&y[l] && (y[l] != '\n') ) {
  if ((y[l] == '|')||(y[l]=='	')) {
    curfields++;
    }
  buf[l] =y[l]; 
  l++;
  }
buf[l]='\0';
  
int flag=strcmp(string,buf);
if (flag==0) {
  y = x+fl;
  while (y[l] && (y[l] != '\n') ) {
    buf[l] = y[l];
    l++;
    }
  buf[l]='\0';
  if (y[l]) l++;
  *nextline= fl + l;
  return 1;
  }
return 0;
}




int search_first_range
     (struct searcher *s,char *string,char *buf,unsigned long long *nextline) {
return(search_first_range_over(s,string,buf,nextline,0));
}     




/* search_first_range will get the value that is on or after the current value
*/
int search_first_range_over
     (struct searcher *s,char *string,char *buf,unsigned long long *nextline,int skip_fields) { 
int searchfields = s->searchfields;
unsigned long long hi = s->size;
unsigned long long lo = 0;
char *x = s->map;
int found=0;
unsigned long long fl=0;


while (hi>lo) {
  int curfields=0;
  unsigned long long mid=(hi+lo)>>1;
  unsigned long long realmid=mid;
  long long l=0; /* find the beginning of the line */
  while (x[realmid] != '\n' && (realmid)) realmid--;
  if (realmid) realmid++; /* start just after the last new line */
  if (realmid <lo) {
    /* we are too low.  Go up one */
    while (x[realmid] && x[realmid] != '\n') realmid++;
    if (x[realmid]) realmid++;
    }
  if ((realmid >= hi)||(lo==hi)) {
    if (found) break;
    fl = realmid;
    char *z = x+realmid;
    while (z[l] && (z[l] != '\n') ) {
        buf[l] = z[l];
        l++;
        }
    buf[l]='\0';
    if (z[l]) l++;
    *nextline = fl + l;
    return(0);
    }
  
  char *y = x+realmid;
  while ((curfields <skip_fields) &&y[0] && (y[0] != '\n')) {
    if ((y[0] == '|')||(y[0]=='	')) {
      curfields++;
      }    
    y++;
    }
  curfields=0;
  while ((curfields < searchfields) &&y[l] && (y[l] != '\n') ) {
    if ((y[l] == '|')||(y[l]=='	')) {
      curfields++;
      }
    buf[l] =y[l]; 
    l++;
    }
  buf[l]='\0';

  
  int flag=strcmp(string,buf);
  if (flag==0) {
    found=1;
    fl = realmid;
    hi = realmid; /* go less in case we have more like us  - then we go beyond it */
    }
  else if (flag>0) {
    lo=realmid+l+1;
    }
  else {
    fl = realmid; /* set fl to just after the field*/
    hi = realmid;
    }
  }
char *y = x + fl;
long long l = 0;
while (y[l] && (y[l] != '\n') ) {
  buf[l] = y[l];
  l++;
  }
buf[l]='\0';
/* go beyond the new line if we have a new line */
if (y[l]) l++;
*nextline= fl + l;
return found;
}






int search_next_range
(struct searcher *s,char *string,char *buf,unsigned long long *nextline) { 
return search_next_range_over(s,string,buf,nextline,0);
}




int search_next_range_over
(struct searcher *s,char *string,char *buf,unsigned long long *nextline,int skip_fields) { 
char *x = s->map;
int found=0;
unsigned long long fl= *nextline;
if (x[fl]=='\0') {
  buf[0]='\0';
  return(0);
  }
long long l=0; /* find the beginning of the line */
char *y = x+fl;
while (y[l] && (y[l] != '\n') ) {
  buf[l] = y[l];
  l++;
  }
buf[l]='\0';
if (y[l]) l++;
*nextline= fl + l;
return 1;
}








static int file_exists (char * fileName)
{
   struct stat buf;
   int i = stat ( fileName, &buf );
     /* File found */
     if ( i == 0 )
     {
       return 1;
     }
     return 0;
       
}













struct sup_searcher *new_sup_searcher(char *filename,int searchfields,int populate,int locked) {
struct searcher *searcher;
searcher = new_searcher(filename,searchfields,populate,locked);
if (!searcher) return NULL;

struct sup_searcher *s;
s = malloc(sizeof(struct sup_searcher));
s->base = searcher;
s->number_of_searcher_letters=0;
int skipped=0;
int i=1;
while (skipped<4) {
  char fname[20000];
  sprintf(fname,"%s.l%02d",searcher->filename,i);
  if (!file_exists(fname)) { skipped++; i++; continue;}
  
  int x=s->number_of_searcher_letters;
  struct searcher *searcher;
  searcher =  new_searcher(fname,searchfields+1,populate,locked);
  if (!searcher) { skipped++;i++; continue;}
  s->searcher_letters[x] = i;
  s->sub_searcher[x]= searcher;
  s->number_of_searcher_letters = s->number_of_searcher_letters + 1;
  skipped=0;
  i++;
  }
return s;
}
    













struct super_searcher *new_super_searcher(char *filename,int searchfields,int populate,int locked) {
struct searcher *searcher;
searcher = new_searcher(filename,searchfields,populate,locked);
if (!searcher) return NULL;

struct super_searcher *s;
s = malloc(sizeof(struct super_searcher));
s->base = searcher;
s->number_of_sup_searcher_dividers=0;
int i=0;
int skipped=0;
while (skipped<4) {
  char fname[20000];
  sprintf(fname,"%s.m%02d",searcher->filename,i);
  if (!file_exists(fname)) { skipped++; i++; continue;}
  
  int x=s->number_of_sup_searcher_dividers;
  struct sup_searcher *sup_searcher;
  sup_searcher =  new_sup_searcher(fname,i,populate,locked);
  if (!sup_searcher) { skipped++; i++; continue; }
  s->searcher_dividers[x] = i;
  s->sub_sup_searcher[x]= sup_searcher;
  s->number_of_sup_searcher_dividers = s->number_of_sup_searcher_dividers + 1;
  
  skipped=0;
  i++;
  }
return s;
}
  






/* super_search_first - perform a search first, and provide the 
*/
int super_search_first_range_over(struct super_searcher *s,char *string,char *buf,unsigned long long *nextline,
int skip_fields,
int *p_supindex,int *p_searchindex,struct searcher **p_base_searcher,
struct sup_searcher **p_sup_searcher,int *pfieldnum,int *pletter_count) {
/* note - p_supindex will be -1 if there is no sup-index - i.e. we are going right from the base */ 
int searchfields = s->number_of_sup_searcher_dividers;
int fieldnum=0;
char *x = string;
int letter_count=0;
int skip_count=0;


while (*x) {
  if (*x == '|') {
    fieldnum++;
    letter_count=0;
    }
  else {
    letter_count++;
    }
  x++;
  }
*pfieldnum = fieldnum;
*pletter_count = letter_count;
/* OK, we know the field num and directiry count that we are AT
but we want the next level down from that.
So, first, we search for the given directory 
and see if there is a letter that is one level lower.
Then if that does not work,  See if there is a directory lower */

int i;
for (i=0;(i<searchfields);i++) {
  if (s->searcher_dividers[i] == fieldnum) {
    /* OK, we have the right number of fields - But there could be a subset */
    struct sup_searcher *sup;
    sup = s->sub_sup_searcher[i];
    int j;
    for (j=0;j<sup->number_of_searcher_letters;j++) {
      if (sup->searcher_letters[j] >= letter_count+1) {
        *p_supindex = i;
	*p_searchindex = j;
        *p_sup_searcher = sup;
	*p_base_searcher = sup->sub_searcher[j];
	return  search_first_range_over(*p_base_searcher,string,buf,nextline,skip_fields);
	}
      }
    /* bummer - we did not find our letter directory.  We will use the directory below.
      then we will go through the directory only section */
    break;
    } /* If we have found our field number */
  }

/* OK - no letter count here.  Lets find the field number that is greater than our current one.
*/
for (i=0;(i<searchfields);i++) {
  if (s->searcher_dividers[i] >= fieldnum+1) { /* this index applies */
    *p_supindex = i;
    *p_searchindex = -1;
    *p_sup_searcher =  s->sub_sup_searcher[i];
    /* OK, if we do not have trailing letters, we will use a letter search, if it exists,  else we will use the base 
      off the trailing letters */
    if ((letter_count==0)&&((*p_sup_searcher)->number_of_searcher_letters))
      *p_base_searcher = (*p_sup_searcher)->sub_searcher[0];
    else
      *p_base_searcher = (*p_sup_searcher)->base;
    return search_first_range_over(*p_base_searcher,string,buf,nextline,skip_fields);
    }
  }
/* here we did not find any subfield searches that will help */
*p_supindex = -1;
*p_searchindex = -1;
*p_sup_searcher =  NULL;
*p_base_searcher = s->base;
return search_first_range_over(*p_base_searcher,string,buf,nextline,skip_fields);
}
