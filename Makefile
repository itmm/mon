.PHONY: all clean

HX_SRCs = $(shell hx-srcs.sh)
CXX_SRCs = $(filter %.cpp,$(shell hx-files.sh $(HX_SRCs)))
CXXFLAGS += -Wall --pedantic -std=c++17

all: hx_run

hx_run: $(HX_SRCs)
	@echo HX
	@hx
	@date >$@
	@make --no-print-directory mon

mon: $(CXX_SRCs)
	@echo C++ $@
	@$(CXX) $(CXXFLAGS) $^ -o $@

$(CXX_SRCs): hx_run

clean:
	@echo RM
	@rm -f $(CXX_SRCs) hx_run mon

