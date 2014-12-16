#!/bin/bash
#
#
#bar_database - commands to manipulate vertical bar delimited fields as a NOSql style database.
#Copyright (C) 2011-2014  Hibbard M. Engler of Killer Cool Development, LLC.
#
#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# multiproc4.sh $Revision$
# $Log$
# Revision 1.2  2014/12/16 17:35:00  hib
# Added GPL to the c and h files. GPL 2.0 for stand alone commands, and LGPL 2.1 for the
# libraries (.o files). Yo!
#
#
#
#
rm 2>/dev/null $TMP/i1_$$ $TMP/i2_$$ $TMP/i3_$$ $TMP/i4_$$ $TMP/i5_$$ $TMP/i6_$$ 
mknod $TMP/i1_$$ p
mknod $TMP/i2_$$ p
mknod $TMP/i3_$$ p
mknod $TMP/i4_$$ p
mknod $TMP/i5_$$ p
mknod $TMP/i6_$$ p
rm 2>/dev/null $TMP/e1_$$ $TMP/e2_$$ $TMP/e3_$$ $TMP/e4_$$ $TMP/e5_$$ $TMP/e6_$$ 
rm 2>/dev/null $TMP/o1_$$ $TMP/o2_$$ $TMP/o3_$$ $TMP/o4_$$ $TMP/o5_$$ $TMP/o6_$$

exec_args=
for arg in "$@" ; do
  exec_args="$exec_args \"$arg\""
done
{
exec_cmd="exec $exec_args <$TMP/i1_$$ >$TMP/o1_$$ 2>$TMP/e1_$$ &"
eval $exec_cmd 
exec_cmd="exec $exec_args <$TMP/i2_$$ >$TMP/o2_$$ 2>$TMP/e2_$$ &"
eval $exec_cmd 
exec_cmd="exec $exec_args <$TMP/i3_$$ >$TMP/o3_$$ 2>$TMP/e3_$$ &"
eval $exec_cmd 
exec_cmd="exec $exec_args <$TMP/i4_$$ >$TMP/o4_$$ 2>$TMP/e4_$$ &"
eval $exec_cmd 
exec_cmd="exec $exec_args <$TMP/i5_$$ >$TMP/o5_$$ 2>$TMP/e5_$$ &"
eval $exec_cmd 
exec_cmd="exec $exec_args <$TMP/i6_$$ >$TMP/o6_$$ 2>$TMP/e6_$$ &"
eval $exec_cmd 
flip_flop >$TMP/i1_$$ 2>$TMP/i2_$$ 3>$TMP/i3_$$ 4>$TMP/i4_$$ 5>$TMP/i5_$$ 6>$TMP/i6_$$ 6 
}
wait
cat $TMP/o1_$$ $TMP/o2_$$ $TMP/o3_$$ $TMP/o4_$$ $TMP/o5_$$ $TMP/o6_$$
rm 2>/dev/null $TMP/o1_$$ $TMP/o2_$$ $TMP/o3_$$ $TMP/o4_$$ $TMP/o5_$$ $TMP/o6_$$ 
rm 2>/dev/null $TMP/i1_$$ $TMP/i2_$$ $TMP/i3_$$ $TMP/i4_$$ $TMP/i5_$$ $TMP/i6_$$ 
cat 1>&2 $TMP/e1_$$ $TMP/e2_$$ $TMP/e3_$$ $TMP/e4_$$ $TMP/e5_$$ $TMP/e6_$$
rm 2>/dev/null $TMP/e1_$$ $TMP/e2_$$ $TMP/e3_$$ $TMP/e4_$$ $TMP/e5_$$ $TMP/e6_$$ 
