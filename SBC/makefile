LIBOBJS = display.o mapmem.o

all: main

%.o : %.s
	as $(DEBUGFLGS) $(LSTFLGS) $< -o $@

libdisplay.a: $(LIBOBJS)
	ar -cvq libdisplay.a display.o mapmem.o

main: main.c libdisplay.a
	gcc -o main main.c libdisplay.a