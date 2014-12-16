#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *gets(char *x) {
char *y;
y=fgets(x,19999,stdin);
if (y) {
  int a=strlen(x);
  if (a && x[a-1]=='\n') {
    x[a-1]='\0';
    }
  }
return y;
}

char *field(char *x, char *y,int fieldnum)
{
char *z =y;
while (fieldnum && *x) {
  if ((*x == '|')||(*x == '	')) fieldnum--;
  x++;
  }
while (*x && (*x != '|') && (*x != '	')) {
  *z++ = *x++;
  }
*z='\0';
return y;
}



char *fieldspan(char *x, char *y,int fromfieldnum,int tofieldnum,int withend)
{
char *z =y;
int i=0;
while (i<fromfieldnum && *x) {
  if ((*x == '|')||(*x == '	')) i++;
  x++;
  }
while (i<=tofieldnum && *x) {
  if ((*x == '|')||(*x == '	')) i++;
  if (i>tofieldnum&&(!(withend))) break; /* skip the end if requested */
  *z++ = *x++;
  }
*z='\0';
return y;
}

int fieldi(char *x,int fieldnum) {
while (fieldnum && *x) {
  if ((*x == '|')||(*x == '	')) fieldnum--;
  x++;
  }
return atoi(x);
}



char *temp_dir() {
char * tmp = getenv("TMP");
if (!tmp || (tmp[0]=='\0')) {
  tmp="/tmp";
  }
return tmp;
}






int add_fields(char *x, int numfields) {
int i;
for (i=0;i<numfields;i++) {
  while ((*x)&&(*x != '|')) {
    x++;
    }
  if (*x == '|') {
    x++;
    }
  else {
    *x++ = '|';
    *x = '\0';
    }
  }
}

/* this will clip or add extra | as needed 
better than add_fields */
char *set_number_of_fields(char *x,int number_of_fields)
{
char lastfield='|';
char *z=x;
char *target = x;
int i=0;
while (i<number_of_fields && *x) {
  if ((*x == '|')||(*x == '	')) {
    lastfield = *x;
    target = x;
    i++;
    }
  else {
    target = x+1;
    }
  x++;
  }
while (i<number_of_fields) {
  target = x;
  *(x++) = lastfield;
  i++;
  }
*target = '\0';
return z;
}







/* get the countfield at the end of a line:
blah|blah|blah|27 
Makes out into
blah|blah|blah
and pcount gets 27
*/
int getcount(char *out,char *in,int *pcount) {
char *number;
char *onumber;
number = in;
onumber = out;
while (*in) {
  if (*in == '|') {
    onumber = out;
    number = in+1;
    }
  *out++ = *in++;
  }
*onumber = '\0';
*pcount =atoi(number);
return(0);
}



/* get the 2 countfield2 at the end of a line:
blah|blah|blah|27|44
Makes out into
blah|blah|blah
and pcount1 gets 27
and pcount2 gets 44
*/
int get2counts(char *out,char *in,int *pcount1,int *pcount2,int *pfields) {
char *number1;
char *onumber1;
char *number2;
char *onumber2;
int fields=0;
number1 = NULL;
number2 = in;
onumber1 =NULL;
onumber2 = out;
while (*in) {
  if (*in == '|') {
    fields++;
    onumber1 = onumber2;
    onumber2 = out;
    number1 = number2;
    number2 = in+1;
    }
  *out++ = *in++;
  }
if (onumber1) {
  *onumber1 = '\0';
  if (number1) *pcount1 = atoi(number1);
  else *pcount1 = atoi(number2);
  fields--;
  }
fields--;
*onumber2 = '\0';
*pcount2 =atoi(number2);
*pfields = fields;
/* add a vertical bar - makes it easier to search right */
if (onumber1) {
  *onumber1++ ='|';
  *onumber1 = '\0';
  }
return(0);
}







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
int make_marker(char *source,char *dest, int markerfields) 
{
int i=0;
while ((i<markerfields)&&(*source)) {
  if (*source=='|') {
    i++;
    if (i==markerfields) break;
    }
  *dest++ = *source++;
  }
if (markerfields) *dest++ = '|';
*dest = '\0';
}







int date_to_mdy(char *date,int *pmonth,int *pday, int *pyear) {
int the_day;
char the_month[4];
int the_year;
int days;
int leaps;
the_month[3]='\0';
int month=0;
sscanf(date,"%d-%c%c%c-%d",&the_day,the_month,the_month+1,the_month+2,&the_year);

if (strcmp(the_month,"Jan")==0) 
  month=1;
else if (strcmp(the_month,"Feb")==0) 
  month=2;
else if (strcmp(the_month,"Mar")==0) 
  month=3;
else if (strcmp(the_month,"Apr")==0)
  month=4;
else if (strcmp(the_month,"May")==0) 
  month=5;
else if (strcmp(the_month,"Jun")==0) 
  month=6;
else if (strcmp(the_month,"Jul")==0)
  month=7;
else if (strcmp(the_month,"Aug")==0) 
  month=8;
else if (strcmp(the_month,"Sep")==0) 
  month=9;
else if (strcmp(the_month,"Oct")==0) 
  month=10;
else if (strcmp(the_month,"Nov")==0) 
  month=11;
else if (strcmp(the_month,"Dec")==0) 
  month=12;
*pmonth = month;
*pday = the_day;
*pyear = the_year;
}



