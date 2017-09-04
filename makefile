all : a.o utils.o
	gcc -o a a.o utils.o

a.o : a.c
	gcc -c a.c

utils.o : utils.c
	gcc -c utils.c


.PHONY : clean
clean:
	rm -f *.o a