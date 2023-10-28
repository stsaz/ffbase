# ffbase makefile

FFBASE := ..

include $(FFBASE)/conf.mk

TESTER := fftest$(DOTEXE)

OBJ := \
	main.o \
	mem.o \
	\
	slice.o \
	vector.o \
	sort.o \
	\
	str.o \
	strz.o \
	str-conv.o \
	str-fmt.o \
	unicode.o \
	\
	list.o \
	rbtree.o \
	map.o \
	ring.o \
	\
	atomic.o \
	lock.o \
	\
	json.o \
	conf.o \
	args.o \
	\
	time.o \
	cpu.o \
	compat.o

default: $(TESTER)

clean:
	$(RM) $(TESTER) $(OBJ)

CFLAGS := -MMD -MP -I$(FFBASE) \
	-Wall -Wextra -Wno-return-local-addr -Wno-multichar \
	-DFF_DEBUG -g
ifeq "$(COMPILER)" "gcc"
	CFLAGS += -Wno-stringop-overflow
endif
CFLAGS += -msse4.2
ifeq "$(DEBUG)" "1"
	CFLAGS += -O0 -DFFBASE_DEBUG
else
	CFLAGS += -O3 -fno-strict-aliasing
endif
CXXFLAGS := $(CFLAGS)
CFLAGS += -std=gnu99

ifeq "$(ASAN)" "1"
	CFLAGS += -fsanitize=address
	LINKFLAGS += -fsanitize=address
endif

-include $(wildcard *.d)

str.o: $(FFBASE)/test/str.c
	$(C) -Wno-array-bounds $(CFLAGS) $< -o $@

%.o: $(FFBASE)/test/%.c
	$(C) $(CFLAGS) $< -o $@

%.o: $(FFBASE)/test/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(TESTER): $(OBJ)
	$(LINK) $+ $(LINKFLAGS) -o $@
