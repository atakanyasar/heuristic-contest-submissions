CXXFLAGS += -std=c++17
CXXFLAGS += -g
DIR = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
MAIN = $(DIR)main.cpp
EXE = $(DIR)main.exe
INPUT = $(DIR)in/0000.txt
OUTPUT = $(DIR)out.txt

default:
	run
all:
	run
compile:
	$(CXX) $(CXXFLAGS) $(MAIN) -o $(EXE)
renew:
	$(shell cargo run --manifest-path $(DIR)tools/Cargo.toml --release --bin gen $(DIR)/tools/seeds.txt)
run: renew compile
	$(shell cat $(INPUT) | cargo run --manifest-path $(DIR)tools/Cargo.toml --release --bin tester $(EXE) > $(OUTPUT))

compile_single_hot_point:
	$(CXX) $(CXXFLAGS) $(DIR)single_hot_point_solution.cpp -o $(EXE)
run_single_hot_point: renew compile_single_hot_point
	$(shell cat $(INPUT) | cargo run --manifest-path $(DIR)tools/Cargo.toml --release --bin tester $(EXE) > $(OUTPUT))

compile_thermal_pit:
	$(CXX) $(CXXFLAGS) $(DIR)thermal_pit_solution.cpp -o $(EXE)
run_thermal_pit: renew compile_thermal_pit
	$(shell cat $(INPUT) | cargo run --manifest-path $(DIR)tools/Cargo.toml --release --bin tester $(EXE) > $(OUTPUT))

compile_compatibility_matrix:
	$(CXX) $(CXXFLAGS) $(DIR)compatibility_matrix_solution.cpp -o $(EXE)
run_compatibility_matrix: renew compile_compatibility_matrix
	$(shell cat $(INPUT) | cargo run --manifest-path $(DIR)tools/Cargo.toml --release --bin tester $(EXE) > $(OUTPUT))

compile_bitmask:
	$(CXX) $(CXXFLAGS) $(DIR)bitmask_solution.cpp -o $(EXE)
run_bitmask: renew compile_bitmask
	$(shell cat $(INPUT) | cargo run --manifest-path $(DIR)tools/Cargo.toml --release --bin tester $(EXE) > $(OUTPUT))