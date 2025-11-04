UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC=clang++
    CC+=-D_XOPEN_SOURCE -Wno-deprecated-declarations
    LIBCPU=libcpu_macos.o
else
    CC=g++
    LIBCPU=libcpu.o
endif

CC+=-g -Wall -std=c++17

EXECS ?= cpu_test

# List of source files for your thread library
THREAD_SOURCES=cpu.cpp thread.cpp cv.cpp mutex.cpp

# Generate the names of the thread library's object files
THREAD_OBJS=${THREAD_SOURCES:.cpp=.o}

all: libthread.o test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13 test14 test15

test: libthread.o ${EXECS}

# Compile the thread library and tag this compilation
libthread.o: ${THREAD_OBJS}
	./autotag.sh push
	ld -r -o $@ ${THREAD_OBJS}

${EXECS}: ${EXECS}.cpp libthread.o ${LIBCPU}
	${CC} -o ./build/$@ $^ -ldl -pthread

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${THREAD_OBJS} libthread.o ./build/*
