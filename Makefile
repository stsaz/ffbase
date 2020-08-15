# ffbase makefile
# 2020, Simon Zolin

# set OS
ifndef $(OS)
	uname := $(shell uname)
	ifeq ($(uname),Linux)
		OS := linux
	else ifeq ($(uname),FreeBSD)
		OS := freebsd
	else ifeq ($(uname),Darwin)
		OS := apple
	else
		OS := win
	endif
endif

# set compiler
CPREFIX :=
# CPREFIX := x86_64-w64-mingw32-
COMPILER := gcc
ifeq ($(OS),freebsd)
	COMPILER := clang
endif
ifeq ($(OS),apple)
	COMPILER := clang
endif
C := $(CPREFIX)gcc -c
CXX := $(CPREFIX)g++ -c
LINKER := $(CPREFIX)gcc
ifeq ($(COMPILER),clang)
	C := clang -c
	CXX := clang++ -c
	LINKER := clang
endif

# set utils
RM := rm -f

SRCDIR := .
HDR := $(wildcard $(SRCDIR)/ffbase/*.h)

TEST_OUTDIR := $(SRCDIR)/test

TESTER := $(TEST_OUTDIR)/fftest
ifeq ($(OS),win)
	TESTER := $(TEST_OUTDIR)/fftest.exe
endif

TEST_OBJ := \
	$(TEST_OUTDIR)/main.o \
	$(TEST_OUTDIR)/mem.o \
	\
	$(TEST_OUTDIR)/slice.o \
	$(TEST_OUTDIR)/vector.o \
	$(TEST_OUTDIR)/sort.o \
	\
	$(TEST_OUTDIR)/str.o \
	$(TEST_OUTDIR)/strz.o \
	$(TEST_OUTDIR)/str-conv.o \
	$(TEST_OUTDIR)/str-fmt.o \
	$(TEST_OUTDIR)/unicode.o \
	\
	$(TEST_OUTDIR)/list.o \
	$(TEST_OUTDIR)/rbtree.o \
	$(TEST_OUTDIR)/map.o \
	$(TEST_OUTDIR)/ring.o \
	\
	$(TEST_OUTDIR)/atomic.o \
	$(TEST_OUTDIR)/lock.o \
	\
	$(TEST_OUTDIR)/json.o \
	\
	$(TEST_OUTDIR)/time.o \
	$(TEST_OUTDIR)/compat.o

all: $(TESTER)

clean:
	$(RM) $(TESTER) $(TEST_OBJ)

TEST_CFLAGS := -I$(SRCDIR) -Wall -Wextra -Werror -fvisibility=hidden
TEST_CFLAGS += -DFF_DEBUG -O0 -g
TEST_CXXFLAGS := $(TEST_CFLAGS)
TEST_CFLAGS += -std=gnu99
# TEST_CFLAGS += -fsanitize=address
# TEST_LDFLAGS += -fsanitize=address

$(TEST_OUTDIR)/str.o: $(SRCDIR)/test/str.c $(HDR) $(SRCDIR)/test/test.h $(SRCDIR)/Makefile
	$(C) -Wno-array-bounds $(TEST_CFLAGS) $< -o $@

$(TEST_OUTDIR)/%.o: $(SRCDIR)/test/%.c $(HDR) $(SRCDIR)/test/test.h $(SRCDIR)/Makefile
	$(C) $(TEST_CFLAGS) $< -o $@

$(TEST_OUTDIR)/%.o: $(SRCDIR)/test/%.cpp $(HDR) $(SRCDIR)/test/test.h $(SRCDIR)/Makefile
	$(CXX) $(TEST_CXXFLAGS) $< -o $@

$(TESTER): $(TEST_OBJ)
	$(LINKER) $(TEST_LDFLAGS) $+ -o $@
