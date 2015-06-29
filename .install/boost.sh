#!/bin/bash
echo $1
echo $2
wget http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz > /dev/null
echo A
tar xfz boost_1_58_0.tar.gz > /dev/null
echo B
cd boost_1_58_0
echo C
./bootstrap.sh --with-libraries=$1 > /dev/null
echo D
./b2 --prefix=$2 install > /dev/null
echo E
