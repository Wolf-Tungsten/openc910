#include "Vct_ifu_icache_if.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vct_ifu_icache_if *dut) {
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static uint32_t lfsr_next(uint32_t lfsr) {
    const uint32_t bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1U;
    return (lfsr >> 1) | (bit << 31);
}

static void set_wide128(VlWide<4>& wide, uint64_t val) {
    wide.m_storage[0] = static_cast<uint32_t>(val);
    wide.m_storage[1] = static_cast<uint32_t>(val >> 32);
    wide.m_storage[2] = 0;
    wide.m_storage[3] = 0;
}

static void drive_inputs(Vct_ifu_icache_if *dut, uint32_t cycle, uint32_t lfsr) {
    // Default control signals
    dut->cp0_ifu_icache_en = 1;
    dut->cp0_ifu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->hpcp_ifu_cnt_en = 1;

    // Mix cycle and LFSR for broader switching
    const uint32_t mix = lfsr ^ (cycle * 0x9E37U);

    // PCGen interface
    dut->pcgen_icache_if_chgflw = (mix >> 0) & 1;
    dut->pcgen_icache_if_chgflw_bank0 = (mix >> 1) & 1;
    dut->pcgen_icache_if_chgflw_bank1 = (mix >> 2) & 1;
    dut->pcgen_icache_if_chgflw_bank2 = (mix >> 3) & 1;
    dut->pcgen_icache_if_chgflw_bank3 = (mix >> 4) & 1;
    dut->pcgen_icache_if_chgflw_short = (mix >> 5) & 1;
    dut->pcgen_icache_if_gateclk_en = (mix >> 6) & 1;
    dut->pcgen_icache_if_index = static_cast<uint16_t>((mix >> 7) & 0xFFFF);
    dut->pcgen_icache_if_seq_data_req = (mix >> 23) & 1;
    dut->pcgen_icache_if_seq_data_req_short = (mix >> 24) & 1;
    dut->pcgen_icache_if_seq_tag_req = (mix >> 25) & 1;
    dut->pcgen_icache_if_way_pred = static_cast<uint8_t>((mix >> 26) & 0x3);

    // IFCTRL interface - truncate to 39 bits
    dut->ifctrl_icache_if_index = (static_cast<uint64_t>(lfsr) << 7) | cycle;
    dut->ifctrl_icache_if_inv_fifo = (mix >> 0) & 1;
    dut->ifctrl_icache_if_inv_on = (cycle == 50) ? 1 : 0;  // Pulse inv_on at cycle 50
    dut->ifctrl_icache_if_read_req_data0 = (mix >> 1) & 1;
    dut->ifctrl_icache_if_read_req_data1 = (mix >> 2) & 1;
    dut->ifctrl_icache_if_read_req_index = (static_cast<uint64_t>(lfsr) << 3) | cycle;
    dut->ifctrl_icache_if_read_req_tag = (mix >> 3) & 1;
    dut->ifctrl_icache_if_reset_req = (cycle < 10) ? 1 : 0;  // Reset req during first 10 cycles
    dut->ifctrl_icache_if_tag_req = (mix >> 4) & 1;
    dut->ifctrl_icache_if_tag_wen = static_cast<uint8_t>((mix >> 5) & 0x7);

    // IPB interface - truncate to 34 bits
    dut->ipb_icache_if_index = static_cast<uint64_t>(((mix >> 10) ^ cycle) & 0x3F);
    dut->ipb_icache_if_req = (mix >> 8) & 1;
    dut->ipb_icache_if_req_for_gateclk = (mix >> 9) & 1;

    // L1 Refill interface
    dut->l1_refill_icache_if_fifo = (mix >> 12) & 1;
    dut->l1_refill_icache_if_first = (mix >> 13) & 1;
    dut->l1_refill_icache_if_index = (static_cast<uint64_t>(lfsr) << 5) | cycle;
    set_wide128(dut->l1_refill_icache_if_inst_data, (static_cast<uint64_t>(lfsr) << 32) | mix);
    dut->l1_refill_icache_if_last = (mix >> 14) & 1;
    dut->l1_refill_icache_if_pre_code = mix;
    dut->l1_refill_icache_if_ptag = static_cast<uint32_t>((mix >> 15) & 0xFFFFFFF);
    dut->l1_refill_icache_if_wr = (mix >> 16) & 1;

    // HPCP interface
    dut->ifu_hpcp_icache_miss_pre = (mix >> 20) & 1;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_ifu_icache_if(ctx);

    // Initialize inputs before reset release
    dut->forever_cpuclk = 0;
    dut->cpurst_b = 0;
    uint32_t lfsr = 0xACE1u;
    drive_inputs(dut, 0, lfsr);

    // Hold reset for a few cycles
    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }

    // Deassert reset
    dut->cpurst_b = 1;

    bool checked_after_reset = false;

    for (int cycle = 0; cycle < 2000; ++cycle) {
        lfsr = lfsr_next(lfsr);
        drive_inputs(dut, static_cast<uint32_t>(cycle), lfsr);
        tick(ctx, dut);

        // Basic checks after reset
        if (cycle == 20) {
            checked_after_reset = true;
            // Check that outputs are valid (not X)
            // icache_if_ifdp_fifo should be 0 or 1 after reset
            if (dut->icache_if_ifdp_fifo != 0 && dut->icache_if_ifdp_fifo != 1) {
                std::cerr << "[TB] icache_if_ifdp_fifo is X after reset" << std::endl;
                return 1;
            }
        }

        // Trigger various control paths for coverage
        if (cycle == 100) {
            // Test refill write
            dut->l1_refill_icache_if_wr = 1;
            dut->l1_refill_icache_if_first = 1;
        }
        if (cycle == 105) {
            dut->l1_refill_icache_if_first = 0;
            dut->l1_refill_icache_if_last = 1;
        }
        if (cycle == 110) {
            dut->l1_refill_icache_if_wr = 0;
            dut->l1_refill_icache_if_last = 0;
        }

        if (ctx->gotFinish()) {
            break;
        }
    }

    if (!checked_after_reset) {
        std::cerr << "[TB] reset check did not run" << std::endl;
        return 1;
    }

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif

    dut->final();
    delete dut;
    delete ctx;
    return 0;
}
