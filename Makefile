PROJECT=sm
AUTHOR=Avril (Flanchan) <flanchan@cumallover.me>

SRC_C   = $(wildcard src/*.c)
SRC_CXX = $(wildcard src/*.cpp)

TEST_SRC_C   = $(wildcard src/test/*.c)
TEST_SRC_CXX = $(wildcard src/test/*.cpp)

INCLUDE=include

COMMON_FLAGS= -W -Wall -pedantic -fno-strict-aliasing $(addprefix -I,$(INCLUDE))

OPT_FLAGS?= -march=native -fgraphite -fopenmp -floop-parallelize-all -ftree-parallelize-loops=4 \
	    -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block \
	    -fno-stack-check

CXX_OPT_FLAGS?= $(OPT_FLAGS) -felide-constructors

CFLAGS   += $(COMMON_FLAGS) --std=gnu11
CXXFLAGS += $(COMMON_FLAGS) --std=gnu++20 -fno-exceptions
LDFLAGS  +=  

STRIP=strip

RELEASE_CFLAGS?=   -O3 -flto $(OPT_FLAGS)
RELEASE_CXXFLAGS?= -O3 -flto $(CXX_OPT_FLAGS)
RELEASE_LDFLAGS?=  -O3 -flto

DEBUG_CFLAGS?= -O0 -g -DDEBUG
DEBUG_CXXFLAGS?= $(DEBUG_CFLAGS)
DEBUG_LDFLAGS?=

# Objects

OBJ_C   = $(addprefix obj/c/,$(SRC_C:.c=.o))
OBJ_CXX = $(addprefix obj/cxx/,$(SRC_CXX:.cpp=.o))
OBJ = $(OBJ_C) $(OBJ_CXX)

TEST_OBJ_C   = $(addprefix obj/c/,$(TEST_SRC_C:.c=.o))
TEST_OBJ_CXX = $(addprefix obj/cxx/,$(TEST_SRC_CXX:.cpp=.o))
TEST_OBJ = $(TEST_OBJ_C) $(TEST_OBJ_CXX)

# Phonies

.PHONY: release
release: | dirs lib$(PROJECT).so lib$(PROJECT).a

.PHONY: debug
debug: | dirs lib$(PROJECT)-debug.so lib$(PROJECT)-debug.a

.PHONY: test
test: | dirs $(PROJECT)-test

# Targets

dirs:
	@mkdir -p obj/c{,xx}/src{,/test}

obj/c/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@ $(LDFLAGS)

obj/cxx/%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@ $(LDFLAGS)

lib$(PROJECT).a: CFLAGS+= $(RELEASE_CFLAGS)
lib$(PROJECT).a: CXXFLAGS += $(RELEASE_CXXFLAGS)
lib$(PROJECT).a: $(OBJ)
	ar -rcs $@ $^
	ranlib $@

lib$(PROJECT).so: CFLAGS+= -fPIC $(RELEASE_CFLAGS)
lib$(PROJECT).so: CXXFLAGS += -fPIC $(RELEASE_CXXFLAGS)
lib$(PROJECT).so: LDFLAGS += $(RELEASE_LDFLAGS)
lib$(PROJECT).so: $(OBJ)
	$(CXX) -shared $^ $(CXXFLAGS) -o $@ $(LDFLAGS)
	$(STRIP) $@

lib$(PROJECT)-debug.a: CFLAGS+= $(DEBUG_CFLAGS)
lib$(PROJECT)-debug.a: CXXFLAGS += $(DEBUG_CXXFLAGS)
lib$(PROJECT)-debug.a: $(OBJ)
	ar -rcs $@ $^
	ranlib $@

lib$(PROJECT)-debug.so: CFLAGS+= -fPIC $(DEBUG_CFLAGS)
lib$(PROJECT)-debug.so: CXXFLAGS += -fPIC $(DEBUG_CXXFLAGS)
lib$(PROJECT)-debug.so: LDFLAGS += $(DEBUG_LDFLAGS)
lib$(PROJECT)-debug.so: $(OBJ)
	$(CXX) -shared $^ $(CXXFLAGS) -o $@ $(LDFLAGS)

$(PROJECT)-test-static: $(TEST_OBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@ -L. -l:lib$(PROJECT).a $(LDFLAGS)

$(PROJECT)-test: lib$(PROJECT).a
	$(MAKE) $(PROJECT)-test-static
	mv $(PROJECT)-test-static $@
	./$@

clean-rebuild:
	rm -rf obj

clean: clean-rebuild
	rm -f lib$(PROJECT){-debug,}.{so,a}
	rm -f $(PROJECT)-test
