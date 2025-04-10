
UNAME:= $(shell uname -s)
ifeq ($(UNAME),Darwin)
SHLIB=dylib
else
SHLIB=so 
endif

# enable use of clang
CXX ?= g++

LIB=libifbeam.a 
IFBLIB=libifbeam.$(SHLIB)
OTHER_LIBS=-L$(LIBWDA_LIB) -L$(IFDHC_LIB) -lifdh -lwda
UTLSRC=../util/*.cc
HDR=ifbeam.h wda_singleton.h
OBJ=ifbeam.o wda_singleton.o $(UTL)
SRC=ifbeam.cc
TST=ifbeam-test
#DEFS=-DOLD_CACHE
TESTDEFS=-DUNITTEST
CXXFLAGS=-pedantic-errors -Wall -Wextra -Werror -fPIC -g -O3 $(DEFS) $(ARCH) -I. -I$(LIBWDA_FQ_DIR)/include -I$(IFDHC_FQ_DIR)/inc

DESTDIR=..
VPATH=../../src

all: $(BIN) $(LIB) $(IFBLIB) $(TST)

install: install-lib install-headers

install-lib:
	test -d $(DESTDIR)/lib || mkdir $(DESTDIR)/lib
	cp $(LIB) $(IFBLIB)  $(DESTDIR)/lib

install-headers:
	test -d $(DESTDIR)/include || mkdir -p $(DESTDIR)/include
	cp $(HDR) $(DESTDIR)/include

clean:
	rm -f *.o *.a $(IFBLIB)

$(LIB): $(OBJ) $(UTLOBJ)
	rm -f $(LIB)
	ar qv $(LIB)  $(OBJ) $(UTLOBJ)

$(IFBLIB): $(OBJ) $(UTLOBJ)
	${CXX} $(ARCH) --shared -Wl,-soname,$(IFBLIB) -o $(IFBLIB) $(OBJ) $(UTLOBJ) $(OTHER_LIBS) -lcurl 

$(UTLOBJ):
	cd ../util; make

%-test: %.cc
	${CXX} -c -o $@.o $(ARCH) $(TESTDEFS) $(CXXFLAGS) $<
	${CXX} -o $@ $@.o $(ARCH)  $(OTHER_LIBS) wda_singleton.o -lcurl 

%.o: %.cc
	${CXX} -c -o $@  $(CXXFLAGS) $< 

ifbeam.o: ifbeam.cc
