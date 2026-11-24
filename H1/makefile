UNAME_S := $(shell uname -s)
LOCAL := $(HOME)/.local
LIBDIR := lib

ifeq ($(UNAME_S),Darwin)
CC := clang
CXX := clang++
BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
NEP_LIB := $(LIBDIR)/libnep.dylib
INCLUDE_FLAGS := $(if $(BREW_PREFIX),-I$(BREW_PREFIX)/include)
LIBRARY_FLAGS := $(if $(BREW_PREFIX),-L$(BREW_PREFIX)/lib)
RPATH_FLAGS := -Wl,-rpath,@executable_path/$(LIBDIR)
NEP_SHARED := -dynamiclib -install_name @rpath/libnep.dylib
else
CC := gcc
CXX := g++
NEP_LIB := $(LIBDIR)/libnep.so
INCLUDE_FLAGS :=
LIBRARY_FLAGS :=
RPATH_FLAGS := -Wl,-rpath,'$$ORIGIN/$(LIBDIR)'
NEP_SHARED := -shared
endif

CFLAGS := -g -pedantic -Werror -Wall -Iinclude -I$(LOCAL)/include -Isrc/nepc $(INCLUDE_FLAGS)
LDFLAGS := $(LIBRARY_FLAGS) $(RPATH_FLAGS)
CFLAGS_OPT := -O2
ifeq ($(UNAME_S),Linux)
CFLAGS_OPT += -march=native
endif
LOCAL_LIB_FLAG := $(if $(wildcard $(LOCAL)/lib),-L$(LOCAL)/lib)

LIB := $(LOCAL_LIB_FLAG) -L$(LIBDIR) -lm -lgsl -lgslcblas -lnep
OBJ := obj/tools.o obj/lattice.o obj/potential.o obj/task1.o obj/task2.o obj/run.o
OBJ_LIB := obj_nep/nep.o obj_nep/interface.o
CXXFLAGS := -DUSE_TABLE_FOR_RADIAL_FUNCTIONS -O3 -fPIC -Iinclude -Isrc/nepc
MAIN := obj/main.o

ifeq ($(MAKECMDGOALS),test)
OBJ_TEST := $(patsubst obj/%.o,obj_test/%.o,$(OBJ))
-include unit-test/test.mk
else
CFLAGS += $(CFLAGS_OPT)
endif

.PHONY: all lib clean test

all: H1

H1: $(MAIN) $(OBJ) $(NEP_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(MAIN) $(OBJ) $(LIB) -o $@

obj/%.o: src/%.c | obj
	$(CC) -MMD -c $(CFLAGS) $< -o $@

obj_nep/%.o: src/nepc/%.cpp | obj
	$(CXX) -MMD -c $(CXXFLAGS) $< -o $@

$(NEP_LIB): $(OBJ_LIB) | obj
	$(CXX) $(NEP_SHARED) $(OBJ_LIB) -o $@

lib: $(NEP_LIB)

obj:
	mkdir -p obj obj_nep obj_test $(LIBDIR)

clean:
	rm -rf obj obj_test obj_nep $(LIBDIR)
	rm -f H1 run-test

-include $(OBJ:.o=.d)
