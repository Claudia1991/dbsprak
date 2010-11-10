#!/bin/bash

if [[ ! -d bin ]] ; then
    mkdir bin
fi;

javac -d bin -sourcepath src/java src/java/musicDB/MusicDBTextUI.java
javac -d bin -sourcepath src/java src/java/musicDB/MusicDBGraphicUI.java

cp src/java/*properties* bin/