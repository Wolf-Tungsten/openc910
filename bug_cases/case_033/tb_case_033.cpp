#include "Vct_iu_bju_pcfifo_entry.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

struct ModelState {
    bool vld = false;
    bool cmplt = false;
    bool flush = false;
    uint32_t chk_idx = 0;
    bool bht_pred = false;
    bool jmp_mispred = false;
    uint64_t pc = 0;
    bool bju = false;
    bool condbr = false;
    bool pcall = false;
    bool pret = false;
    bool jmp = false;
    bool bht_mispred = false;
    bool length = false;
};

static void tick(VerilatedContext *ctx, Vct_iu_bju_pcfifo_entry *dut) {
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static unsigned __int128 make_create_data(uint32_t chk_idx,
                                          bool jmp_mispred,
                                          bool bht_pred,
                                          uint64_t pc) {
    unsigned __int128 value = 0;
    value |= (static_cast<unsigned __int128>(chk_idx & 0x1ffffffU) << 42);
    value |= (static_cast<unsigned __int128>(jmp_mispred ? 1 : 0) << 41);
    value |= (static_cast<unsigned __int128>(bht_pred ? 1 : 0) << 40);
    value |= static_cast<unsigned __int128>(pc & ((1ULL << 40) - 1));
    return value;
}

static void set_wide67(WData *dst, unsigned __int128 value) {
    dst[0] = static_cast<uint32_t>(value & 0xffffffffu);
    dst[1] = static_cast<uint32_t>((value >> 32) & 0xffffffffu);
    dst[2] = static_cast<uint32_t>((value >> 64) & 0x7u);
}

static unsigned __int128 get_wide67(const WData *src) {
    unsigned __int128 value = 0;
    value |= static_cast<unsigned __int128>(src[0]);
    value |= static_cast<unsigned __int128>(src[1]) << 32;
    value |= static_cast<unsigned __int128>(src[2]) << 64;
    return value;
}

static void step_model(ModelState &state,
                       bool cpurst_b,
                       bool rtu_yy_xx_flush,
                       bool iu_yy_xx_cancel,
                       bool rtu_iu_flush_fe,
                       bool x_cmplt_en,
                       uint8_t x_create_en,
                       bool x_pop_en,
                       unsigned __int128 create0,
                       unsigned __int128 create1,
                       unsigned __int128 create2,
                       bool bju_pcfifo_ex2_bht_mispred,
                       bool bju_pcfifo_ex2_condbr,
                       bool bju_pcfifo_ex2_jmp,
                       bool bju_pcfifo_ex2_length,
                       uint64_t bju_pcfifo_ex2_pc,
                       bool bju_pcfifo_ex2_pcall,
                       bool bju_pcfifo_ex2_pret) {
    const bool old_vld = state.vld;
    const bool old_cmplt = state.cmplt;
    const bool old_flush = state.flush;
    const bool create_en = (x_create_en != 0);

    unsigned __int128 create_data = 0;
    switch (x_create_en) {
        case 0x1:
            create_data = create0;
            break;
        case 0x2:
            create_data = create1;
            break;
        case 0x4:
            create_data = create2;
            break;
        default:
            create_data = 0;
            break;
    }

    const uint32_t create_chk_idx = static_cast<uint32_t>((create_data >> 42) & 0x1ffffffU);
    const bool create_jmp_mispred = ((create_data >> 41) & 1) != 0;
    const bool create_bht_pred = ((create_data >> 40) & 1) != 0;
    const uint64_t create_pc = static_cast<uint64_t>(create_data & ((1ULL << 40) - 1));

    ModelState next = state;

    if (!cpurst_b) {
        next.vld = false;
    } else if (old_vld && rtu_yy_xx_flush && (old_flush || iu_yy_xx_cancel || rtu_iu_flush_fe)) {
        next.vld = false;
    } else if (create_en) {
        next.vld = true;
    } else if (x_pop_en) {
        next.vld = false;
    }

    if (!cpurst_b) {
        next.cmplt = false;
    } else if (old_vld && rtu_yy_xx_flush && (old_flush || iu_yy_xx_cancel || rtu_iu_flush_fe)) {
        next.cmplt = false;
    } else if (create_en) {
        next.cmplt = false;
    } else if (x_cmplt_en) {
        next.cmplt = true;
    } else if (x_pop_en) {
        next.cmplt = false;
    }

    if (!cpurst_b) {
        next.flush = false;
    } else if (old_vld && (iu_yy_xx_cancel || rtu_iu_flush_fe) && !rtu_yy_xx_flush) {
        next.flush = true;
    } else if (create_en) {
        next.flush = false;
    }

    if (!cpurst_b) {
        next.chk_idx = 0;
        next.bht_pred = false;
        next.jmp_mispred = false;
    } else if (create_en) {
        next.chk_idx = create_chk_idx;
        next.bht_pred = create_bht_pred;
        next.jmp_mispred = create_jmp_mispred;
    }

    if (!cpurst_b) {
        next.pc = 0;
        next.bju = false;
        next.condbr = false;
        next.pcall = false;
        next.pret = false;
        next.jmp = false;
        next.bht_mispred = false;
        next.length = false;
    } else if (create_en) {
        next.pc = create_pc;
        next.bju = false;
        next.condbr = false;
        next.pcall = false;
        next.pret = false;
        next.jmp = false;
        next.bht_mispred = false;
        next.length = false;
    } else if (x_cmplt_en) {
        next.pc = bju_pcfifo_ex2_pc & ((1ULL << 40) - 1);
        next.bju = true;
        next.condbr = bju_pcfifo_ex2_condbr;
        next.pcall = bju_pcfifo_ex2_pcall;
        next.pret = bju_pcfifo_ex2_pret;
        next.jmp = bju_pcfifo_ex2_jmp;
        next.bht_mispred = bju_pcfifo_ex2_bht_mispred;
        next.length = bju_pcfifo_ex2_length;
    }

    state = next;
}

static bool check_outputs(const ModelState &state,
                          const Vct_iu_bju_pcfifo_entry *dut,
                          int cycle) {
    const unsigned __int128 mask67 = (static_cast<unsigned __int128>(1) << 67) - 1;
    const unsigned __int128 expected_rf =
        (static_cast<unsigned __int128>(state.chk_idx & 0x1ffffffU) << 42) |
        (static_cast<unsigned __int128>(state.jmp_mispred ? 1 : 0) << 41) |
        (static_cast<unsigned __int128>(state.bht_pred ? 1 : 0) << 40) |
        static_cast<unsigned __int128>(state.pc & ((1ULL << 40) - 1));

    const uint64_t expected_rt =
        (static_cast<uint64_t>(state.cmplt ? 1 : 0) << 50) |
        (static_cast<uint64_t>(state.flush ? 1 : 0) << 49) |
        (static_cast<uint64_t>(state.vld ? 1 : 0) << 48) |
        (static_cast<uint64_t>(state.length ? 1 : 0) << 47) |
        (static_cast<uint64_t>(state.bht_pred ? 1 : 0) << 46) |
        (static_cast<uint64_t>(state.bju ? 1 : 0) << 45) |
        (static_cast<uint64_t>(state.bht_mispred ? 1 : 0) << 44) |
        (static_cast<uint64_t>(state.jmp ? 1 : 0) << 43) |
        (static_cast<uint64_t>(state.pret ? 1 : 0) << 42) |
        (static_cast<uint64_t>(state.pcall ? 1 : 0) << 41) |
        (static_cast<uint64_t>(state.condbr ? 1 : 0) << 40) |
        (state.pc & ((1ULL << 40) - 1));

    const unsigned __int128 actual_rf = get_wide67(dut->x_rf_read_data) & mask67;
    const uint64_t actual_rt =
        static_cast<uint64_t>(dut->x_rt_read_data) & ((1ULL << 51) - 1);

    if ((dut->x_vld != (state.vld ? 1 : 0)) || (dut->x_flush != (state.flush ? 1 : 0)) ||
        actual_rf != expected_rf || actual_rt != expected_rt) {
        const uint64_t actual_rf_lo = static_cast<uint64_t>(actual_rf & 0xffffffffffffffffULL);
        const uint64_t actual_rf_hi = static_cast<uint64_t>(actual_rf >> 64);
        const uint64_t expected_rf_lo = static_cast<uint64_t>(expected_rf & 0xffffffffffffffffULL);
        const uint64_t expected_rf_hi = static_cast<uint64_t>(expected_rf >> 64);

        std::cerr << "[TB] mismatch at cycle " << cycle << "\n"
                  << "  x_vld expected=" << state.vld << " actual=" << static_cast<int>(dut->x_vld)
                  << "\n  x_flush expected=" << state.flush
                  << " actual=" << static_cast<int>(dut->x_flush) << "\n"
                  << "  x_rf_read_data expected=0x" << std::hex << expected_rf_hi << "_"
                  << expected_rf_lo << " actual=0x" << actual_rf_hi << "_" << actual_rf_lo
                  << "\n  x_rt_read_data expected=0x" << std::hex << expected_rt << " actual=0x"
                  << actual_rt << std::dec << "\n";
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_iu_bju_pcfifo_entry(ctx);

    dut->cp0_iu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->iu_yy_xx_cancel = 0;
    dut->rtu_iu_flush_fe = 0;
    dut->rtu_yy_xx_flush = 0;
    dut->x_cmplt_en = 0;
    dut->x_create_en = 0;
    dut->x_pop_en = 0;
    dut->x_pop_gateclk_en = 0;
    dut->bju_pcfifo_ex2_bht_mispred = 0;
    dut->bju_pcfifo_ex2_condbr = 0;
    dut->bju_pcfifo_ex2_jmp = 0;
    dut->bju_pcfifo_ex2_length = 0;
    dut->bju_pcfifo_ex2_pc = 0;
    dut->bju_pcfifo_ex2_pcall = 0;
    dut->bju_pcfifo_ex2_pret = 0;

    const unsigned __int128 create0 = make_create_data(0x1a5aaU, true, false, 0x123456789ULL);
    const unsigned __int128 create1 = make_create_data(0x0f00dU, false, true, 0x00abcdd11ULL);
    const unsigned __int128 create2 = make_create_data(0x15555U, true, true, 0x0fedcba98ULL);

    set_wide67(dut->pcfifo_create0_data, create0);
    set_wide67(dut->pcfifo_create1_data, create1);
    set_wide67(dut->pcfifo_create2_data, create2);

    ModelState model;

    dut->cpurst_b = 0;
    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
        step_model(model, false, dut->rtu_yy_xx_flush, dut->iu_yy_xx_cancel, dut->rtu_iu_flush_fe,
                   dut->x_cmplt_en, static_cast<uint8_t>(dut->x_create_en), dut->x_pop_en,
                   create0, create1, create2, dut->bju_pcfifo_ex2_bht_mispred,
                   dut->bju_pcfifo_ex2_condbr, dut->bju_pcfifo_ex2_jmp,
                   dut->bju_pcfifo_ex2_length, dut->bju_pcfifo_ex2_pc,
                   dut->bju_pcfifo_ex2_pcall, dut->bju_pcfifo_ex2_pret);
        if (!check_outputs(model, dut, i)) {
            return 1;
        }
    }

    dut->cpurst_b = 1;

    auto run_cycle = [&](int cycle) {
        tick(ctx, dut);
        step_model(model, dut->cpurst_b, dut->rtu_yy_xx_flush, dut->iu_yy_xx_cancel,
                   dut->rtu_iu_flush_fe, dut->x_cmplt_en, static_cast<uint8_t>(dut->x_create_en),
                   dut->x_pop_en, create0, create1, create2, dut->bju_pcfifo_ex2_bht_mispred,
                   dut->bju_pcfifo_ex2_condbr, dut->bju_pcfifo_ex2_jmp,
                   dut->bju_pcfifo_ex2_length, dut->bju_pcfifo_ex2_pc,
                   dut->bju_pcfifo_ex2_pcall, dut->bju_pcfifo_ex2_pret);
        return check_outputs(model, dut, cycle);
    };

    int cycle = 2;

    dut->x_create_en = 0;
    dut->x_cmplt_en = 0;
    dut->x_pop_en = 0;
    dut->x_pop_gateclk_en = 0;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_create_en = 0x1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_create_en = 0;
    dut->x_cmplt_en = 1;
    dut->bju_pcfifo_ex2_bht_mispred = 1;
    dut->bju_pcfifo_ex2_condbr = 1;
    dut->bju_pcfifo_ex2_jmp = 1;
    dut->bju_pcfifo_ex2_length = 0;
    dut->bju_pcfifo_ex2_pc = 0x111111111ULL;
    dut->bju_pcfifo_ex2_pcall = 1;
    dut->bju_pcfifo_ex2_pret = 0;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_cmplt_en = 0;
    dut->x_pop_en = 1;
    dut->x_pop_gateclk_en = 1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_pop_en = 0;
    dut->x_pop_gateclk_en = 0;
    dut->x_create_en = 0x2;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_create_en = 0;
    dut->iu_yy_xx_cancel = 1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->iu_yy_xx_cancel = 0;
    dut->rtu_yy_xx_flush = 1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->rtu_yy_xx_flush = 0;
    dut->x_create_en = 0x4;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_create_en = 0;
    dut->x_cmplt_en = 1;
    dut->rtu_iu_flush_fe = 1;
    dut->bju_pcfifo_ex2_bht_mispred = 0;
    dut->bju_pcfifo_ex2_condbr = 0;
    dut->bju_pcfifo_ex2_jmp = 0;
    dut->bju_pcfifo_ex2_length = 1;
    dut->bju_pcfifo_ex2_pc = 0x222222222ULL;
    dut->bju_pcfifo_ex2_pcall = 0;
    dut->bju_pcfifo_ex2_pret = 1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_cmplt_en = 0;
    dut->rtu_iu_flush_fe = 0;
    dut->rtu_yy_xx_flush = 1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->rtu_yy_xx_flush = 0;
    dut->x_create_en = 0x1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    dut->x_create_en = 0;
    dut->x_cmplt_en = 1;
    dut->bju_pcfifo_ex2_bht_mispred = 1;
    dut->bju_pcfifo_ex2_condbr = 1;
    dut->bju_pcfifo_ex2_jmp = 0;
    dut->bju_pcfifo_ex2_length = 1;
    dut->bju_pcfifo_ex2_pc = 0x333333333ULL;
    dut->bju_pcfifo_ex2_pcall = 1;
    dut->bju_pcfifo_ex2_pret = 1;
    if (!run_cycle(cycle++)) {
        return 1;
    }

    for (int i = 0; i < 3; ++i) {
        dut->x_cmplt_en = 0;
        dut->x_create_en = 0;
        dut->x_pop_en = 0;
        if (!run_cycle(cycle++)) {
            return 1;
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
