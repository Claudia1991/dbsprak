#!/bin/bash

cd ..

MYPWD="`pwd`";
MYCLASS="$MYPWD/bin:";
for name in lib/* ; do
 MYCLASS="$MYCLASS$MYPWD/$name:";
done;

export CLASSPATH=$CLASSPATH:$MYCLASS
java musicDB.ParseMusicXML $*
