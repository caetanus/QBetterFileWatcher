@qmake -config release > NUL
@mingw32-make clean > NUL
@mingw32-make -j4 > NUL
@cd tests\TestWatcher > NUL
@qmake -config release > NUL
@mingw32-make clean > NUL
@mingw32-make -j4 > NUL
@cd ..\..
@echo running smoke tests\test
@echo
@echo ------------------
@TestWatcher.exe 

@cd ..
