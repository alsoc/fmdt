#!/bin/bash
set -x

cmake --version
mkdir build
cd build

if [ -z "$CXX" ]
then
	echo "Please define the 'CXX' environment variable."
	exit 1
fi

if [ -z "$THREADS" ]
then
	echo "The 'THREADS' environment variable is not set, default value = 1."
	THREADS=1
fi

if [ -z "$NAME" ]
then
	echo "The 'NAME' environment variable is not set, default value = 'build_linux_macos'."
	NAME="build_linux_macos"
fi

cmake .. -G"Unix Makefiles" -DCMAKE_CXX_COMPILER=$CXX \
         -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O3 -g" -DCMAKE_CXX_FLAGS="$CFLAGS" \
         -DCMAKE_EXE_LINKER_FLAGS="$LFLAGS" \
         $CMAKE_OPT

rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

make -j $THREADS -k
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi

cd ..
rm -rf $NAME
mkdir $NAME
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
cp build/bin/* $NAME
rc=$?; if [[ $rc != 0 ]]; then exit $rc; fi
