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


randomize.c $Revision$
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>

/* randomize - similar to knuths method,  but different.
This also handles trillions of rows if necessary
*/




  char is_used[100000000];

int rdump(char *lines[],int nlines,FILE *xf,int free_things) {
while (nlines) {
  
  memset(is_used,0,100000000*sizeof(char));
  
  // special case - just one record. Print it out and we are done.
  if (nlines==1) {
    fputs(lines[0],xf);
    fputc('\n',xf);
    if (free_things) free(lines[0]);
    nlines=0;
    break;
    }

  int limit=nlines/2;
  int i;
  // lets do about half of them.  pick out random cards. If they are already picked, pick someone else.  
  for (i=0;i<limit;i++) {
    int x;
    do {
      x = (random()>>2) % nlines;
      } while (is_used[x]);
    fputs(lines[x],xf);
    fputc('\n',xf);
    if (free_things) free(lines[x]);
    is_used[x] = 1;
    }
  
  // ok, now the deck of "cards" is thinner.  Lets shuffle these remainders randomly, without worrying about the used cards.
  // our deck is smaller.
  int j=0;
  for (i=0;i<nlines;i++) {
    if (!is_used[i]) {
      lines[j++] = lines[i];
      }
    } 
  nlines = j; 
  } // while we have some "cards" to shuffle
}










char *lines[100000000];

int main (int argc, char *argv[]) {
long long total_count;
long long fullsize_count;
char buf[20000];
struct entry_struct *c;
size_t total_memory = 0;
int all_in_memory_flag;

total_count=0;
fullsize_count=0;
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

int nlines;
int cursize;

int markerfields=-1;
int memposition=1;
int memfiles = 0;
FILE *xfs[10000];
maxsize=500000000;/* this could be bigger if we had a bigger machine */
all_in_memory_flag = 1;



for (i=1;i<argc;i++) {
  long long w=0;
  srandom(atoi(argv[i]));
  } /* while parsing arguments */


total_memory=0;
nlines=0;
while(gets(buf) != NULL) {
    int ok=1;
    int l=strlen(buf);
    if (nlines>=100000000) ok=0;
    else if (total_memory + (l+1) > maxsize) ok=0;
    lines[nlines] = (char *)malloc((l+1)*sizeof(char));
    if (lines[nlines]==NULL) ok=0;
    
    if (!ok) {
      char fname[20000];
      all_in_memory_flag = 0; /* switch modes */
      sprintf(fname,"%s/temp%d_%d",tmps[memfiles % num_tmps],getpid(),memfiles); /* should open in tmp folder */
      FILE *xf = fopen(fname,"w");
      if (!xf) {
        fprintf(stderr,"error writing to %s %d\n",fname,errno);
        exit(-1);
        }
      xfs[memfiles] = xf; /* save the file descriptor, close at the end. :) */
      rdump(lines,nlines,xf,1);
      fullsize_count=total_count;
      memfiles++;
      total_memory=0;
      nlines=0;
      lines[nlines] = malloc((l+1)*sizeof(char));
      if (lines[nlines]==NULL) { fprintf(stderr,"memory bad! bummer\n");}      
      } /* if we had to flush to disk */
    
    strcpy(lines[nlines],buf);
    total_memory += l+1;
    nlines++;
    total_count++;
    }
        

if (all_in_memory_flag) {
  rdump(lines,nlines,stdout,0);
  }
else {
  char fname[20000];
  sprintf(fname,"%s/temp%d_%d",tmps[memfiles % num_tmps],getpid(),memfiles); /* should open in tmp folder */
  FILE *xf = fopen(fname,"w");
  if (!xf) {
    fprintf(stderr,"error writing to %s %d\n",fname,errno);
    exit(-1);
    }
  xfs[memfiles] = xf; /* save the file descriptor, close at the end. :) */
  rdump(lines,nlines,xf,1); /* free up for kernel buffers yeah!*/
  memfiles++;
  
  for (i=0;i<memfiles;i++) {
    fclose(xfs[i]); /* close them here, now, after we are done reading the input file
               This is so that we don't wait for the slow io of writing the temp files
	       if we are reading from the source file, which usually is on another
	       disk device. */
    }
  /* OK, now lets read from these files randomly */
  FILE *files[10000];
  for (i=0;i<memfiles;i++) {
    char fname[20000];
    sprintf(fname,"%s/temp%d_%d",tmps[i % num_tmps],getpid(),i);
    files[i]=fopen(fname,"r");
    if (!files[i]) {
      fprintf(stderr,"error: cannot open temp file %s\n",fname);
      exit(-1);
      }
    }
  /* get the first lines of all files for comparison */
  int done=0;
  int fclosed=0;
  while (!done) {
    long int rr= random() % total_count;
    int r;
    if (rr<fullsize_count) {
      r= (int)(    (rr * (memfiles-1)) / fullsize_count );
      }
    else r=  memfiles-1; 

    if (files[r]) {
      if (fgets(buf,19999,files[r])) {
        fputs(buf,stdout);
        }
      else {
        char fname[20000];
        fclose(files[r]);
        files[r]=NULL;
        sprintf(fname,"%s/temp%d_%d",tmps[r % num_tmps ],getpid(),r); /* should open in tmp folder */
        remove(fname);
	fclosed++;
	if (fclosed==memfiles) done=1; 
        }
      } /* try again - minor slowdown */
    } /* while reading and writing to the file randomly */
  } /* if we are not all in memory */
return(0);
}



