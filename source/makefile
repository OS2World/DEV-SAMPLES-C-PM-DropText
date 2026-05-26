# nmake makefile
#
# Tools used:
#  Compile::Watcom Resource Compiler
#  Compile::GNU C
#  Make: nmake or GNU make
all : droptext.exe

droptext.exe:  droptext.obj droptext.res droptext.def 
	gcc -Zomf droptext.obj droptext.def -o droptext.exe
	wrc droptext.res

droptext.obj: droptext.c droptext.h
	gcc -Wall -Zomf -c -O2 droptext.c -o droptext.obj

droptext.res : droptext.rc droptext.dlg droptext.h 
	wrc -r droptext.rc

clean :
	rm -rf *exe *res *obj
