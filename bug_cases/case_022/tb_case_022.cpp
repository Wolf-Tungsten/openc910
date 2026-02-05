#include "Vct_rtu_rob_entry.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vct_rtu_rob_entry *dut) {
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static uint32_t next_rand(uint32_t &state) {
    state = state * 1664525u + 1013904223u;
    return state;
}

static uint64_t next_data40(uint32_t &state) {
    uint64_t lo = next_rand(state);
    uint64_t hi = next_rand(state);
    return ((hi << 32) | lo) & ((1ULL << 40) - 1ULL);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_rtu_rob_entry(ctx);

    dut->cp0_rtu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->lsu_misc_cmplt_gateclk_en = 1;
    dut->x_create_gateclk_en = 1;
    dut->x_cmplt_gateclk_vld = 1;

    dut->cpurst_b = 0;
    dut->retire_rob_flush = 0;
    dut->retire_rob_flush_gateclk = 0;
    dut->x_create_en = 0;
    dut->x_create_dp_en = 0;
    dut->x_pop_en = 0;
    dut->x_create_sel = 0;
    dut->x_cmplt_vld = 0;

    uint32_t rng = 0x12345678u;
    uint8_t expected_vld = 0;

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->cpurst_b = 1;

    for (int cycle = 0; cycle < 200; ++cycle) {
        uint32_t r = next_rand(rng);
        dut->idu_rtu_rob_create0_data = next_data40(rng);
        dut->idu_rtu_rob_create1_data = next_data40(rng);
        dut->idu_rtu_rob_create2_data = next_data40(rng);
        dut->idu_rtu_rob_create3_data = next_data40(rng);

        dut->lsu_rtu_wb_pipe3_bkpta_data = (r >> 0) & 1U;
        dut->lsu_rtu_wb_pipe3_bkptb_data = (r >> 1) & 1U;
        dut->lsu_rtu_wb_pipe3_no_spec_hit = (r >> 2) & 1U;
        dut->lsu_rtu_wb_pipe3_no_spec_mispred = (r >> 3) & 1U;
        dut->lsu_rtu_wb_pipe3_no_spec_miss = (r >> 4) & 1U;
        dut->lsu_rtu_wb_pipe4_bkpta_data = (r >> 5) & 1U;
        dut->lsu_rtu_wb_pipe4_bkptb_data = (r >> 6) & 1U;
        dut->lsu_rtu_wb_pipe4_no_spec_hit = (r >> 7) & 1U;
        dut->lsu_rtu_wb_pipe4_no_spec_mispred = (r >> 8) & 1U;
        dut->lsu_rtu_wb_pipe4_no_spec_miss = (r >> 9) & 1U;

        dut->x_cmplt_vld = static_cast<uint8_t>(r & 0x7FU);
        dut->x_pop_en = (r >> 10) & 1U;
        dut->x_create_dp_en = (r >> 11) & 1U;
        dut->retire_rob_flush = (r >> 12) & 1U;
        dut->retire_rob_flush_gateclk = dut->retire_rob_flush;

        const bool do_create = (cycle % 5) != 0;
        dut->x_create_en = do_create ? 1 : 0;
        if (do_create) {
            switch (cycle % 4) {
            case 0:
                dut->x_create_sel = 0x1;
                break;
            case 1:
                dut->x_create_sel = 0x2;
                break;
            case 2:
                dut->x_create_sel = 0x4;
                break;
            default:
                dut->x_create_sel = 0x8;
                break;
            }
        } else {
            dut->x_create_sel = (cycle & 1) ? 0x0 : 0x3; // invalid to hit default branch
        }

        uint8_t create_bit0 = expected_vld;
        if (do_create) {
            if (dut->x_create_sel == 0x1) {
                create_bit0 = dut->idu_rtu_rob_create0_data & 0x1U;
            } else if (dut->x_create_sel == 0x2) {
                create_bit0 = dut->idu_rtu_rob_create1_data & 0x1U;
            } else if (dut->x_create_sel == 0x4) {
                create_bit0 = dut->idu_rtu_rob_create2_data & 0x1U;
            } else if (dut->x_create_sel == 0x8) {
                create_bit0 = dut->idu_rtu_rob_create3_data & 0x1U;
            }
        }

        uint8_t expected_next = expected_vld;
        if (!dut->cpurst_b) {
            expected_next = 0;
        } else if (dut->retire_rob_flush) {
            expected_next = 0;
        } else if (dut->x_create_en) {
            expected_next = create_bit0;
        } else if (dut->x_pop_en) {
            expected_next = 0;
        }

        tick(ctx, dut);

        if (dut->cpurst_b) {
            const uint8_t got_vld = static_cast<uint8_t>(dut->x_read_data & 0x1U);
            if (got_vld != expected_next) {
                std::cerr << "[TB] vld mismatch at cycle " << cycle
                          << " expected=" << static_cast<int>(expected_next)
                          << " got=" << static_cast<int>(got_vld) << std::endl;
                return 1;
            }
        }
        expected_vld = expected_next;

        if (ctx->gotFinish()) {
            break;
        }
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