static int dates[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
static int datesl[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
static char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                           "Jul","Aug","Sep","Oct","Nov","Dec"};


int date_to_code(char *date) {
int the_day;
char the_month[4];
int the_year;
int days;
int leaps;
the_month[3]='\0';
sscanf(date,"%d-%c%c%c-%d",&the_day,the_month,the_month+1,the_month+2,&the_year);
days = the_day;
int *dt;
days = days + 365 * (the_year - 1776);
leaps = (the_year - 1777) /4;
days += leaps;
if ((the_year - 1776) % 4 ==0) {
  dt = datesl;
}
else {
  dt = dates;
  }
int i;
i=0;
if (strcmp(the_month,"Jan")) {
  days += dt[i++];
  if (strcmp(the_month,"Feb")) {
    days += dt[i++];
    if (strcmp(the_month,"Mar")) {
      days += dt[i++];
      if (strcmp(the_month,"Apr")) {
        days += dt[i++];
        if (strcmp(the_month,"May")) {
          days += dt[i++];
          if (strcmp(the_month,"Jun")) {
            days += dt[i++];
            if (strcmp(the_month,"Jul")) {
              days += dt[i++];
              if (strcmp(the_month,"Aug")) {
                days += dt[i++];
                if (strcmp(the_month,"Sep")) {
                  days += dt[i++];
                  if (strcmp(the_month,"Oct")) {
                    days += dt[i++];
                    if (strcmp(the_month,"Nov")) {
                      days += dt[i++];
                      if (strcmp(the_month,"Dec")) {
  			fprintf(stderr,"Error month of date %s\n",date);
			exit(-1);
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
	          
return days;  

}


char * code_to_date(int code,char*date) {
int days;
int years4;
int years;
int *dt;
days = code;
years4 = days / (365 + 365 + 365 + 366);
days = days %  (365 + 365 + 365 + 366);
if (days <366) {
  years = years4*4 + days / 366;
  days = days % 366;
  dt = datesl;
  }
else {
  years = years4*4 + days / 365;
  days = days % 365;
  dt = dates;
  }
int i=0;
while (days >= dt[i]) {
  days -= dt[i];
  i++;
  }
years += 1776;
days++;
sprintf(date,"%d-%s-%d",days,months[i],years);
return(date);  
}






char * initcap(char *dest,char *src) {
char ch;
if (ch = *src) {
  if (ch >= 'a' && ch <='z') {
    ch = ch + 'A' - 'a';
    }
  *dest++ = ch;
  src++;
  }
while (ch= *src) {
  if (ch >= 'A' && ch <='Z') {
    ch = ch - 'A' + 'a';
    }
  *dest++ = ch;
  src++;
  } 
*dest = '\0';
return dest;
}  

char * lower(char *dest,char *src) {
char ch;
while (ch= *src) {
  if (ch >= 'A' && ch <='Z') {
    ch = ch - 'A' + 'a';
    }
  *dest++ = ch;
  src++;
  } 
*dest = '\0';
return dest;
}  


char * upper(char *dest,char *src) {
char ch;
while (ch= *src) {
  if (ch >= 'a' && ch <='z') {
    ch = ch + 'A' - 'a';
    }
  *dest++ = ch;
  src++;
  }
*dest = '\0';
return dest;
}  




char *decode_url(char *out,char *in,int max) {
int i=0;
char *x=in;
char *z=out;
while ((*x)&&(i<max)) {
  if ((x[0] == '%') && (((x[1]>='0')&&(x[1]<='9'))||((x[1]>='a')&&(x[1]<='f'))||
                      ((x[1]>='A')&&(x[1]<='F')))
		   && (((x[2]>='0')&&(x[2]<='9'))||((x[2]>='a')&&(x[2]<='f'))||
                      ((x[2]>='A')&&(x[2]<='F'))) ) {
    char buf[3];
    buf[0]=x[1];
    buf[1]=x[2];
    buf[2]='\0';
    unsigned int a;
    sscanf(buf,"%x",&a);
    if (a == '/') { // special case  - need to convert to |
      a = 001; // which will get swapped back.  Tricky!
      }
    *z = a;
    z++;
    x+=3;
    i+=3;
    }
  else {
    *(z++) = *(x++);
    i++;
    }
  }
*z = '\0';
return out;
}




char *encode_url(char *out,char *in,int max) {
int i=0;
char *x=in;
char *z=out;
while ((*x)&&(i<max)) {
  if (index("/$-_.+!*'(),abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",x[0])) {
    *z++ = *x++;
    i++;
    }
  else if (x[0]==001) { /* special because / is in the data file and ctrl-a is in our file */
    *z++='%';
    *z++ = '2';
    *z++ = 'F';
    x++;
    i++;
    }
  else {
    *z++='%';
    sprintf(z,"%02x",*x);
    x++;
    z+=2;
    i++;
    }
  }
*z = '\0';
return out;
}


char *encode_html(char *out,char *in, int max) {
int i=0;
char *x=in;
char *z=out;
while ((*x)&&(i<max)) {
  if (*x == '&') {
    strcpy(z,"&amp;");
    z += 5;
    }
  else if (*x == 001) {
     strcpy(z,"/"); /* special fusging ctrl-a to / */
     z += 1;
     }
  else if (*x == '<') {
     strcpy(z,"&lt;");
     z += 4;
     }
  else if (*x == '>') {
     strcpy(z,"&gt;");
     z += 4;
     }
  else {
    *z++ = *x;
    }
  i++;
  x++;
  }
*z='\0';
return out;
}
