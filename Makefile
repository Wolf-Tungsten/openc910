# Simple Verilator hook for wolf_sv_parser_tb cases

CASE ?= 001
CASE_DIR := $(CURDIR)/wolf_sv_parser_tb/case$(CASE)
BUILD_DIR := $(CURDIR)/build/case$(CASE)
TOP ?= ct_fadd_close_s0_d
COVER ?= 1

# Root for RTL, used inside filelist templates
C910_PROJ ?= $(CURDIR)

.PHONY: run
run: $(BUILD_DIR)/sim
	COV_OUT=$(BUILD_DIR)/coverage.dat $(BUILD_DIR)/sim
	verilator_coverage --write-info $(BUILD_DIR)/coverage.info $(BUILD_DIR)/coverage.dat

$(BUILD_DIR)/sim: $(CASE_DIR)/filelist.f $(CASE_DIR)/tb.cpp
	@mkdir -p $(BUILD_DIR)
	@C910_PROJ=$(C910_PROJ) envsubst < $(CASE_DIR)/filelist.f > $(BUILD_DIR)/filelist.f
	CCACHE_DISABLE=1 VERILATOR_NO_CCACHE=1 VERILATOR_CXX=g++ verilator --cc --exe --build -Mdir $(BUILD_DIR) \
	  $(if $(COVER),--coverage,) \
	  -o $(BUILD_DIR)/sim \
	  -CFLAGS "-std=c++17" \
	  --top-module $(TOP) \
	  -f $(BUILD_DIR)/filelist.f \
	  $(CASE_DIR)/tb.cpp

.PHONY: clean
clean:
	rm -rf build
