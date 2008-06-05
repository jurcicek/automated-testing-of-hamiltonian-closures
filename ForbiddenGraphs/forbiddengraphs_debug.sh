#! /bin/sh

SOURCE_FILES=""
for i in `find ./ ../Source | grep '\.cpp$'`
do
  SOURCE_FILES=`echo $SOURCE_FILES $i`
done

export SOURCE_FILES
export OBJECTS_DIR="debug_objs"
export DEPS_DIR="debug_deps"

export COMPILER=g++
export WARN="-Wall -Wno-comment -Wno-reorder"
export INCLUDE=""
export PROG_LIBS=""
export CPPARGS="-g3 -O0"
export MACROS="-D_DEBUG"

export PROG_NAME="forbiddengraphs_debug"

../fmake.sh
