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

combine_pipes.c $Revision$

$Log$
Revision 1.2  2014/12/16 17:34:59  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!


*/
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include <linux/tcp.h>



/* note - look at the following web site for performance tips

http://www.ibm.com/developerworks/linux/library/l-hisock.html
*/


#define READ_COUNT 4096
/* read count was 2048 but I upped it at one time now back to 2048 */

#define MAX_SOURCES 10000
#define MAX_POLLS 131071
/* max polls = max_sockets *2+1 + 10 for fudge
+1 because of listening on main port.
*2 because one poll for the socket,  one poll for the file/pipe/whatever */



/* action is a generic action that requires work */
struct action {
FILE *file; /* file name if it exists */
int filedes; /* file descriptor */
int poll_position;
char *buf1; /* buffer to read in */
int buf1_length;
char *buf2;
int buf2_length;
int mode;  /* 
for read mode of 0 - havent started IO.
              mode of 1 - started IO
	      mode of 2 - finished IO
	      mode of 3 - about to finish IO
1 - ready to write out something
0 - also ready to write out something
*/
};


int file_des[MAX_SOURCES];
int no_url_sources=0;


struct action actions[10000];
int no_actions=0;


struct pollfd poll_fds[10000];
int no_poll_fds=0;

void d(char *x) {
fprintf(stderr,"%s\n",x);
}











int add_to_poll (int fd, FILE *f,
 short eventflags,
 char *buf1,
 int buf1_length,
 char *buf2,
 int buf2_length)
{
if (!fd) fd = fileno(f);
poll_fds[no_poll_fds].fd = fd;
poll_fds[no_poll_fds].events=eventflags;
poll_fds[no_poll_fds].revents=0;

{
  struct action *a;
  a=actions + fd;
  a->file=f;
  a->filedes=fd;
  a->poll_position=no_poll_fds;
  a->buf1=buf1;
  a->buf1_length=buf1_length;
  a->buf2=buf2;
  a->buf2_length=buf2_length;
  a->mode=0;
  read_me(a);
  }
no_poll_fds++;
if (no_actions<=fd) no_actions=fd+1;
}















void print_polls (FILE *fd) 
{
int i;
fprintf(fd,"%d polls\n",no_poll_fds);
for (i=0;i<no_poll_fds;i++) {
  struct pollfd *f;
  struct action *a;
  f=poll_fds+i;
  a = actions+f->fd;
  fprintf(fd,"  Poll %d (",i);
  {
    short flag;
    flag=f->events;
    if (flag&POLLIN) fprintf(fd,"POLLIN ");
    if (flag&POLLPRI) fprintf(fd,"POLLPRI ");
    if (flag&POLLOUT) fprintf(fd,"POLLOUT ");
    if (flag&POLLERR) fprintf(fd,"POLLERR ");
    if (flag&POLLHUP) fprintf(fd,"POLLHUP ");
    if (flag&POLLNVAL) fprintf(fd,"POLLNVAL ");
  }
  fprintf(fd,") <");
  {
    short flag;
    flag=f->revents;
    if (flag&POLLIN) fprintf(fd,"POLLIN ");
    if (flag&POLLPRI) fprintf(fd,"POLLPRI ");
    if (flag&POLLOUT) fprintf(fd,"POLLOUT ");
    if (flag&POLLERR) fprintf(fd,"POLLERR ");
    if (flag&POLLHUP) fprintf(fd,"POLLHUP ");
    if (flag&POLLNVAL) fprintf(fd,"POLLNVAL ");
  }
  if (a->filedes==0) {fprintf(fd,"CLOSED!!!  ");}
  }
fprintf(fd,"end actions\n");
}






int read_me (struct action *read_a) {
  
int cnt;
cnt =READ_COUNT; /* usual amount to read in */
cnt = read(read_a->filedes,read_a->buf1,cnt,0); /* send it out */
if (cnt==-1) {fprintf(stderr,"error reading from media stream: %d\n",errno);}
else if (cnt==0) {
  int e=errno;
  if ((e==EAGAIN)||(e==EWOULDBLOCK)) {
    fprintf(stderr,"bmode3 %d\n",errno);
    read_a->mode=3;
    }
  else {
    read_a->mode=1; /* we are reading a buffer in now */
    }
  read_a->buf1_length = cnt;
  }
poll_fds[read_a->poll_position].events |= (POLLIN||POLLPRI);
return(0);
}

















char *gettotoken(char *buf,int *bptr,char token) {
char *line;
int s=*bptr;
if (!buf[*bptr]) return(NULL);

while ((buf[s])&&(buf[s] != token)) {
  s++;
  }

line = malloc(sizeof(char)*(s-*bptr+1));
if (!line) return (NULL);
strncpy(line,buf+*bptr,(s-*bptr));
line[(s-*bptr)] = '\0';
if ((buf[s])) s++;
*bptr=s;
return(line);
}

char *getaline(char *buf,int *bptr) {
return(gettotoken(buf,bptr,'\n'));
}








int poll_fds_restruct=0;







