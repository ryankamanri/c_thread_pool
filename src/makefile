clean: main.exe
	rm -rf *.o

main.exe: main.o sched.o switch_to.o thread.o
	gcc main.o sched.o thread.o switch_to.o -o main.exe -Xlinker "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64\WinMM.Lib"

main.o: main.c
	gcc -c main.c -o main.o -g

switch_to.o: switch_to.s
	gcc -c switch_to.s -o switch_to.o -g

sched.o: sched.c
	gcc -c sched.c -o sched.o -g

thread.o: thread.c
	gcc -c thread.c -o thread.o -g




