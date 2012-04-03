LIB=libifbeam.a 
UTLOBJ=../util/*.o
UTLSRC=../util/*.cc
HDR=ifbeam.h ../util/*.h
OBJ=ifbeam.o $(UTL)
SRC=ifbeam.cc
TST=ifbeam-test ifbeam_art_test
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

ifbeam_art_test: ifbeam_art_test.cc ifbeam_art.cc ifbeam.o ../util/WebAPI.o ../util/utils.o
	g++ -o ifbeam_art_test ifbeam_art_test.cc ifbeam_art.cc ifbeam.o ../util/WebAPI.o ../util/utils.o
