#include "Vct_ifu_bht.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vct_ifu_bht *dut) {
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

static void drive_inputs(Vct_ifu_bht *dut, uint32_t cycle, uint32_t lfsr) {
    // Defaults that keep the BHT enabled and clocks active.
    dut->cp0_ifu_bht_en = 1;
    dut->cp0_ifu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;

    // Keep invalidation off until explicitly pulsed.
    dut->ifctrl_bht_inv = 0;

    // Mix cycle and LFSR for broader switching.
    const uint32_t mix = lfsr ^ (cycle * 0x9E37U);

    dut->ifctrl_bht_pipedown = (mix >> 0) & 1;
    dut->ifctrl_bht_stall = (mix >> 1) & 1;
    dut->ipctrl_bht_con_br_gateclk_en = (mix >> 2) & 1;
    dut->ipctrl_bht_con_br_taken = (mix >> 3) & 1;
    dut->ipctrl_bht_con_br_vld = (mix >> 4) & 1;
    dut->ipctrl_bht_more_br = (mix >> 5) & 1;
    dut->ipctrl_bht_vld = (mix >> 6) & 1;
    dut->ipdp_bht_h0_con_br = (mix >> 7) & 1;

    const uint64_t vpc_seed = 0x1F1234567ULL ^ (static_cast<uint64_t>(lfsr) << 7) ^ (static_cast<uint64_t>(cycle) << 1);
    dut->ipdp_bht_vpc = static_cast<uint64_t>(vpc_seed & ((1ULL << 39) - 1));

    dut->iu_ifu_bht_check_vld = (mix >> 8) & 1;
    dut->iu_ifu_bht_condbr_taken = (mix >> 9) & 1;
    dut->iu_ifu_bht_pred = (mix >> 10) & 1;
    dut->iu_ifu_chgflw_vld = (mix >> 11) & 1;

    const uint64_t idx_seed = 0x15555U ^ (lfsr * 0x1234U) ^ (cycle << 3);
    dut->iu_ifu_chk_idx = static_cast<uint32_t>(idx_seed & ((1U << 25) - 1));

    const uint64_t pc_seed = 0x3A5A5A5A5ULL ^ (static_cast<uint64_t>(lfsr) << 5) ^ (static_cast<uint64_t>(cycle) << 2);
    dut->iu_ifu_cur_pc = static_cast<uint64_t>(pc_seed & ((1ULL << 39) - 1));

    dut->lbuf_bht_active_state = (mix >> 12) & 1;
    dut->lbuf_bht_con_br_taken = (mix >> 13) & 1;
    dut->lbuf_bht_con_br_vld = (mix >> 14) & 1;

    dut->pcgen_bht_chgflw = (mix >> 15) & 1;
    dut->pcgen_bht_chgflw_short = (mix >> 16) & 1;
    dut->pcgen_bht_ifpc = static_cast<uint32_t>(((mix >> 17) ^ (cycle * 7)) & 0x7F);
    dut->pcgen_bht_pcindex = static_cast<uint32_t>(((mix >> 24) ^ (cycle * 13)) & 0x3FF);
    dut->pcgen_bht_seq_read = (mix >> 18) & 1;

    dut->rtu_ifu_flush = (mix >> 19) & 1;
    dut->rtu_ifu_retire0_condbr = (mix >> 20) & 1;
    dut->rtu_ifu_retire0_condbr_taken = (mix >> 21) & 1;
    dut->rtu_ifu_retire1_condbr = (mix >> 22) & 1;
    dut->rtu_ifu_retire1_condbr_taken = (mix >> 23) & 1;
    dut->rtu_ifu_retire2_condbr = (mix >> 24) & 1;
    dut->rtu_ifu_retire2_condbr_taken = (mix >> 25) & 1;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_ifu_bht(ctx);

    // Initialize inputs before reset release.
    dut->forever_cpuclk = 0;
    dut->cpurst_b = 0;
    uint32_t lfsr = 0xACE1u;
    drive_inputs(dut, 0, lfsr);

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }

    // Deassert reset.
    dut->cpurst_b = 1;

    bool checked_after_reset = false;
    for (int cycle = 0; cycle < 2000; ++cycle) {
        lfsr = lfsr_next(lfsr);
        drive_inputs(dut, static_cast<uint32_t>(cycle), lfsr);

        if (cycle == 100) {
            dut->ifctrl_bht_inv = 1;
        }

        tick(ctx, dut);

        if (cycle == 10) {
            checked_after_reset = true;
            if (dut->bht_ifctrl_inv_on != 0) {
                std::cerr << "[TB] bht_ifctrl_inv_on expected 0 after reset" << std::endl;
                return 1;
            }
            if (dut->bht_ifctrl_inv_done != 1) {
                std::cerr << "[TB] bht_ifctrl_inv_done expected 1 after reset" << std::endl;
                return 1;
            }
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
