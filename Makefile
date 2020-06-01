.PHONY: all apps clean

HX_SRCs = $(shell hx-srcs.sh)
CXX_SRCs = $(filter %.cpp,$(shell hx-files.sh $(HX_SRCs)))
CXXFLAGS += -Wall -std=c++17 -O2
TARGET = riscv32-unknown-elf

all: hx_run

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
	@$(TARGET)-gcc -Wall -O2 -c $^ -o $@

mon-riscv.o: mon.cpp
	@echo C++ $@
	@$(TARGET)-g++ $(CXXFLAGS) -c $^ -o $@

mon-riscv: init.o mon-riscv.o
	@echo LD $@
	@$(TARGET)-ld -T memory.lds $^ -o $@

%.hex: %
	@echo HEX $@
	@$(TARGET)-objcopy $^ -O ihex $@

$(CXX_SRCs): hx_run

clean:
	@echo RM
	@rm -f *.o $(CXX_SRCs) hx_run mon mon-riscv

