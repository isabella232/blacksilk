#!/usr/bin/env bash

QTDIR=/home/samuel/Qt5.14.2/5.14.2/gcc_64
QT_LIBS=(OpenGL Widgets Gui Core XcbQpa DBus Svg)

cd "$1" || exit

mkdir -p "lib"
mkdir -p "plugins"

for LIB in ${QT_LIBS[@]:0};
do
    QT_LIB="libQt5$LIB.so.5"
    echo "cp $QT_LIB"
    cp "$QTDIR/lib/$QT_LIB" "lib/$QT_LIB"
done

ICU_LIBS=( i18n uc data )

for LIB in ${ICU_LIBS[@]:0};
do
    ICU_LIB="libicu$LIB.so.56"
    echo "cp $ICU_LIB"
    cp "$QTDIR/lib/$ICU_LIB" "lib/$ICU_LIB"
done

PLUGINS=( imageformats platforms xcbglintegrations )

for PLUGIN in ${PLUGINS[@]:0};
do
    echo "cp $PLUGIN"
    cp -r "$QTDIR/plugins/$PLUGIN" "plugins/"
done
