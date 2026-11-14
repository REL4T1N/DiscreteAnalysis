CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

.PHONY: all bench report
all: main.out benchmark.out
main.out: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
benchmark.out: benchmark.cpp main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
bench: benchmark.out
	python3 benchmark_tables.py
report:
	$(MAKE) -C report pdf
