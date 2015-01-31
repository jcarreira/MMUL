CXX = gcc
CXXFLAGS=-O3 -Wall -fno-asynchronous-unwind-tables
TARGET=mmul

all: mmul

assembly: mmul.c
	$(CXX) $(CXXFLAGS) -S $^

$(TARGET): mmul.c
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET) *~
