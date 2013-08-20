
@qmake -config "release develop" > NUL
@mingw32-make clean > NUL
@mingw32-make -j4 > NUL
@cd tests\SmokeTest > NUL
@qmake -config release > NUL
@mingw32-make clean > NUL
@mingw32-make -j4 > NUL
@cd ..\..
@md tests\test
@echo running smoke test
@echo ------------------
@SmokeTest.exe tests\test
@rd /S /Q tests\test

