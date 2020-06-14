.PHONY: all apps clean

HX_SRCs = $(shell hx-srcs.sh)
SRCs = $(shell hx-files.sh $(HX_SRCs))

CC = clang-9
CXX = clang++-9
LD = ld.lld-9

CXXFLAGS += -Wall --pedantic -std=c++17 -Os
RISCARCH = --target=riscv32 -march=rv32imac
RISCFLAGS = $(CXXFLAGS) -fno-exceptions $(RISCARCH)

all: hx_run apps

apps: mon mon-riscv.hex

hx_run: $(HX_SRCs)
	@echo HX
	@hx
	@date >$@
	@make --no-print-directory apps

mon: mon.cpp
	@echo C++ $@
	@$(CXX) $(CXXFLAGS) -DUNIX_APP=1 $^ -o $@

init.o: init.s
	@echo AS $@
	@$(CC) -Wall -Os $(RISCARCH) -c $^ -o $@

mon-riscv.o: mon.cpp
	@echo C++ $@
	@$(CXX) $(RISCFLAGS) -c $^ -o $@

mon-riscv: init.o mon-riscv.o
	@echo LD $@
	@$(LD) -T memory.lds $^ -o $@

%.hex: %
	@echo HEX $@
	@objcopy -j.text $^ -O ihex $@

$(SRCs): hx_run

clean:
	@echo RM
	@rm -f $(SRCs) *.o mon-riscv.hex mon-riscv mon hx_run

