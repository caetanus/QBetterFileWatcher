#!/bin/bash


buildLib()  {
    echo "Building lib..."
    qmake # -config "debug develop"
    make -j4 > /dev/null
    if [ "$?" != "0" ]
    then
        exit $?
    fi
}

buildTests() {
    echo "Compiling tests"
    cd tests/TestWatcher
    qmake -config "debug develop" && make clean 2>&1 > /dev/null
    make -j4 > /dev/null

    if [ "$?" != "0" ]
    then
        exit $?
    fi
    cd $OLDPWD
    cd tests/SmokeTest
    qmake -config "debug develop" && make clean 2>&1 > /dev/null
    make -j4 > /dev/null
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
