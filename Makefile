PDOBJ=sine~.pd_linux \
	  biscale~.pd_linux \
	  met~.pd_linux \
	  tenv~.pd_linux \
	  tog~.pd_linux \
	  branch~.pd_linux \
	  saw~.pd_linux \
	  round~.pd_linux \

default: $(PDOBJ)

clean: ; rm -f *.pd_linux *.o

# ----------------------- LINUX i386 -----------------------


.SUFFIXES: .pd_linux

LINUXCFLAGS = -DPD -O2 -funroll-loops -fomit-frame-pointer \
    -Wall -W -Wshadow -Wstrict-prototypes \
    -Wno-unused -Wno-parentheses -Wno-switch -fPIC

LINUXINCLUDE =  -I../../src 

.c.pd_linux:
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o $*.o -c $*.c
	ld -o $*.pd_linux $*.o -lc -lsporth -lsoundpipe -lsndfile -lm -shared
	strip --strip-unneeded $*.pd_linux
	rm $*.o