int close_poll(int fd) {
struct action *a;

a=actions+fd;

/* take out of our file descriptor polling list */
if (no_poll_fds==1) {
  no_poll_fds =0;
  }
else {
  poll_fds[a->poll_position]=poll_fds[no_poll_fds-1];
  /* Move the last poll position into the current spot */
  actions[poll_fds[no_poll_fds-1].fd].poll_position=a->poll_position;
  /* That top position poll position now points to the poll position we just had */
  /* so this moves an action */
  no_poll_fds--;
  }
poll_fds_restruct+=1; /* need to restructure the file descriptors -- after we are done going through them all */





/* clean up the action */
{
  close(fd);
  a->filedes=0;
  a->buf1=NULL;
  a->buf1_length=0;
  a->buf2=NULL;
  a->buf2_length=0;
  }


}






      
											  


/* Add to the buffer - if it has anything! 
pointer to buffer,  pointer to length, new buffer, new length 
Assumes that if there is a buffer,  it was malloced so we can free it.
*/
int add_buffer(char **b, int *pl,char *n, int nl) {
if (!(*b)) {
  *b = malloc(sizeof(char)*(nl+1));
  strncpy(*b,n,nl);
  *pl = nl;
  }
else { 
  char *r;
  r = malloc(sizeof(char)*(*pl + nl +2));
  strncpy(r,*b,*pl);
  strncpy(r+*pl,n,nl);
  *pl += nl;
  free(*b);
  *b = r;
  (*b)[*pl]='\0';
  }
}







int handle_flow (struct action *read_a) {
  {
  if (read_a->mode==2) /* read something and full buffer */
    {
    int cnt;
/*??? 
   cnt = send(write_a->filedes,
   read_a->buf1,read_a->buf1_length,MSG_DONTWAIT|MSG_NOSIGNAL); 
    send it out */
cnt=1;
    if (cnt==-1) {
      if (errno==EPIPE) {
	return;
        }
      else {fprintf(stderr,"error sending out %d\n",errno);
	return;
        }
      }
//    poll_fds[write_a->poll_position].events |= (POLLOUT);
    
    
    { /* swap buf1 and buf2 */
      char *temp;
      int t2;
      temp = read_a->buf2;
     read_a->buf2 = read_a->buf1;
     read_a->buf1=temp;
     t2=read_a->buf2_length;
     read_a->buf2_length=read_a->buf1_length;
     read_a->buf1_length=t2;
     }

    
//    write_a->mode=2; /* are writing things out now */
    
    read_me(read_a);
    }
  else if (read_a->mode==1) /* still reading */ {
    /* nothing right now.  Have to wait until the read is done */
//    poll_fds[write_a->poll_position].events &= ~(POLLOUT);
    }
  else if (read_a->mode==0) /* haven't started reading */ {
    read_me(read_a);
    } /* if need to read */
  else if (read_a->mode==3) {
//    close_poll(write_a->filedes);
    }
  } /* if we are ready to write some more */
}






int main(int argc,char *argv[]) {

int s;
int check;
struct sockaddr_in server_sa;

no_poll_fds=0;
for (s=0;s<argc-1;s++) {
  char *filename;
  int filedes;
  filename=argv[s+1];
  filedes = open(filename,O_RDONLY|O_NONBLOCK);
  if (filedes == -1) {
    fprintf(stderr,"cannot open %s: %d\n",filename,errno);
    exit(-1);
    }
  struct stat filestat;
  int test;
  test=fstat(filedes,&filestat);
  if (test) {
    fprintf(stderr,"error stating file %s: %d\n",filename,errno);
    exit(-1);
    /* should be a cannot access file or something like that */
    }
  if (no_url_sources >= MAX_SOURCES) {
    fprintf(stderr,"out of url_sources! ???\n");
    /* we should have an overloaded url source that is returned in this case - which is a special response. */
    exit(-1);
    }
  char *buf1;
  char *buf2;
  buf1=malloc(READ_COUNT);
  buf2=malloc(READ_COUNT);
  
  add_to_poll(filedes, /* file des */
        NULL,  /* File alternate */
        POLLIN|POLLPRI|POLLERR|POLLHUP, /* event flags */
	  buf1,READ_COUNT,buf2,READ_COUNT);
  }     


while (1) {  
  int no_events;
  int i;
  int number_poll_fds_local;
  no_events=poll(poll_fds,no_poll_fds,1222222);
  i=0;

  number_poll_fds_local=no_poll_fds;
  while ((no_events)&&(i<number_poll_fds_local)) {
    if (poll_fds[i].revents) {
      struct action *a;
      short revents;
      a=actions+ poll_fds[i].fd;
      revents=poll_fds[i].revents;
      handle_flow(a);
      if (poll_fds_restruct) /* push a forward 1 if we took out some events */ {
        number_poll_fds_local =- poll_fds_restruct; /* less to do */
        poll_fds_restruct=0;
        }
      else {
        i++; /* go to the next one */
	}
      no_events--;
      } /* if found an event */
    else {
      i++;
      }
    } /* while */
  } /* forever */
exit(0);
}




