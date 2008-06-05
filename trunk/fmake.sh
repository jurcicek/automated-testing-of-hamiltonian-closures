#! /bin/sh

# fmake $Revision: 1.2 $ $Date: 2002/03/22 17:18:58 $
# Copyright (C) 2002 Filip Jurcicek

if [ "$1" = "-h" ]
then
  echo "Usage: fmake [-h]"
  echo
  echo "$Revision: 1.2 $ $Date: 2002/03/22 17:18:58 $ (C) by Filip Jurcicek"
  echo
  echo "Compile all \$SOURCE_FILES files."
  echo "Move their object files to the directory ./\$OBJECTS_DIR"
  echo "Then make a library ./\$LIB_NAME from this objects files"
  echo " or make program \$PROG_NAME."
  echo "Object files dependences are stored in directory \$DEPS_DIR."
  echo "If you change a project configuration, you must remove all appropriate object files."
  echo
  echo " COMPILER specify a name of used compiler."
  echo " WARN     specify warning option for compiler."
  echo " INCLUDE  set all includes directories."
  echo " CPPARGS  set another compiler flags."
  echo " MACROS   predefine some macros."
  echo
  echo "Operation modes:"
  echo "       -h       prints this help, then exit"
  echo

  exit 1
fi

if ! [ -e $OBJECTS_DIR ]
then
  mkdir $OBJECTS_DIR
fi

if ! [ -e $DEPS_DIR ]
then
  mkdir $DEPS_DIR
fi

OBJS=`pwd`/$OBJECTS_DIR
PATH_OLD=`pwd`
OBJECTS=

for i in $SOURCE_FILES
do
    cd $PATH_OLD
    FILE_CPP=`echo $i | sed "s/[a-zA-Z0-9/\.]*\/\([a-zA-Z0-9]*\)\(\.cpp\)/\1\2/g"`
    FILE_O=`echo $FILE_CPP | sed "s/\.cpp/\.o/g"`

    OBJECTS=`echo $OBJECTS ./$OBJECTS_DIR/$FILE_O`

    DIR=`echo $i | sed "s/\([a-zA-Z0-9/\.]*\/\)\([a-zA-Z0-9]*\)\(\.cpp\)$/\1/g"`
    cd $DIR

    echo "Processed file: $FILE_CPP"
    if [ -e $PATH_OLD/$OBJECTS_DIR/$FILE_O ] && [ -e $PATH_OLD/$DEPS_DIR/$FILE_O.dep ]
    then

	if [ "$HOSTTYPE" = "mips" ]
	then
	  OBJECT_TIME=`set $(stat -t $PATH_OLD/$OBJECTS_DIR/$FILE_O); echo ${28} | tr -d \<\>`
	else
	  OBJECT_TIME=`set $(stat -t $PATH_OLD/$OBJECTS_DIR/$FILE_O); echo ${13}`
	fi

	RECOMPILE=0
	for d in `echo $FILE_CPP; cat $PATH_OLD/$DEPS_DIR/$FILE_O.dep`
	do
	    if [ "$HOSTTYPE" = "mips" ]
	    then
		SOURCE_TIME=`set $(stat -t $d); echo ${28} | tr -d \<\>`
	    else
		SOURCE_TIME=`set $(stat -t $d); echo ${13}`
	    fi
	    
	    # echo "OT $OBJECT_TIME < ST $SOURCE_TIME"
	    
	    if [ $(( $OBJECT_TIME < $SOURCE_TIME)) -eq 1 ]
	    then
		RECOMPILE=1
		break
	    fi
	done
	
	
	if [ $RECOMPILE -eq 0 ]
	then
	    cd $PATH_OLD
	    
	    continue
	fi
    fi    
    
    # compile changed file
    echo Compile: $COMPILER $WARN $INCLUDE $CPPARGS $MACROS -Wp,-MD,$PATH_OLD/$DEPS_DIR/$FILE_O -c $FILE_CPP    
    $COMPILER $WARN $INCLUDE $CPPARGS $MACROS -Wp,-MD,$PATH_OLD/$DEPS_DIR/$FILE_O -c $FILE_CPP 2> err.log 
    
    #echo "echo $FILE_CPP | sed "s/\\\./\\\\\./g"; echo $i | sed "s/\\\//\\\\\\\\\\\//g""
    F=`echo $FILE_CPP | sed "s/\\\./\\\\\./g"`
    R=`echo $i | sed "s/\\\//\\\\\\\\\\\//g"`
    
    #echo "more err.log | sed "s/$F/$R/g""
    cat err.log | sed "s/$F/$R/g"
    rm -f err.log

    # preprocess dependence file
    cat $PATH_OLD/$DEPS_DIR/$FILE_O | tr '[:space:]' '\n' | sed "s/\([a-zA-Z0-9]*\.o:\)\(.*\)/\2/g" | grep -v '\.cpp$' | grep -v '^/usr/' | grep -v '\\' | grep -v -x '[:space:]*' > $PATH_OLD/$DEPS_DIR/$FILE_O.dep
    rm $PATH_OLD/$DEPS_DIR/$FILE_O   	
    
    cd $PATH_OLD
    
    # echo Moving `echo $i | sed "s/\.cpp$/.o/g"` to ./$OBJECTS_DIR/$FILE_O
    rm -f ./$OBJECTS_DIR/$FILE_O
    mv `echo $i | sed "s/\.cpp$/.o/g"` ./$OBJECTS_DIR/$FILE_O 
done

if [ "$LIB_NAME" != "" ]
then
    rm -f $LIB_NAME

    echo Library: ar cru $LIB_NAME $OBJECTS
    ar cru $LIB_NAME $OBJECTS
    ranlib $LIB_NAME
fi

if [ "$PROG_NAME" != "" ]
then
    rm -f $PROG_NAME
    
    echo Link: $COMPILER $WARN $INCLUDE $CPPARGS $MACROS -o $PROG_NAME $OBJECTS $PROG_LIBS
    $COMPILER $WARN $INCLUDE $CPPARGS $MACROS -o $PROG_NAME $OBJECTS $PROG_LIBS
fi

