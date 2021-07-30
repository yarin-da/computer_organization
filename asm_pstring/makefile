a.out: main.o run_main.o func_select.o pstring.o
	gcc -no-pie -g -o a.out main.o run_main.o func_select.o pstring.o

main.o: main.c
	gcc -g -c -o main.o main.c

run_main.o: run_main.s
	gcc -g -c -o run_main.o run_main.s

func_select.o: func_select.s
	gcc -g -c -o func_select.o func_select.s

pstring.o: pstring.s
	gcc -g -c -no-pie -o pstring.o pstring.s

clean:
	rm -f *.o a.out
