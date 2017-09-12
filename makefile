all : a.o utils.o
	gcc -o a a.o utils.o

a.o : a.c utils.h box_define.h
	gcc -c a.c

utils.o : utils.c utils.h box_define.h
	gcc -c utils.c


.PHONY : clean
clean:
	rm -f *.o a