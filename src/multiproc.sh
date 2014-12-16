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
