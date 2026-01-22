ROOT := $(abspath ../../../../..)
WOLF_SV_PARSER ?= $(ROOT)/build/bin/wolf-sv-parser
FILELIST := $(CURDIR)/filelist.f
TOP := tb_case_001
OUT := $(CURDIR)/wolf_emit.sv

.PHONY: run clean

run:
	@if [ ! -x "$(WOLF_SV_PARSER)" ]; then \
		echo "[case_001] wolf-sv-parser not found: $(WOLF_SV_PARSER)"; \
		echo "[case_001] Build it from repo root: cmake -S . -B build && cmake --build build --target wolf-sv-parser"; \
		exit 1; \
	fi
	@$(WOLF_SV_PARSER) --emit-sv --top $(TOP) -o $(OUT) -f $(FILELIST)

clean:
	@rm -f $(OUT)
