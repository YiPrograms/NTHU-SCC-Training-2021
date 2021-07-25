CC = gcc
CXX = g++
CFLAGS = -lm -O3 -march=native -m64
lab2: CFLAGS += -pthread
CXXFLAGS = $(CFLAGS)
TARGETS = lab2

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS) $(TARGETS:=.o)
