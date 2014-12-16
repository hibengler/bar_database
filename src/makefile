#
# 
# before start do
# sudo apt-get install imagemagick libmagickcore-dev libmagickwand-dev 
#
# $Revision$
#
# $Log$
# Revision 1.5  2014/12/16 17:35:00  hib
# Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
# libraries (.o files). Yo!
#
# Revision 1.4  2014/12/16 16:10:54  hib
# forgot soundex
#
# Revision 1.3  2014/12/16 16:08:37  hib
# Getting closer to being complete
#
# Revision 1.2  2014/12/16 15:59:28  hib
# Cleanup - should be complete now
#
# Revision 1.1  2014/12/16 15:48:56  hib
# initial checkin for gpl
#
#

	ALLEXE =field fieldu fielduc match \
    set_id add_id is_it_filled_in find_duplicate_records \
    set_field fsort \
    clip_to_1_char \
    set_number_of_fields \
    randomize \
    give_new_id_number \
    flip_flop \
    multiproc.sh \
    combine_pipes \
    dos2unix 



	ALLINC = bsearch.h    utarray.h  uthash.h  util.h  utlist.h  utstring.h

	ALLLIB = bsearch.o util.o 


#    verify_captcha.pl \

system: allexe.flag allinc.flag alllib.flag

allexe.flag: 	$(ALLEXE) 
	chmod 755 *.sh 
	cp $(ALLEXE) ../bin
	touch allexe.flag

alllib.flag: 	$(ALLLIB) 
	cp $(ALLLIB) ../lib
	touch alllib.flag

allinc.flag: 	$(ALLINC) 
	cp $(ALLINC) ../include
	touch allinc.flag

clip_to_1_char: clip_to_1_char.c util.o 
	gcc -O3 -o clip_to_1_char clip_to_1_char.c util.o bsearch.o

fsort: fsort.c util.o
	gcc  -O3 -o fsort fsort.c util.o

set_field: set_field.c util.o
	gcc  -O3 -o set_field set_field.c util.o
	
find_duplicate_records: find_duplicate_records.c util.o
	gcc  -O3 -o find_duplicate_records find_duplicate_records.c util.o

is_it_filled_in: is_it_filled_in.c util.o
	gcc  -O3 -o is_it_filled_in is_it_filled_in.c util.o

set_id: set_id.c util.o
	gcc  -O3 -o set_id set_id.c util.o

add_id: add_id.c 
	gcc  -O3 -o add_id add_id.c 

match: match.c util.o bsearch.o
	gcc  -O3 -o match match.c util.o bsearch.o

field: field.c util.o
	gcc  -O3 -o field field.c util.o
	
fieldu: fieldu.c uthash.h util.o
	gcc  -O3 -o fieldu fieldu.c util.o
	
fielduc: fielduc.c uthash.h util.o
	gcc   -O3 -o fielduc fielduc.c util.o
	

set_number_of_fields:	set_number_of_fields.c util.o bsearch.o
	gcc -O3 -o set_number_of_fields set_number_of_fields.c util.o bsearch.o

randomize:	randomize.c
	gcc -O3 -o randomize randomize.c

give_new_id_number:	give_new_id_number.c util.o bsearch.o
	gcc -O3 -o give_new_id_number give_new_id_number.c util.o bsearch.o

flip_flop:	flip_flop.c
	gcc -O3 -o flip_flop flip_flop.c

util.o: util.c util.h
	gcc -O3 -c util.c

bsearch.o: bsearch.c bsearch.h
	gcc -O3 -c bsearch.c

combine_pipes: util.o combine_pipes.c
	gcc -O3 -o combine_pipes combine_pipes.c util.o

dos2unix: dos2unix.c
	gcc -o dos2unix dos2unix.c
