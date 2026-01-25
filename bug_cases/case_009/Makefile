ROOT := $(abspath ../../../../..)
WOLF_SV_PARSER ?= $(ROOT)/build/bin/wolf-sv-parser
FILELIST := $(CURDIR)/filelist.f
TOP := tb_case_009
OUT_DIR := $(ROOT)/build/c910_bug_case/case_009
WOLF_DIR := $(OUT_DIR)/wolf
RTL_DIR := $(OUT_DIR)/rtl
OUT := $(WOLF_DIR)/wolf_emit.sv

DUT_TOP := cpu_sub_system_axi
TB_SRC := $(CURDIR)/tb_case_009.cpp
VERILATOR ?= verilator
VERILATOR_FLAGS ?= --no-timing -Wall -Wno-fatal -Wno-DECLFILENAME -Wno-UNUSEDSIGNAL \
	-Wno-UNDRIVEN -Wno-SYNCASYNCNET -Wno-PINCONNECTEMPTY -Wno-ASSIGNDLY -Wno-MULTIDRIVEN
SIM_BIN_NAME := sim_case_009
RTL_SIM_BIN := $(RTL_DIR)/$(SIM_BIN_NAME)
WOLF_SIM_BIN := $(WOLF_DIR)/$(SIM_BIN_NAME)
VERILATOR_PREFIX := V$(DUT_TOP)
RTL_VERILATOR_MK := $(RTL_DIR)/$(VERILATOR_PREFIX).mk
WOLF_VERILATOR_MK := $(WOLF_DIR)/$(VERILATOR_PREFIX).mk

.PHONY: run run_verilator run_wolf_sv_parser_verilator clean

run:
	@if [ ! -x "$(WOLF_SV_PARSER)" ]; then \
		echo "[case_009] wolf-sv-parser not found: $(WOLF_SV_PARSER)"; \
		echo "[case_009] Build it from repo root: cmake -S . -B build && cmake --build build --target wolf-sv-parser"; \
		exit 1; \
	fi
	@mkdir -p $(WOLF_DIR)
	@$(WOLF_SV_PARSER) --emit-sv --emit-json --top $(TOP) -o $(OUT) -f $(FILELIST)

$(RTL_SIM_BIN): $(TB_SRC) $(FILELIST)
	@if ! command -v $(VERILATOR) >/dev/null 2>&1; then \
		echo "[case_009] verilator not found in PATH"; \
		exit 1; \
	fi
	@mkdir -p $(RTL_DIR)
	@$(VERILATOR) $(VERILATOR_FLAGS) --cc -f $(FILELIST) --exe $(TB_SRC) \
		--top-module $(DUT_TOP) -Mdir $(RTL_DIR) -o $(SIM_BIN_NAME)
	CCACHE_DISABLE=1 $(MAKE) -C $(RTL_DIR) -f $(RTL_VERILATOR_MK) $(SIM_BIN_NAME)

$(WOLF_SIM_BIN): $(TB_SRC)
	@if [ ! -x "$(WOLF_SV_PARSER)" ]; then \
		echo "[case_009] wolf-sv-parser not found: $(WOLF_SV_PARSER)"; \
		echo "[case_009] Build it from repo root: cmake -S . -B build && cmake --build build --target wolf-sv-parser"; \
		exit 1; \
	fi
	@if ! command -v $(VERILATOR) >/dev/null 2>&1; then \
		echo "[case_009] verilator not found in PATH"; \
		exit 1; \
	fi
	@mkdir -p $(WOLF_DIR)
	@$(WOLF_SV_PARSER) --emit-sv --emit-json --top $(DUT_TOP) -o $(OUT) -f $(FILELIST)
	@$(VERILATOR) $(VERILATOR_FLAGS) --cc $(OUT) --exe $(TB_SRC) \
		--top-module $(DUT_TOP) -Mdir $(WOLF_DIR) -o $(SIM_BIN_NAME)
	CCACHE_DISABLE=1 $(MAKE) -C $(WOLF_DIR) -f $(WOLF_VERILATOR_MK) $(SIM_BIN_NAME)

run_verilator: $(RTL_SIM_BIN)
	@echo "[RUN] ./$(RTL_SIM_BIN)"
	@cd $(RTL_DIR) && ./$(SIM_BIN_NAME)

run_wolf_sv_parser_verilator: $(WOLF_SIM_BIN)
	@echo "[RUN] ./$(WOLF_SIM_BIN)"
	@cd $(WOLF_DIR) && ./$(SIM_BIN_NAME)

clean:
	@rm -rf $(OUT_DIR)
