PDOBJ=sine~.pd_linux \
	  biscale~.pd_linux \
	  met~.pd_linux \
	  tenv~.pd_linux \
	  tog~.pd_linux \
	  branch~.pd_linux \
	  saw~.pd_linux \
	  round~.pd_linux \
	  moogladder~.pd_linux \
	  port~.pd_linux \
	  thresh~.pd_linux \

EXTPATH?=~/.pd/

default: $(PDOBJ)

clean: ; rm -f *.pd_linux *.o

.SUFFIXES: .pd_linux

LINUXCFLAGS = -DPD -O2 -funroll-loops -fomit-frame-pointer -Wall -fPIC

.c.pd_linux:
	cc $(LINUXCFLAGS) -o $*.o -c $*.c
	ld -o $*.pd_linux $*.o -lc -lsporth -lsoundpipe -lsndfile -lm -shared
	strip --strip-unneeded $*.pd_linux
	rm $*.o

install: $(PDOBJ)
	cp $(PDOBJ) $(EXTPATH)

