LIB=libifbeam.a 
UTLOBJ=../util/*.o
UTLSRC=../util/*.cc
HDR=ifbeam.h ../util/*.h
OBJ=ifbeam.o $(UTL)
SRC=ifbeam.cc
TST=ifbeam-test
TESTDEFS=-DUNITTEST
CXXFLAGS=-fPIC -g $(DEFS) 

all: $(BIN) $(TST) $(LIB)

clean:
	rm -f *.o *.a 

$(LIB): $(OBJ) $(UTLOBJ)
	rm -f $(LIB)
	ar qv $(LIB) $(OBJ) $(UTLOBJ)

$(UTLOBJ):
	cd ../util; make

%-test: %.cc
	g++ -o $@ $(TESTDEFS) $(CXXFLAGS) $(UTLOBJ) $<

