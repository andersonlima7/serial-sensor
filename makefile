LIBOBJS = turn.o
all: displaytst2
%.o : %.s
 as $(DEBUGFLGS) $(LSTFLGS) $< -o $@
libdisplay.a: $(LIBOBJS)
 ar -cvq libdisplay.a turn.o
displaytst2: displaytst.c libdisplay.a
 gcc -o displaytst2 displaytst.c libdisplay.a