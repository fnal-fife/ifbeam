LIBS=
DEFS=
TESTDEFS=-DUNITTEST
CFLAGS=-g $(DEFS) 
CXXFLAGS=-g $(DEFS) 
BIN= demo nucondb-test WebAPI-test

all: $(BIN)

clean:
	rm -f *.o $(BIN)

nucondb-test: nucondb.cc WebAPI.o nucondb.h
	g++ -o $@ $(TESTDEFS) $(CFLAGS) nucondb.cc WebAPI.o $(LIBS)

WebAPI-test: WebAPI.cc WebAPI.h
	g++ -o $@ $(TESTDEFS) $(CFLAGS) WebAPI.cc $(LIBS)

demo: nucondb.o WebAPI.o demo.o
	g++ -o $@ $(CFLAGS) demo.o nucondb.o WebAPI.o $(LIBS) 
  
WebAPI.o: WebAPI.h

nucondb.o: nucondb.h
