
CXXFLAGS= \
  -I$(IFBEAM_FQ_DIR)/include \
  -I$(LIBWDA_FQ_DIR)/include \
  -I$(IFDHC_FQ_DIR)/inc 

LDFLAGS= \
  -L$(IFBEAM_LIB) \
  -L$(LIBWDA_LIB) \
  -L$(IFDHC_LIB) \
  -lifbeam -lifdh -lwda -lcurl

.c.o: 
	g++ $* -o $< $(CXXFLAGS) $(LDFLAGS) 
