CXX      := g++
LD       := g++
CXXFLAGS := -Wall -Wextra -Werror -O2 -std=c++1y -g -I./ $(shell root-config --cflags) -lTMVA
LDFLAGS  := -shared -Wall -Wextra $(shell root-config --libs) -lTMVA
SRC 	 := $(wildcard src/*.cc)
OBJS     := $(SRC:src/%.cc=obj/lib/%.o)
LIB 	 := lib/libCLFVAnalysis.so

-include $(OBJS:.o=.d)

obj/lib/%.o: src/%.cc
	if [[ ! -d obj/lib ]]; then mkdir -p obj/lib; fi; if [[ ! -d lib ]]; then mkdir lib; fi;
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
	$(CXX) -o $@ $^ $(shell root-config --libs)

all: $(LIB)

.SECONDARY: $(BINOBJS) obj/lib/CLFVAnalysis_xr.cc
.PHONY: all clean
.DEFAULT_GOAL := all

clean:
	rm obj/lib/* lib/* src/*~ interface/*~
