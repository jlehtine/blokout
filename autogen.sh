#! /bin/sh

# Copyright 2007 Johannes Lehtinen
# This shell script is free software; Johannes Lehtinen gives
# unlimited permission to copy, distribute and modify it.

set -e

# Check directory
basedir="`dirname "$0"`"
if ! test -f "$basedir"/src/blocks.c; then
    echo 'Run autogen.sh in the top level source directory.' 1>&2
    exit 1
fi

# Generate files in top level directory
cd "$basedir"
aclocal -I m4
autoconf
automake -a
