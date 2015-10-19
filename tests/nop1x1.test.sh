#! /bin/sh
PROG_I=${0%.sh}
PROG=${PROG_I#./tests/}

[ -z `./$PROG` ]
[ -z `echo 42 | ./$PROG` ]
[ -z `echo a  | ./$PROG` ]
[ -z `echo    | ./$PROG` ]
