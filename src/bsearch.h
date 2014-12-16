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

bsearch.h - header for binary search routines for a bar delimited text file.

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>


/* searcher is a sorted text file - we can do binary searches on it */
struct searcher {
  char *filename;
  int fd;
  size_t size;
  int searchfields;
  char *map;
  };
  

  
  
/* sup_searcher allows searchers to work where there are partial letters set up
This is good to fing the names that start with "F" for  example 
This requires several derives sorted files to be made - with the given subset listed.
And the name will be .lnn - which is letter (number of letters)
When we open the base searcher,  we will also open the .lnn files as needed
*/
struct sup_searcher {
  struct searcher *base;
  int number_of_searcher_letters;
  int searcher_letters[100];
  struct searcher *sub_searcher[100];
  };

/* super searcher is made for parial indexes of specific fields.
It builds on sup_searcher, But in this case is looking for .fnn files
So, yes, you can have .fnn files and .lnn files
And,  you can have a .f00 file - which basically has the entire count */  
struct super_searcher {
  struct searcher *base;
  int number_of_sup_searcher_dividers;
  int searcher_dividers[100];
  struct sup_searcher *sub_sup_searcher[100];
  };
  
  
  
struct searcher *new_searcher(char *filename,int searchfields,int populate,int locked) ;
int dealloc_searcher(struct searcher *s) ;
int search(struct searcher *s,char *string,char *buf) ;
int search_first(struct searcher *s,char *string,char *buf,unsigned long long *nextline);
int search_next(struct searcher *s,char *string,char *buf,unsigned long long *nextline);
int search_first_range(struct searcher *s,char *string,char *buf,unsigned long long *nextline);
int search_next_range(struct searcher *s,char *string,char *buf,unsigned long long *nextline);

int search_first_range_over(struct searcher *s,char *string,char *buf,
unsigned long long *nextline,int skip_fields);
int search_next_range_over(struct searcher *s,char *string,char *buf,unsigned long long *nextline,int skip_fields);

static size_t read_big(int fd,void *buf, size_t nbytes);



struct sup_searcher *new_sup_searcher(char *filename,int searchfields,int populate,int locked);



struct super_searcher *new_super_searcher(char *filename,int searchfields,int populate,int locked);

int super_search_first_range_over(struct super_searcher *s,char *string,char *buf,unsigned long long *nextline,
int skip_fields,
int *p_supindex,int *p_searchindex,struct searcher **p_base_searcher,
struct sup_searcher **p_sup_searcher,int *pfieldnum,int *pletter_count);
