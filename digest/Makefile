# Name of your target
DEBUG        := 0

# Source filenames
SRCS     = \
../hash/src/sha1.c \
../hash/src/sha2_512.c \
../hash/src/tiger_coefs.c \
../hash/src/tiger_internal.c \
../hash/src/tiger.c \
../hash/src/hashtree.c \
../hash/src/md4.c \
../hash/src/md5.c

ifeq (${MAKECMDGOALS},check)
TARGET := unittest
SRCS += \
./tests/tests.c \
../unittest/src/unittest_main.c \
../hash/tests/simple_hash_test.c \
../hash/tests/hash_tests.c \
../hash/tests/md4_test.c \
../hash/tests/md5_test.c \
../hash/tests/sha1_test.c \
../hash/tests/sha2_512_test.c \
../hash/tests/tiger_test.c \
../hash/tests/tigertree_test.c
else
TARGET := digest
SRCS += ./src/digest.c
endif

# Compiler flags
CCFLAGS = \
-Wall \
-Wdouble-promotion \
-Wno-unused-function \
-msse3 \
-std=gnu99 \
-mtune=corei7 \
-pedantic \
-D_BSD_SOURCE \
-I..

LDFLAGS =
LIBS    =
CC      = gcc
CXX     = g++

ifneq ($(DEBUG),0)
CCFLAGS  += -O0 -ggdb3 -ftrapv
LDFLAGS  += -O0
else
CCFLAGS  += -DNDEBUG=1 -O3 -mtune=corei7 -fomit-frame-pointer -funroll-loops -funswitch-loops
LDFLAGS  += -O3
endif

CC_SRCS  := $(filter %.c,${SRCS})
CC_OBJS  := $(notdir ${CC_SRCS:.c=.o})
CC_DEPS  := $(notdir ${CC_SRCS:.c=.dep})
CXX_SRCS := $(filter %.cpp,${SRCS})
CXX_OBJS := $(notdir ${CXX_SRCS:.cpp=.o})
CXX_DEPS := $(notdir ${CXX_SRCS:.cpp=.dep})
OBJS     := ${CC_OBJS} ${CXX_OBJS}
DEPS     := ${CC_DEPS} ${CXX_DEPS}
XDEPS    := $(wildcard ${DEPS})

.PHONY: all test clean distclean

all: $(TARGET)

check: $(TARGET)
	@./$(TARGET)

ifneq (${XDEPS},)
include ${XDEPS}
endif 

$(TARGET): ${OBJS}
ifeq (${CXX_OBJS},)
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}
else
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}
endif

.SECONDEXPANSION:

${CXX_OBJS}: %.o: $$(foreach src,$${CXX_SRCS},$$(if $$(findstring $$(strip %.cpp),$$(strip $${src})),$${src},)) %.dep
	${CXX} ${CXXFLAGS} -o $@ -c $<

${CXX_DEPS}: %.dep: $$(foreach src,$${CXX_SRCS},$$(if $$(findstring $$(strip %.cpp),$$(strip $${src})),$${src},)) Makefile
	${CXX} ${CXXFLAGS} -MM $< > $@

${CC_OBJS}: %.o: $$(foreach src,$${CC_SRCS},$$(if $$(findstring $$(strip %.c),$$(strip $${src})),$${src},)) %.dep
	${CC} ${CCFLAGS} -o $@ -c $<

${CC_DEPS}: %.dep: $$(foreach src,$${CC_SRCS},$$(if $$(findstring $$(strip %.c),$$(strip $${src})),$${src},)) Makefile
	${CC} ${CCFLAGS} -MM $< > $@

clean:
	-rm -f *.o *.dep $(GEN)

distclean:: clean
