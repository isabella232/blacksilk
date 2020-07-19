#!/usr/bin/env bash

function silent_remove {
    if [ -f "$1" ]; then
        rm "$1"
    fi
    if [ -d "$1" ]; then
        rm -rf "$1"
    fi
}

function indent {
    sed  's/^/     /'
}

silent_remove results.txt

case $(uname) in
    Linux)
        SPEC="linux-g++"
        MAKE="make --silent"
        PATH="/opt/local/qt/bin/:$PATH"
        ;;
    Darwin)
        SPEC="macx-clang"
        MAKE="make --silent"
        ;;
    CYGWIN*)
        SPEC="win32-msvc2012"
        MAKE="jom.exe"
        # add this line to C:\cygwin\Cygwin.bat
        # call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" amd64 >NUL;
        # and revert PATH order in /etc/profile :
        # PATH="${PATH}:/usr/local/bin:/usr/bin/"
         ;;
    *)
        echo "Unknown OS"
        ;;
esac

TESTS="ColorSpaces Mixer YUVFrame ImageMagick trialversion"

for test in $TESTS; do
    (
        echo "$test"
        cd "$test"
        silent_remove Makefile*
        silent_remove bin/"test$test"*

        qmake "test$test.pro" -r -spec "$SPEC" CONFIG+=Release CONFIG-=Debug CONFIG+=x86_64 2>&1 | indent

        ${MAKE} clean > /dev/null 2>&1
        echo "Clean"
        ${MAKE} > /dev/null 2>&1
        echo "Compile"

        if [ -f ./"bin/test$test" ]; then
            ( 
                echo "Running test${test}"
                cd bin;
                ./"test$test" >> ../../results.txt
            )
        fi
        echo "" >> ../results.txt
        echo
    )
done

echo
grep Totals < results.txt



