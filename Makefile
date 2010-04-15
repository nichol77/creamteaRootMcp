#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include Makefile.arch

#Site Specific  Flags
SYSINCLUDES	= 
SYSLIBS         = 

ifdef RJN_UTIL_INSTALL_DIR
RJN_UTIL_LIB_DIR=${RJN_UTIL_INSTALL_DIR}/lib
RJN_UTIL_INC_DIR=${RJN_UTIL_INSTALL_DIR}/include
LD_RJN_UTIL=-L$(RJN_UTIL_LIB_DIR)
INC_RJN_UTIL=-I$(RJN_UTIL_INC_DIR)
else
RJN_UTIL_LIB_DIR=/usr/local/lib
RJN_UTIL_INC_DIR=/usr/local/include
endif

#Toggles the FFT functions on and off
USE_FFT_TOOLS=0

ifdef USE_FFT_TOOLS
FFTLIBS = -lRootFftwWrapper -lfftw3
FFTFLAG = -DUSE_FFT_TOOLS 
else
FFTLIBS =
FFTFLAG =
endif

ifdef USE_GOOGLE_PROFILER
SYSLIBS +=-lprofiler
endif

#Generic and Site Specific Flags
#Toggle between MCP_PCI and USB using the READOUT_MCP_CPCI flag
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_RJN_UTIL) -DREADOUT_MCP_CPCI 
LDFLAGS      += -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS) -lMathMore -lMinuit $(SYSLIBS) $(LD_RJN_UTIL) $(FFTLIBS) -lusb
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = libMcpTargetRoot.${DLLSUF}
LIB_OBJS =  McpTarget.o McpTargetDisplay.o WaveformGraph.o FFTGraph.o stdUSBl.o TargetData.o RawTargetData.o McpPci.o targetDict.o
CLASS_HEADERS = McpTarget.h McpTargetDisplay.h WaveformGraph.h FFTGraph.h TargetData.h RawTargetData.h McpPci.h


all : $(ROOT_LIBRARY)


#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LIBS) $(LDFLAGS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
		   $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIBS) $(LIB_OBJS) -o $@
endif

%.$(OBJSUF) : %.$(SRCSUF)
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o  $@

%.$(OBJSUF) : %.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@


targetDict.C: $(CLASS_HEADERS)
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
	rootcint $@ -c $(INC_RJN_UTIL)  $(CLASS_HEADERS) LinkDef.h

install: $(ROOT_LIBRARY)
	install -d $(RJN_UTIL_LIB_DIR)
	install -d $(RJN_UTIL_INC_DIR)
ifeq ($(PLATFORM),macosx)
	install -c -m 755 $(ROOT_LIBRARY) $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY)) $(RJN_UTIL_LIB_DIR)
else
	install -c -m 755 $(ROOT_LIBRARY) $(RJN_UTIL_LIB_DIR)
endif
	install -c -m 644  $(CLASS_HEADERS) $(RJN_UTIL_INC_DIR)

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#############################################################################



