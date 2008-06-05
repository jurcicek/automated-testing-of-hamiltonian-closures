#! /bin/sh

SOURCE_FILES=""
for i in `find ./ ../Source | grep '\.cpp$'`
do
  SOURCE_FILES=`echo $SOURCE_FILES $i`
done

export SOURCE_FILES
export OBJECTS_DIR="release_objs"
export DEPS_DIR="release_deps"

export COMPILER=g++
export WARN="-Wall -Wno-comment -Wno-reorder"
export INCLUDE=""
export PROG_LIBS=""
export CPPARGS="-O3"
export MACROS="-D_DEBUG"

export PROG_NAME="hamiltonpath_release"

../fmake.sh
