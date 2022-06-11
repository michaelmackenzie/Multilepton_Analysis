CXX      := g++
LD       := g++
CXXFLAGS := -Wall -Wextra -Werror -O2 -std=c++1y -g -I./ $(shell root-config --cflags) -lTMVA
LDFLAGS  := -shared -Wall -Wextra $(shell root-config --libs) -lTMVA
SRC 	 := $(wildcard src/*.cc)
BINSRC 	 := $(wildcard test/*.cpp)
OBJS     := $(SRC:src/%.cc=obj/lib/%.o)
BINOBJS  := $(BINSRC:src/%.cpp=obj/bin/%.o)
BINS 	 := $(BINSRC:src/%.cpp=bin/%)
LIB 	 := lib/libCLFVAnalysis.so

-include $(OBJS:.o=.d)
-include $(BINOBJS:.o=.d)

obj/lib/%.o: src/%.cc
	if [[ ! -d obj/lib ]]; then mkdir -p obj/lib; fi; if [[ ! -d lib ]]; then mkdir lib; fi;
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@
	$(CXX) $(CXXFLAGS) -MM -MP -MT $@ $< -o $(@:.o=.d)

obj/bin/%.o: src/%.cpp
	if [[ ! -d obj/bin ]]; then mkdir -p obj/bin; fi; if [[ ! -d bin ]]; then mkdir bin; fi;
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@
	$(CXX) $(CXXFLAGS) -MM -MP -MT $@ $< -o $(@:.o=.d)


lib/CLFVAnalysis_xr.cc:
	genreflex src/classes.h -s src/classes_def.xml -o lib/CLFVAnalysis_xr.cc --deep --fail_on_warnings \
	--rootmap=lib/CLFVAnalysis_xr.rootmap --rootmap-lib=lib/libCLFVAnalysis.so --capabilities=lib/CLFVAnalysis_xi.cc \
	-DCMS_DICT_IMPL -D_REENTRANT -DGNUSOURCE -D__STRICT_ANSI__ -DGNU_GCC -D_GNU_SOURCE \
	-I../../ -DCMSSW_REFLEX_DICT

lib/CLFVAnalysis_xr.cc.o: lib/CLFVAnalysis_xr.cc
	c++ -MMD -MF lib/CLFVAnalysis_xr.cc.d -c -I../../ -I./ -DGNU_GCC -D_GNU_SOURCE $(shell root-config --cflags)  \
	-DCMSSW_REFLEX_DICT -pthread -Os -Wno-unused-variable -g -fPIC lib/CLFVAnalysis_xr.cc -o lib/CLFVAnalysis_xr.cc.o


$(LIB): $(OBJS) lib/CLFVAnalysis_xr.cc.o
	$(LD) $(LDFLAGS) -o $@ $^

bin/%: obj/bin/%.o $(LIB)
	$(CXX) -o $@ $^ $(LDFLAGS)

all: $(LIB)

test: $(LIB) $(BINS)

# $(CXX) $(CXXFLAGS) -fPIC -c src/test.cpp -o obj/lib/test.o
# $(CXX) -o test.exe $(OBJS) test.o $(LDFLAGS) $(LIB)
# $(CXX) $(CXXFLAGS) $(shell root-config --libs) -L$(LIB) -pedantic test.cpp -o test.exe

.SECONDARY: $(BINOBJS) obj/lib/CLFVAnalysis_xr.cc
.PHONY: all clean
.DEFAULT_GOAL := all

clean:
	rm obj/lib/* lib/* src/*~ interface/*~
