# ffbase makefile
# 2020, Simon Zolin

SRCDIR := ..
HDR := $(wildcard $(SRCDIR)/ffbase/*.h)
HDR += $(SRCDIR)/test/test.h $(SRCDIR)/test/Makefile

include $(SRCDIR)/test/makeconf

TESTER := ./fftest$(DOTEXE)

TEST_OBJ := \
	./main.o \
	./mem.o \
	\
	./slice.o \
	./vector.o \
	./sort.o \
	\
	./str.o \
	./strz.o \
	./str-conv.o \
	./str-fmt.o \
	./unicode.o \
	\
	./list.o \
	./rbtree.o \
	./map.o \
	./ring.o \
	\
	./atomic.o \
	./lock.o \
	\
	./json.o \
	./args.o \
	\
	./time.o \
	./cpu.o \
	./compat.o

all: $(TESTER)

clean:
	$(RM) $(TESTER) $(TEST_OBJ)

TEST_CFLAGS := -I$(SRCDIR) \
	-Wall -Wextra -Wno-return-local-addr \
	-DFF_DEBUG -g
ifeq "$(COMPILER)" "gcc"
	TEST_CFLAGS += -Wno-stringop-overflow
endif
TEST_CFLAGS += -msse4.2
ifeq "$(DEBUG)" "1"
	TEST_CFLAGS += -O0 -DFFBASE_DEBUG
else
	TEST_CFLAGS += -O3 -fno-strict-aliasing
endif
TEST_CXXFLAGS := $(TEST_CFLAGS)
TEST_CFLAGS += -std=gnu99

ifeq "$(ASAN)" "1"
	TEST_CFLAGS += -fsanitize=address
	TEST_LDFLAGS += -fsanitize=address
endif

./str.o: $(SRCDIR)/test/str.c $(HDR)
	$(C) -Wno-array-bounds $(TEST_CFLAGS) $< -o $@

./%.o: $(SRCDIR)/test/%.c $(HDR)
	$(C) $(TEST_CFLAGS) $< -o $@

./%.o: $(SRCDIR)/test/%.cpp $(HDR)
	$(CXX) $(TEST_CXXFLAGS) $< -o $@

$(TESTER): $(TEST_OBJ)
	$(LINK) $(TEST_LDFLAGS) $+ -o $@
