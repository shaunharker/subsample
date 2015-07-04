#!/bin/bash
#  Usage: install.sh [--prefix=INSTALLATION_PREFIX]
#  Effects: Copies header files into INSTALLATION_PREFIX/include
#           If no prefix is provided, it chooses 
#            /usr/local 
#            unless it cannot write there, in which case it chooses
#            ~/.local 
#            unless it does not exist, in which case it fails.

## Parse command line arguments to get
#  PREFIX, SEARCHPATH, BUILDTYPE, TESTS, and MASS
SRC_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SRC_DIR/.install/parse.sh

## Build the library
./.install/build.sh --prefix=$PREFIX --searchpath=$SEARCHPATH --build=$BUILDTYPE --test $MASS || exit 1

