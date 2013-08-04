#!/bin/bash


buildLib()  {
    echo "Building lib..."
    qmake
    make -j4 > /dev/null
    if [ "$?" != "0" ]
    then
        exit $?
    fi
}

buildTests() {
    echo "Compiling tests"
    cd tests/TestWatcher
    qmake -config debug && make clean 2>&1 > /dev/null
    make -j4 > /dev/null

    if [ "$?" != "0" ]
    then
        exit $?
    fi
    cd $OLDPWD
}

runTests() {
    echo "Running Tests"
    builds/TestWatcher

    if [ "$?" = "0" ]
    then
        return 0
    fi
    exit $?
}

buildLib
buildTests
runTests
