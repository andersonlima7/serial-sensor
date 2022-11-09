LIBOBJS = initLCD.o clearLCD.o writeLCD.o mapmem.o

all: main

%.o : %.s
	as $(DEBUGFLGS) $(LSTFLGS) $< -o $@

libdisplay.a: $(LIBOBJS)
	ar -cvq libdisplay.a initLCD.o clearLCD.o writeLCD.o mapmem.o

main: main.c libdisplay.a
	gcc -o main main.c libdisplay.a