#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_half_dp.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {

void tick(Vct_fadd_half_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t pack_half(uint16_t bits, bool high_ones = true) {
    uint64_t upper = high_ones ? 0xFFFFFFFFFFFF0000ULL : 0x0ULL;
    return upper | static_cast<uint64_t>(bits);
}

std::string hex4(uint16_t v) {
    std::ostringstream oss;
    oss << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned>(v);
    return oss.str();
}

struct Scenario {
    const char* name;
    bool op_add;
    bool op_sub;
    bool op_cmp;
    bool op_maxnm;
    bool op_minnm;
    bool op_feq;
    bool op_fle;
    bool op_flt;
    bool op_fne;
    bool op_ford;
    bool rm_rne;
    bool rm_rtz;
    bool rm_rdn;
    bool rm_rup;
    bool rm_rmm;
    uint16_t src0;
    uint16_t src1;
    bool high_ones_src0;
    bool high_ones_src1;
    bool dqnan;
    bool check_result;
    uint16_t expected_result;
};

bool drive(Vct_fadd_half_dp& dut,
           const Scenario& sc,
           const std::function<bool(bool, const std::string&)>& expect,
           bool scalar = true) {
    dut.vfpu_yy_xx_dqnan = sc.dqnan;

    dut.ex1_op_add = sc.op_add;
    dut.ex1_op_sub = sc.op_sub;
    dut.ex1_op_cmp = sc.op_cmp;
    dut.ex1_op_maxnm = sc.op_maxnm;
    dut.ex1_op_minnm = sc.op_minnm;
    dut.ex1_op_feq = sc.op_feq;
    dut.ex1_op_fle = sc.op_fle;
    dut.ex1_op_flt = sc.op_flt;
    dut.ex1_op_fne = sc.op_fne;
    dut.ex1_op_ford = sc.op_ford;
    dut.ex1_scalar = scalar;
    dut.fadd_ctrl_src0 = pack_half(sc.src0, sc.high_ones_src0);
    dut.fadd_ctrl_src1 = pack_half(sc.src1, sc.high_ones_src1);
    dut.ex1_pipedown = 1;
    tick(dut);
    dut.ex1_pipedown = 0;

    dut.ex2_op_add = sc.op_add;
    dut.ex2_op_sub = sc.op_sub;
    dut.ex2_op_cmp = sc.op_cmp;
    dut.ex2_op_maxnm = sc.op_maxnm;
    dut.ex2_op_minnm = sc.op_minnm;
    dut.ex2_op_fle = sc.op_fle;
    dut.ex2_op_flt = sc.op_flt;
    dut.ex2_rm_rne = sc.rm_rne;
    dut.ex2_rm_rtz = sc.rm_rtz;
    dut.ex2_rm_rdn = sc.rm_rdn;
    dut.ex2_rm_rup = sc.rm_rup;
    dut.ex2_rm_rmm = sc.rm_rmm;
    dut.ex2_pipedown = 1;
    tick(dut);
    dut.ex2_pipedown = 0;

    tick(dut);

    if (!expect(dut.ex3_expt <= 0x1F, std::string(sc.name) + ": ex3_expt out of range")) {
        return false;
    }
    if (sc.check_result) {
        uint16_t got = static_cast<uint16_t>(dut.ex3_result);
        if (!expect(got == sc.expected_result,
                    std::string(sc.name) + ": expected 0x" + hex4(sc.expected_result) +
                        " got 0x" + hex4(got))) {
            return false;
        }
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_half_dp dut;

    auto expect = [&](bool cond, const std::string& msg) -> bool {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    // Reset and baseline configuration.
    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.vfpu_yy_xx_dqnan = 0;
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex1_scalar = 0;
    dut.forever_cpuclk = 0;
    dut.eval();
    tick(dut);
    dut.cpurst_b = 1;
    dut.eval();
    tick(dut);
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    std::vector<Scenario> scenarios{
        {"add_one_plus_one",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3C00, 0x3C00, true, true,
         false, true, 0x4000},
        {"sub_self_zero",
         false, true, false, false, false,
         false, false, false, false, false,
         false, true, false, false, false,
         0x3C00, 0x3C00, true, true,
         false, true, 0x0000},
        {"maxnm_sign_mix",
         false, false, false, true, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3C00, 0xBC00, true, true,
         false, true, 0x3C00},
        {"cmp_less_than",
         false, false, true, false, false,
         false, false, true, false, true,
         true, false, false, false, false,
         0x0001, 0x3C00, true, true,
         true, false, 0},
        {"feq_zero",
         false, false, true, false, false,
         true, false, false, false, false,
         true, false, false, false, false,
         0x0000, 0x8000, true, true,
         false, false, 0},
        {"fle_nan",
         false, false, true, false, false,
         false, true, false, false, true,
         false, true, false, false, false,
         0x7E00, 0x3C00, true, true,
         true, false, 0},
        {"fne_snan_dq",
         false, false, true, false, false,
         false, false, false, true, true,
         false, false, true, false, false,
         0x7DFF, 0x3555, true, true,
         true, false, 0},
        {"minnm_denorm_vs_inf",
         false, false, false, false, true,
         false, false, false, false, false,
         false, false, true, false, false,
         0x0001, 0x7C00, true, true,
         false, false, 0},
        {"round_rup_close",
         true, false, false, false, false,
         false, false, false, false, false,
         false, false, false, true, false,
         0x3555, 0x3555, true, true,
         false, false, 0},
        {"round_rdn_far",
         true, false, false, false, false,
         false, false, false, false, false,
         false, false, true, false, false,
         0x3BFF, 0x0400, true, true,
         false, false, 0},
        {"round_rmm_sub",
         false, true, false, false, false,
         false, false, false, false, false,
         false, false, false, false, true,
         0x3555, 0x0001, true, true,
         false, false, 0},
        {"canonical_nan_path",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x7E00, 0x3C00, false, true,
         false, false, 0},
        {"inf_plus_normal",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x7C00, 0x3C00, true, true,
         false, false, 0},
        {"lfn_overflow_rtz",
         true, false, false, false, false,
         false, false, false, false, false,
         false, true, false, false, false,
         0x7BFF, 0x0800, true, true,
         false, false, 0},
        {"lfn_operand_src1",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3C00, 0x7BFF, true, true,
         false, false, 0},
        {"snan_src1_maxnm",
         false, false, false, true, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3C00, 0x7DFF, true, true,
         false, false, 0},
        {"qnan_sign_src1",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3555, 0xFE00, true, true,
         true, false, 0},
        {"sub_to_denorm",
         false, true, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3C00, 0x3BFF, true, true,
         false, false, 0},
        {"minnm_inf_vs_lfn",
         false, false, false, false, true,
         false, false, false, false, false,
         true, false, false, false, false,
         0x7C00, 0x7BFF, true, true,
         false, false, 0},
        {"far_sub_rtz",
         false, true, false, false, false,
         false, false, false, false, false,
         false, true, false, false, false,
         0x4000, 0x0400, true, true,
         false, false, 0},
        {"src1_inf_add",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3555, 0x7C00, true, true,
         false, false, 0},
        {"src1_snan_add",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x3555, 0x7D01, true, true,
         false, false, 0},
        {"small_normal_flags",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x0401, 0x0002, true, true,
         false, false, 0},
        {"underflow_dn_sub",
         false, true, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x0400, 0x03FF, true, true,
         false, false, 0},
        {"far_add_rtz",
         true, false, false, false, false,
         false, false, false, false, false,
         false, true, false, false, false,
         0x7BFF, 0x0001, true, true,
         false, false, 0},
        {"far_sub_inc_modes",
         false, true, false, false, false,
         false, false, false, false, false,
         false, false, true, false, false,
         0x7BFF, 0x0001, true, true,
         false, false, 0},
        {"far_add_overflow_inf",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x7BFF, 0x7BFF, true, true,
         false, false, 0},
        {"snan_cmp_unord",
         false, false, true, false, false,
         false, true, false, false, false,
         true, false, false, false, false,
         0x3C00, 0x7DFF, true, true,
         false, false, 0},
        {"close_shift_deep",
         true, false, false, false, false,
         false, false, false, false, false,
         true, false, false, false, false,
         0x0400, 0x0001, true, true,
         false, false, 0},
    };

    for (const auto& sc : scenarios) {
        if (!drive(dut, sc, expect)) {
            return 1;
        }
    }

    Scenario vector_denorm{
        "vector_denorm_add",
        true, false, false, false, false,
        false, false, false, false, false,
        true, false, false, false, false,
        0x0001, 0x0002, false, false,
        false, false, 0};
    if (!drive(dut, vector_denorm, expect, false)) {
        return 1;
    }

    std::vector<uint16_t> denorms{0x0001, 0x0002, 0x0004, 0x0008, 0x0010};
    std::array<std::array<bool, 5>, 5> rm_sweep{{
        {true, false, false, false, false},
        {false, true, false, false, false},
        {false, false, true, false, false},
        {false, false, false, true, false},
        {false, false, false, false, true},
    }};
    int idx = 0;
    for (uint16_t a : denorms) {
        for (uint16_t b : denorms) {
            for (const auto& rm : rm_sweep) {
                bool sub = (idx & 1) != 0;
                Scenario sc{
                    "denorm_sweep",
                    !sub, sub, false, false, false,
                    false, false, false, false, false,
                    rm[0], rm[1], rm[2], rm[3], rm[4],
                    static_cast<uint16_t>(a | (sub ? 0x8000 : 0x0000)),
                    static_cast<uint16_t>(b),
                    true, true,
                    false, false, 0};
                if (!drive(dut, sc, expect)) {
                    return 1;
                }
                ++idx;
            }
        }
    }

    uint32_t lcg = 1;
    auto next_rand = [&]() {
        lcg = lcg * 1664525u + 1013904223u;
        return lcg;
    };
    for (int i = 0; i < 80; ++i) {
        uint16_t a = static_cast<uint16_t>(next_rand() & 0x7FFF);
        uint16_t b = static_cast<uint16_t>((next_rand() >> 1) & 0x7FFF);
        const auto& rm = rm_sweep[i % rm_sweep.size()];
        bool do_cmp = (i % 4) == 0;
        bool do_max = (i % 5) == 0;
        bool do_min = (i % 6) == 0;
        bool add = !do_max && !do_min && ((i & 1) == 0);
        bool sub = !do_max && !do_min && !add;
        Scenario sc{
            "fuzz_mix",
            add, sub, do_cmp, do_max, do_min,
            do_cmp && ((i & 1) == 0),
            do_cmp && ((i & 2) == 0),
            do_cmp && ((i & 4) != 0),
            do_cmp && ((i & 8) != 0),
            do_cmp && ((i & 3) == 0),
            rm[0], rm[1], rm[2], rm[3], rm[4],
            a, b,
            (i & 2) == 0, (i & 4) == 0,
            (i & 1) == 0,
            false, 0};
        if (!drive(dut, sc, expect)) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case010/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
