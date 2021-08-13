# ffbase makefile
# 2020, Simon Zolin

SRCDIR := ..
HDR := $(wildcard $(SRCDIR)/ffbase/*.h)

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
	\
	./time.o \
	./compat.o
HDR += $(SRCDIR)/test/test.h $(SRCDIR)/test/Makefile

all: $(TESTER)

clean:
	$(RM) $(TESTER) $(TEST_OBJ)

TEST_CFLAGS := -I$(SRCDIR) \
	-Wall -Wextra -Wno-return-local-addr -Wno-stringop-overflow \
	-DFF_DEBUG -g
ifeq "$(OPT)" "0"
	TEST_CFLAGS += -O0 -DFFBASE_DEBUG
else
	TEST_CFLAGS += -O3
endif
TEST_CXXFLAGS := $(TEST_CFLAGS)
TEST_CFLAGS += -std=gnu99
# TEST_CFLAGS += -fsanitize=address
# TEST_LDFLAGS += -fsanitize=address

./str.o: $(SRCDIR)/test/str.c $(HDR)
	$(C) -Wno-array-bounds $(TEST_CFLAGS) $< -o $@

./%.o: $(SRCDIR)/test/%.c $(HDR)
	$(C) $(TEST_CFLAGS) $< -o $@

./%.o: $(SRCDIR)/test/%.cpp $(HDR)
	$(CXX) $(TEST_CXXFLAGS) $< -o $@

$(TESTER): $(TEST_OBJ)
	$(LINK) $(TEST_LDFLAGS) $+ -o $@
