# Simple Verilator hook for wolf_sv_parser_tb cases

CASE ?= 001
CASE_DIR := $(CURDIR)/wolf_sv_parser_tb/case$(CASE)
BUILD_DIR := $(CURDIR)/build/case$(CASE)
PLAN_FILE ?= $(CURDIR)/wolf_sv_parser_tb/plan.md
CASE_NUM := $(shell echo $(CASE) | sed 's/^0*//')
TOP_FROM_PLAN := $(shell sed -n '/^$(CASE_NUM)\./s/^[0-9][0-9]*\. *//p' $(PLAN_FILE) | head -n1 | xargs basename | sed 's/\.v$$//')
TOP ?= $(if $(TOP_FROM_PLAN),$(TOP_FROM_PLAN),ct_fadd_close_s0_d)
COVER ?= 1
COV_DAT := $(BUILD_DIR)/coverage.dat
COV_INFO := $(BUILD_DIR)/coverage.info
COV_ANNOTATE_DIR := $(BUILD_DIR)/annot

# Root for RTL, used inside filelist templates
C910_PROJ ?= $(CURDIR)

.PHONY: run
run: $(BUILD_DIR)/sim
	COV_OUT=$(COV_DAT) $(BUILD_DIR)/sim
	verilator_coverage --write-info $(COV_INFO) $(COV_DAT)
	verilator_coverage --annotate-min 1 --annotate $(COV_ANNOTATE_DIR) $(COV_DAT)

$(BUILD_DIR)/sim: $(CASE_DIR)/filelist.f $(CASE_DIR)/tb.cpp
	@mkdir -p $(BUILD_DIR)
	@C910_PROJ=$(C910_PROJ) envsubst < $(CASE_DIR)/filelist.f > $(BUILD_DIR)/filelist.f
	CCACHE_DISABLE=1 VERILATOR_NO_CCACHE=1 VERILATOR_CXX=g++ verilator --cc --exe --build -Mdir $(BUILD_DIR) \
	  $(if $(COVER),--coverage,) \
	  $(VERILATOR_FLAGS) \
	  -o $(BUILD_DIR)/sim \
	  -CFLAGS "-std=c++17" \
	  --top-module $(TOP) \
	  -f $(BUILD_DIR)/filelist.f \
	  $(CASE_DIR)/tb.cpp

.PHONY: clean
clean:
	rm -rf build
