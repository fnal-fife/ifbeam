SRCDIR=../../util/
VPATH=$(SRCDIR)
LIB=libifbeam.a 
SHLIB=libifbeam.so
UTLOBJ=../util/*.o
UTLSRC=../util/*.cc
HDR=ifbeam.h ../util/*.h
OBJ=ifbeam.o $(UTL)
SRC=ifbeam.cc
TST=ifbeam-test
TESTDEFS=-DUNITTEST
CXXFLAGS=-pedantic-errors -Wall -Werror -fPIC -g $(DEFS) $(ARCH) -I$(SRCDIR)

VPATH=../../ifbeam

all: $(BIN) $(TST) $(LIB) $(SHLIB)

install:
	test -d ../lib || mkdir ../lib
	test -d ../include || mkdir -p ../include
	cp $(LIB) ../lib
	cp $(HDR) ../include

clean:
	rm -f *.o *.a 

$(LIB): $(OBJ) $(UTLOBJ)
	rm -f $(LIB)
	ar qv $(LIB) $(OBJ) $(UTLOBJ)

$(SHLIB): $(OBJ) $(UTLOBJ)
	g++ --shared -o $(SHLIB) $(OBJ) $(UTILOBJ)

$(UTLOBJ):
	cd ../util; make

%-test: %.cc
	g++ -o $@ $(TESTDEFS) $(CXXFLAGS) $(UTLOBJ) $<

%.o: %.cc
	g++ -c -o $@  $(CXXFLAGS) $< 
