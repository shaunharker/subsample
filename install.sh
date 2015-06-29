#!/bin/bash
CUR_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
if [ $# -ge 1 ]; then
    # The user supplied an argument
    PREFIX=${1}
    # Get absolute path name of install directory
    mkdir -p "${PREFIX}" 2> /dev/null
    cd "${PREFIX}" > /dev/null 2>&1
    if [ $? != 0 ] ; then
        echo "ERROR: directory '${PREFIX}' does not exist nor could be created."
        echo "Please choose another directory."
        exit 1
    else
        PREFIX=`pwd -P`
    fi
    ARGUMENT="-DCMAKE_PREFIX_PATH=${PREFIX} -DUSER_INCLUDE_PATH=${PREFIX}/include -DUSER_LIBRARY_PATH=${PREFIX}/lib"
fi

cd ${CUR_DIR}
rm -rf build
mkdir build
cd build
# Note: we pass `which g++` because apparently
#  CMake doesn't necessarily pick the compiler on the path
cmake $ARGUMENT ..
make
make install
