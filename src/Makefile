LIB=libifbeam.a 
SHLIB=libifbeam.so
OTHER_LIBS=-L$(LIBWDA_DIR)/lib -L$(IFDHC_LIB) -lifdh -lwda
UTLSRC=../util/*.cc
HDR=ifbeam.h
OBJ=ifbeam.o $(UTL)
SRC=ifbeam.cc
TST=ifbeam-test
#DEFS=-DOLD_CACHE
TESTDEFS=-DUNITTEST
CXXFLAGS=-pedantic-errors -Wall -Wextra -Werror -fPIC -g -O3 $(DEFS) $(ARCH) -I$(SRCDIR) -I../fife_wda -I../../fife_wda -I$(LIBWDA_DIR)/include -I$(IFDHC_DIR)/inc

DESTDIR=..
VPATH=../../src

all: $(BIN) $(TST) $(LIB) $(SHLIB)

install: install-lib install-headers

install-lib:
	test -d $(DESTDIR)/lib || mkdir $(DESTDIR)/lib
	cp $(LIB) $(SHLIB)  $(DESTDIR)/lib

install-headers:
	test -d $(DESTDIR)/include || mkdir -p $(DESTDIR)/include
	cp $(HDR) $(DESTDIR)/include

clean:
	rm -f *.o *.a 

$(LIB): $(OBJ) $(UTLOBJ)
	rm -f $(LIB)
	ar qv $(LIB)  $(OBJ) $(UTLOBJ)

$(SHLIB): $(OBJ) $(UTLOBJ)
	g++ $(ARCH) --shared -o $(SHLIB) $(OBJ) $(UTLOBJ) -lcurl 

$(UTLOBJ):
	cd ../util; make

%-test: %.cc
	g++ -c -o $@.o $(ARCH) $(TESTDEFS) $(CXXFLAGS) $<
	g++ -o $@ $@.o $(ARCH)  $(OTHER_LIBS) -lcurl 

%.o: %.cc
	g++ -c -o $@  $(CXXFLAGS) $< 

ifbeam.o: ifbeam.cc
