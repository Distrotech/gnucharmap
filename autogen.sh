#! /bin/sh
#
# $Id$
#
# runs autotools to create ./configure and friends
#

PROJECT=gucharmap

srcdir=`dirname "$0"`
test -z $srcdir && srcdir=.

origdir=`pwd`
cd "$srcdir"

if test -z "$AUTOGEN_SUBDIR_MODE" && test -z "$*"
then
  echo "I am going to run ./configure with no arguments - if you wish "
  echo "to pass any to it, please specify them on the $0 command line."
fi

# Use the versioned executables if available.
aclocal=aclocal-1.4
automake=automake-1.4
$aclocal  --version </dev/null 2>&1 || aclocal=aclocal
$automake --version </dev/null 2>&1 || automake=automake


rm -f config.guess config.sub depcomp install-sh missing mkinstalldirs
rm -f config.cache acconfig.h
rm -rf autom4te.cache

set_option=':'
test -n "${BASH_VERSION+set}" && set_option='set'

$set_option -x

$aclocal $ACLOCAL_FLAGS                  || exit 1
glib-gettextize --force --copy           || exit 1
libtoolize --force --copy                || exit 1
autoheader                               || exit 1
$automake --foreign --add-missing --copy || exit 1
autoconf                                 || exit 1
cd "$origdir"                            || exit 1

if test -z "$AUTOGEN_SUBDIR_MODE"
then
  "$srcdir/configure" "$@" || exit 1
  $set_option +x
  echo
  echo "Now type 'make' to compile $PROJECT."
fi

exit 0

