#include "Vct_fcnvt_itof_sh.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

static int leading_one_index(uint64_t value) {
    if (value == 0) {
        return -1;
    }
    return 63 - __builtin_clzll(value);
}

static uint64_t mask_bits(uint64_t value, int bits) {
    if (bits >= 64) {
        return value;
    }
    if (bits <= 0) {
        return 0;
    }
    return value & ((1ULL << bits) - 1ULL);
}

static bool check_basic(Vct_fcnvt_itof_sh *dut, uint64_t src) {
    dut->ff1_sh_src = src;
    dut->eval();

    const int idx = leading_one_index(src);
    const uint8_t exp_cnt = (idx >= 0) ? static_cast<uint8_t>(idx) : 0;
    const uint8_t exp_cnt_p1 = (idx >= 0) ? static_cast<uint8_t>((idx + 1) & 0x3F) : 0;
    const uint64_t shifted = (idx >= 0) ? (src << (63 - idx)) : 0;
    const uint32_t exp_f_v = static_cast<uint32_t>((shifted >> 40) & 0xFFFFFFu);
    const uint64_t exp_f_x = mask_bits(shifted, 40);

    bool exp_c_in = false;
    if (idx >= 0) {
        const uint64_t mask = (idx == 63) ? ~0ULL : ((1ULL << (idx + 1)) - 1ULL);
        exp_c_in = (src & mask) == mask;
    }

    bool ok = true;
    if ((dut->ff1_sh_cnt & 0x3F) != exp_cnt) {
        std::cerr << "[TB] ff1_sh_cnt mismatch for 0x" << std::hex << src << std::dec
                  << ": got " << static_cast<int>(dut->ff1_sh_cnt)
                  << " expected " << static_cast<int>(exp_cnt) << "\n";
        ok = false;
    }
    if ((dut->ff1_sh_cnt_p1 & 0x3F) != exp_cnt_p1) {
        std::cerr << "[TB] ff1_sh_cnt_p1 mismatch for 0x" << std::hex << src << std::dec
                  << ": got " << static_cast<int>(dut->ff1_sh_cnt_p1)
                  << " expected " << static_cast<int>(exp_cnt_p1) << "\n";
        ok = false;
    }
    if ((dut->ff1_sh_f_v & 0xFFFFFFu) != exp_f_v) {
        std::cerr << "[TB] ff1_sh_f_v mismatch for 0x" << std::hex << src << std::dec
                  << ": got 0x" << std::hex << dut->ff1_sh_f_v
                  << " expected 0x" << exp_f_v << std::dec << "\n";
        ok = false;
    }
    if (mask_bits(dut->ff1_sh_f_x, 40) != exp_f_x) {
        std::cerr << "[TB] ff1_sh_f_x mismatch for 0x" << std::hex << src << std::dec
                  << ": got 0x" << std::hex << dut->ff1_sh_f_x
                  << " expected 0x" << exp_f_x << std::dec << "\n";
        ok = false;
    }
    if (static_cast<bool>(dut->ff1_sh_c_in) != exp_c_in) {
        std::cerr << "[TB] ff1_sh_c_in mismatch for 0x" << std::hex << src << std::dec
                  << ": got " << static_cast<int>(dut->ff1_sh_c_in)
                  << " expected " << static_cast<int>(exp_c_in) << "\n";
        ok = false;
    }

    if (src == 0 || src == 1) {
        const uint64_t exp_add = (src == 1) ? (1ULL << 52) : 0;
        const uint8_t exp_zero = 1;
        const uint8_t exp_eq = 0;
        const uint8_t exp_gt = 0;
        if (mask_bits(dut->ff1_sh_d_add_1, 53) != exp_add) {
            std::cerr << "[TB] ff1_sh_d_add_1 mismatch for 0x" << std::hex << src << std::dec << "\n";
            ok = false;
        }
        if (mask_bits(dut->ff1_sh_s_add_1, 53) != exp_add) {
            std::cerr << "[TB] ff1_sh_s_add_1 mismatch for 0x" << std::hex << src << std::dec << "\n";
            ok = false;
        }
        if (mask_bits(dut->ff1_sh_h_add_1, 53) != exp_add) {
            std::cerr << "[TB] ff1_sh_h_add_1 mismatch for 0x" << std::hex << src << std::dec << "\n";
            ok = false;
        }
        if (dut->ff1_sh_d_c_zero != exp_zero || dut->ff1_sh_s_c_zero != exp_zero ||
            dut->ff1_sh_h_c_zero != exp_zero) {
            std::cerr << "[TB] c_zero mismatch for 0x" << std::hex << src << std::dec << "\n";
            ok = false;
        }
        if (dut->ff1_sh_d_c_eq_0p5 != exp_eq || dut->ff1_sh_s_c_eq_0p5 != exp_eq ||
            dut->ff1_sh_h_c_eq_0p5 != exp_eq) {
            std::cerr << "[TB] c_eq_0p5 mismatch for 0x" << std::hex << src << std::dec << "\n";
            ok = false;
        }
        if (dut->ff1_sh_d_c_gt_0p5 != exp_gt || dut->ff1_sh_s_c_gt_0p5 != exp_gt ||
            dut->ff1_sh_h_c_gt_0p5 != exp_gt) {
            std::cerr << "[TB] c_gt_0p5 mismatch for 0x" << std::hex << src << std::dec << "\n";
            ok = false;
        }
    }

    return ok;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_fcnvt_itof_sh(ctx);

    std::vector<uint64_t> vectors;
    vectors.push_back(0);
    vectors.push_back(1);
    vectors.push_back(~0ULL);
    vectors.push_back((1ULL << 10) | ((1ULL << 10) - 1ULL));

    for (int p = 63; p >= 0; --p) {
        const uint64_t lower_mask = (p == 0) ? 0ULL : ((1ULL << p) - 1ULL);
        const uint64_t lower_bits = (0x9E3779B97F4A7C15ULL * static_cast<uint64_t>(p + 1)) & lower_mask;
        const uint64_t value = (1ULL << p) | lower_bits;
        vectors.push_back(value);
    }

    bool ok = true;
    for (uint64_t value : vectors) {
        ctx->timeInc(1);
        if (!check_basic(dut, value)) {
            ok = false;
        }
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

    if (!ok) {
        return 1;
    }
    return 0;
}
