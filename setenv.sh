#!/bin/bash

MYPWD="`pwd`"
MYCLASS="$MYPWD/bin"


if [[ -z $DB2DIR ]]; then
    echo "no DB2DIR - source $0"
else

    rm -f $PWD/lib/db2java.zip
    ln -s $DB2DIR"/java/db2java.zip" $PWD/lib/db2java.zip

    case "$OSTYPE" in
    linux*)
	echo "Setup Linux";
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$MYPWD/lib/linuxGtk";
        rm -f $MYPWD/lib/swt.jar
	ln -s $MYPWD/lib/linuxGtk/swt.jar $MYPWD/lib/swt.jar
        ;;
    solaris*)
	echo "Setup Sun";
	AddRemoveString LD_LIBRARY_PATH ${INSTHOME?}/sqllib/lib64 r
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$MYPWD/lib/sunGtk";
	rm -f $MYPWD/lib/swt.jar
        ln -s $MYPWD/lib/sunGtk/swt.jar $MYPWD/lib/swt.jar
        ;;
    *)
	echo "Unknown OS `uname -a`"
        ;;
    esac

    for name in lib/*.* ; do
	MYCLASS="$MYCLASS:$MYPWD/$name"
    done
    export CLASSPATH=$CLASSPATH:$MYCLASS
fi
