CXX = mpicxx
CXXFLAGS = -std=c++17 -O3 -march=native -m64 -I/home/sc21/sc21s121/homework/hw1/include
TARGETS = hw1

.PHONY: all
all: $(TARGETS)

hw1: hw1.cc
	$(CXX) $(CXXFLAGS) hw1.cc /home/sc21/sc21s121/libmimalloc.a -o hw1

.PHONY: clean
clean:
	rm -f $(TARGETS)
