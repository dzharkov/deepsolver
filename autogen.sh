#!/bin/sh -e

#created by aclocal;
/bin/rm -rf autom4te.cache
/bin/rm -f aclocal.m4

#created by libtoolize;
/bin/rm -rf m4
/bin/rm -f ltmain.sh

#created by autoconf;
/bin/rm -f configure

#created by automake;
/bin/rm -f install-sh missing depcomp config.guess config.sub
find ./ -name Makefile.in -exec /bin/rm '{}' \;

#Note that clean mode leaves the code tree at its origin state only after 'make distclean';
[ "$1" == 'clean' ] && exit 0

IPATHS="-I lib"
aclocal $IPATHS
libtoolize
autoconf $IPATHS
automake --add-missing

/bin/rm -rf autom4te.cache
echo Now run ./configure and then make.
