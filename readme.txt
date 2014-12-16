
$Revision$



bar_database is a set of NoSQL routines used to process and generate the fake names used on xdd2.org, xdduk.org, and other sites.

This works with flat text files (unix format) with the vertical bar as the sentinnel between columns and a line field between each line.

The reason that this exists is for speed of processing.  In order to create 260 million fake names for the United States,  the general purpose
databases did not cut it.  By switching to unix stream based processing,  the work became acceptably fast.

I decided to publish these as open source, because there are probably many more uses, especially for the very quick sorting program: fsort

Licensing is GPL 2.0 for stand alone commands, and LGPL for libraries.

Hibbard M. Engler
12/16/2014



bin/add_id
bin/clip_to_1_char
bin/combine_pipes
bin/dos2unix
bin/field      select specific fields from a stream
bin/fieldu     select unique set of specific fields from a stream
bin/fielduc    select unique set of specific fields, and count the number of rows with this unique set in a stream
bin/find_duplicate_records  find duplicated records between two files.
bin/flip_flop  hydra out lines into multiple streams.  This is used to share the load amonst many processes.
bin/fsort      field sort - perform a sort.  Most sorting is done with the merge sort.  This process can use a large chunk (multiple gigabytes) of memory
               which speeds up sorting signifigantly.  By signifigant, I mean that 100 million rows (2011 fast hardware) will sort in 5 minutes versus 
	       1 hour with the standard unix sort.
bin/give_new_id_number
bin/is_it_filled_in
bin/match      match fields to fields in a sorted bar_database text file.  Positive matches stream to stdout, where negative matches stream to stderr
bin/multiproc.sh  Split a job into 6 processes
bin/randomize     randomize a file based on a starting seed number.  
bin/set_field
bin/set_id
bin/set_number_of_fields

lib/:
bsearch.o   binary search algorithms that use virtual memory and memory mapping; thereby being extremely efficient.
util.o

include
uthash.h    from  http://uthash.sourceforge.net,  nice macro based hash routines used in many of these tools.




Caveats:
This was written for linux/mac OSX 10.5.  I dont have the Configure make crap in here.

Most commands are not buffer overflow safe - except the bsearch.c library which is used in production environemnts.  This could be fixed cleaned up etc.

This is NOT the recipe for making the fake names.  That takes a bit more do-diligence folks.  I might release this in the future under GPL, which uses 
this set of stuff as the core.  Also,  there will be others doing some related work, which will be GPL from the get go.


Log:
$Log$
Revision 1.1  2014/12/16 17:34:59  hib
Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
libraries (.o files). Yo!

