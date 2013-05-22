SRCDIR=../../util/
VPATH=$(SRCDIR)
LIB=libifbeam.a 
SHLIB=libifbeam.so
UTLOBJ=../util/*.o ../fife_wda/wda.o ../fife_wda/ifbeam.o
UTLSRC=../util/*.cc
HDR=ifbeam.h ../util/*.h
OBJ=ifbeam.o $(UTL)
SRC=ifbeam.cc
TST=ifbeam-test
TESTDEFS=-DUNITTEST
CXXFLAGS=-pedantic-errors -Wall -Wextra -Werror -fPIC -g $(DEFS) $(ARCH) -I$(SRCDIR) -I../fife_wda -I../../fife_wda

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
	g++ $(ARCH) --shared -o $(SHLIB) $(OBJ) $(UTLOBJ) -lcurl 

$(UTLOBJ):
	cd ../util; make

%-test: %.cc
	g++ -c -o $@.o $(ARCH) $(TESTDEFS) $(CXXFLAGS) $<
	g++ -o $@ $@.o $(ARCH)  $(UTLOBJ) -lcurl 

%.o: %.cc
	g++ -c -o $@  $(CXXFLAGS) $< 
ifbeam.o: ifbeam.cc ../util/WebAPI.h 
