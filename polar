#!/bin/sh

if [ $# != 2 ];
     then echo "Usage: $0 boat_type polar+file"
    exit 2
fi

boat_type=$1

cat >polar_$1.inc<<EOF
/* polar from `basename $2 | sed 's/.*_\([^\.]*\).*/\1/'` */

void init_polar$1() {

EOF

cat $2 | grep -v TWA | awk '{ for (i=1; i<=NF; i++) {if (i == 1) { angle = $i } else { print "  polar['$1'][" angle "][" (2*i-4) "]=" $i ";" } } }' FS=';' >> polar_$1.inc

cat >>polar_$1.inc<<EOF

  init_polar_fill_blanks_5_2($1);
}
EOF
