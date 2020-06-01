.PHONY: all apps clean

HX_SRCs = $(shell hx-srcs.sh)
CXX_SRCs = $(filter %.cpp,$(shell hx-files.sh $(HX_SRCs)))
CXXFLAGS += -Wall --pedantic -std=c++17
CXXRISCV = riscv32-unknown-elf-g++

all: hx_run

hx_run: $(HX_SRCs)
	@echo HX
	@hx
	@date >$@
	@make --no-print-directory apps

apps: mon mon-riscv

mon: $(CXX_SRCs)
	@echo C++ $@
	@$(CXX) $(CXXFLAGS) -DUNIX_APP=1 $^ -o $@

mon-riscv: $(CXX_SRCs)
	@echo C++ $@
	@$(CXXRISCV) $(CXXFLAGS) $^ -o $@

$(CXX_SRCs): hx_run

clean:
	@echo RM
	@rm -f $(CXX_SRCs) hx_run mon mon-riscv

