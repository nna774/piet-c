#! /bin/sh -xe
PROG_I=${0%.sh}
PROG=${PROG_I#./tests/}

[ `echo 0 | ./$PROG` = "0" ]
[ `echo 1 | ./$PROG` = "10" ]
[ `echo 2 | ./$PROG` = "210" ]
[ `echo 10 | ./$PROG` = "105210" ]
[ `echo 42 | ./$PROG` = "4221105210" ]
