# ffbase makefile
# 2020, Simon Zolin

OS := unix
CPREFIX :=
# CPREFIX := x86_64-w64-mingw32-
C := $(CPREFIX)gcc -c
CXX := $(CPREFIX)g++ -c
LINKER := $(CPREFIX)gcc
# C := $(CPREFIX)clang -c
# CXX := $(CPREFIX)clang++ -c
# LINKER := $(CPREFIX)clang
RM := rm -f

SRCDIR := .
HDR := $(wildcard $(SRCDIR)/ffbase/*.h)

TESTER := fftest
ifeq ($(OS),win)
TESTER := fftest.exe
endif
TEST_OUTDIR := /tmp/ffbase
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
TEST_CXXFLAGS := -std=gnu++03 $(TEST_CFLAGS)
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
