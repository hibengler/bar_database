

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
