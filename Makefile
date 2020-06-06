.PHONY: all apps clean

HX_SRCs = $(shell hx-srcs.sh)
CXX_SRCs = $(filter %.cpp,$(shell hx-files.sh $(HX_SRCs)))
CXXFLAGS += -Wall -std=c++17 -O2
CC = clang-9
CXX = clang++-9
LD = ld.lld-9
TARGET = riscv32-unknown-elf

all: hx_run mon mon-riscv.hex

hx_run: $(HX_SRCs)
	@echo HX
	@hx
	@date >$@
	@make --no-print-directory apps

apps: mon mon-riscv.hex

mon: $(CXX_SRCs)
	@echo C++ $@
	@$(CXX) $(CXXFLAGS) -DUNIX_APP=1 $^ -o $@

init.o: init.S
	@echo AS $@
	@$(CC) -Wall -O2 --target=riscv32 -march=rv32imac -c $^ -o $@

mon-riscv.o: mon.cpp
	@echo C++ $@
	@$(CXX) $(CXXFLAGS) -fno-exceptions --target=riscv32 -march=rv32imac -c $^ -o $@

mon-riscv.s: mon.cpp
	@echo C++ -S $@
	@$(CXX) $(CXXFLAGS) -fno-exceptions --target=riscv32 -march=rv32imac -S $^ -o $@

mon-riscv: init.o mon-riscv.o
	@echo LD $@
	@$(LD) -T memory.lds $^ -o $@

%.hex: %
	@echo HEX $@
	@objcopy $^ -O ihex $@

$(CXX_SRCs): hx_run

clean:
	@echo RM
	@rm -f *.o mon-riscv.hex mon-riscv.s $(CXX_SRCs) hx_run mon mon-riscv

