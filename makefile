CC       := c++
CXX      := g++
LD       := g++
XGBOOST  := /cvmfs/cms.cern.ch/$(SCRAM_ARCH)/external/py2-xgboost/0.80-ikaegh/lib/python2.7/site-packages/xgboost
CXXFLAGS := -Wall -Wextra -Werror -O2 -std=c++1y -g -I./ $(shell root-config --cflags) -lTMVA -I$(XGBOOST)/include/ -I$(XGBOOST)/rabit/include/
LDFLAGS  := -shared -Wall -Wextra $(shell root-config --libs) -lTMVA
SRC 	 := $(wildcard src/*.cc)
BINSRC 	 := $(wildcard test/*.cpp)
OBJS     := $(SRC:src/%.cc=obj/lib/%.o)
BINOBJS  := $(BINSRC:src/%.cpp=obj/bin/%.o)
BINS 	 := $(BINSRC:src/%.cpp=bin/%)
LIB 	 := lib/libCLFVAnalysis.so
LIBNAME  := CLFVAnalysis
PARENT_DIR := $(shell pwd)/../
SRC_DIR    := src
INC_DIR    := interface
LIB_DIR    := lib
PROG_DIR   := bin
EXE_DIR    := exe
OBJ_DIR    := obj

-include $(OBJS:.o=.d)
-include $(BINOBJS:.o=.d)

dirs:
	@mkdir -p obj/lib
	@mkdir -p lib
	@mkdir -p obj/bin
	@mkdir -p bin

obj/lib/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@
	$(CXX) $(CXXFLAGS) -MM -MP -MT $@ $< -o $(@:.o=.d)

obj/bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@
	$(CXX) $(CXXFLAGS) -MM -MP -MT $@ $< -o $(@:.o=.d)


lib/CLFVAnalysis_xr.cc:
	genreflex src/classes.h -s src/classes_def.xml -o lib/CLFVAnalysis_xr.cc --deep --fail_on_warnings \
	--rootmap=lib/CLFVAnalysis_xr.rootmap --rootmap-lib=lib/libCLFVAnalysis.so --capabilities=lib/CLFVAnalysis_xi.cc \
	-DCMS_DICT_IMPL -D_REENTRANT -DGNUSOURCE -D__STRICT_ANSI__ -DGNU_GCC -D_GNU_SOURCE \
	-I$(PARENT_DIR) -I$(XGBOOST)/include -I$(XGBOOST)/rabit/include/ -DCMSSW_REFLEX_DICT

lib/CLFVAnalysis_xr.cc.o: lib/CLFVAnalysis_xr.cc
	$(CC) -MMD -MF lib/CLFVAnalysis_xr.cc.d -c -I$(PARENT_DIR) -I./ -I$(XGBOOST)/include -I$(XGBOOST)/rabit/include/ -DGNU_GCC -D_GNU_SOURCE $(shell root-config --cflags)  \
	-DCMSSW_REFLEX_DICT -pthread -Os -Wno-unused-variable -g -fPIC lib/CLFVAnalysis_xr.cc -o lib/CLFVAnalysis_xr.cc.o


$(LIB): $(OBJS) lib/CLFVAnalysis_xr.cc.o
	$(LD) $(LDFLAGS) -o $@ $^

bin/%: obj/bin/%.o $(LIB)
	$(CXX) -o $@ $^ $(LDFLAGS)

exe: $(addprefix $(EXE_DIR)/,$(EXES))
	@echo "*** Compiling executables..."
$(EXE_DIR)/% : $(PROG_DIR)/%.cpp lib
	$(CC) $< -o $@ $(CCFLAGS) -L $(LIB_DIR) -l$(LIBNAME) -I$(INC_DIR) -I$(SRC_DIR) -I$(PARENT_DIR) -I$(XGBOOST)/include $(XGBOOST)/rabit/include/ $(LIB)

all: dirs $(LIB)

test: $(LIB) $(BINS)

# $(CXX) $(CXXFLAGS) -fPIC -c src/test.cpp -o obj/lib/test.o
# $(CXX) -o test.exe $(OBJS) test.o $(LDFLAGS) $(LIB)
# $(CXX) $(CXXFLAGS) $(shell root-config --libs) -L$(LIB) -pedantic test.cpp -o test.exe

.SECONDARY: $(BINOBJS) obj/lib/CLFVAnalysis_xr.cc
.PHONY: all clean
.DEFAULT_GOAL := all

clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(EXE_DIR)
	@rm -rf $(LIB_DIR)
	@rm src/*~
	@rm interface/*~
