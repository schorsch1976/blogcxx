#!/bin/sh
rm -Rf build
mkdir build
cd build
export CC=/usr/bin/clang-6.0
export CXX=/usr/bin/clang++-6.0
cmake 	-DBOOST_ROOT=/home/georg/Dokumente/boost/1.68.0 \
	-DBOOST_INCLUDEDIR=/home/georg/Dokumente/boost/1.68.0 \
	-DBOOST_LIBRARYDIR=/home/georg/Dokumente/boost/1.68.0/stage64/lib \
	-DBoost_NO_SYSTEM_PATHS=ON \
	-DCMAKE_BUILD_TYPE=Debug \
	-DWITH_DEBUG_LOG=ON \
	..

make -j8
