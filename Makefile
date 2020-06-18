.PHONY: all apps clean

HX_SRCs = $(shell hx-srcs.sh)
SRCs = $(shell hx-files.sh $(HX_SRCs))

RISCARCH = --target=riscv32 -march=rv32imac
RISCFLAGS = $(CXXFLAGS) -fno-exceptions $(RISCARCH)
CC = clang-9 $(RISCARCH)
CXX = clang++-9
RCXX = clang++-9 $(RISCFLAGS)
LD = ld.lld-9
#TARGET = riscv32-unknown-elf-
#CC = $(TARGET)gcc
#RCXX = $(TARGET)c++
#LD = $(TARGET)ld

CXXFLAGS += -Wall --pedantic -std=c++17 -Os

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
	@$(CC) -Wall -Os -c $^ -o $@

mon-riscv.o: mon.cpp
	@echo C++ $@
	@$(RCXX) -c $^ -o $@

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

