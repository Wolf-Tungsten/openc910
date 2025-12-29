#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_scalar_dp.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void tick(Vct_fcnvt_scalar_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

struct Scenario {
    uint32_t func;
    uint8_t imm0;
    uint8_t rm_global;
    uint64_t src;
};

struct Expected {
    bool ex1_src_l64;
    bool ex1_src_l32;
    bool ex1_src_l16;
    bool ex1_widden;
    bool ex1_narrow;
    bool ex1_equal;
    bool ex1_sover;
    bool ex1_src_si;
    bool ex1_src_float;
    bool ex1_dest_float;
    bool ex1_dest_si;
    bool ex1_dest_l64;
    bool ex1_dest_l32;
    bool ex1_dest_l16;
    bool ex1_dest_half;
    bool ex1_dest_single;
    bool ex1_dest_double;
    uint8_t ex1_rm;
};

Expected compute_expected(uint32_t func, uint8_t imm0, uint8_t rm_global) {
    Expected e{};
    bool func16 = (func >> 16) & 1u;
    bool func15 = (func >> 15) & 1u;
    bool func14 = (func >> 14) & 1u;
    bool func13 = (func >> 13) & 1u;

    e.ex1_widden = func14 && !func13;
    e.ex1_narrow = !func14 && func13;
    e.ex1_equal = !func13 && !func14;
    e.ex1_sover = func14 && func13;

    e.ex1_src_l64 = func16 || (func15 && e.ex1_narrow);
    e.ex1_src_l32 = (func15 && !e.ex1_narrow) || (!func16 && !func15 && e.ex1_narrow);
    e.ex1_src_l16 = (!func16 && !func15 && !e.ex1_narrow);

    e.ex1_src_si = func & 1u;
    e.ex1_src_float = (func >> 1) & 1u;
    e.ex1_dest_float = (func >> 2) & 1u;
    e.ex1_dest_si = (func >> 3) & 1u;

    e.ex1_dest_l64 = (e.ex1_src_l64 && e.ex1_equal) || (e.ex1_src_l32 && e.ex1_widden) ||
                     (e.ex1_src_l16 && e.ex1_sover);
    e.ex1_dest_l32 = (e.ex1_src_l32 && e.ex1_equal) || (e.ex1_src_l16 && e.ex1_widden) ||
                     (e.ex1_src_l64 && e.ex1_narrow);
    e.ex1_dest_l16 = (e.ex1_src_l32 && e.ex1_narrow) || (e.ex1_src_l16 && e.ex1_equal) ||
                     (e.ex1_src_l64 && e.ex1_sover);

    e.ex1_dest_half = e.ex1_dest_float && e.ex1_dest_l16;
    e.ex1_dest_single = e.ex1_dest_float && e.ex1_dest_l32;
    e.ex1_dest_double = e.ex1_dest_float && e.ex1_dest_l64;

    uint8_t vfalu_rm = (imm0 == 0x7) ? (rm_global & 0x7) : (imm0 & 0x7);
    bool rm_rne = vfalu_rm == 0;
    bool rm_rtz = vfalu_rm == 1;
    bool rm_rdn = vfalu_rm == 2;
    bool rm_rup = vfalu_rm == 3;
    bool rm_rmm = vfalu_rm == 4;
    e.ex1_rm = static_cast<uint8_t>((rm_rmm << 4) | (rm_rdn << 3) | (rm_rup << 2) | (rm_rtz << 1) | rm_rne);
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_scalar_dp dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.ex1_pipedown = 0;
    dut.ex3_pipedown = 0;
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);
    // Exercise gating false paths.
    dut.cp0_yy_clk_en = 0;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    dut.cp0_vfpu_icg_en = 0;
    tick(dut);
    dut.cp0_vfpu_icg_en = 1;
    tick(dut);

    std::vector<Scenario> scenarios = {
        {0x10007, 0x7, 0x2, 0x1122334455667788ULL},   // src64, equal path, rm from global
        {0x0A006, 0x0, 0x0, 0x0FEDCBA987654321ULL},   // narrow path with src32
        {0x0C008, 0x4, 0x3, 0x0101010101010101ULL},   // widden path with src32
        {0x16004, 0x2, 0x5, 0xFFFFFFFFFFFFFFFFULL},   // sover path with src64
        {0x02004, 0x1, 0x0, 0x123456789ABCDEF0ULL},   // narrow path, dest_half true, rm rtz
        {0x0200C, 0x3, 0x0, 0x00FF00FF00FF00FFULL},   // narrow with dest_si, rm rup
    };
    for (uint32_t i = 0; i < 64; ++i) {
        uint32_t func = ((i << 5) ^ (0x15555u + i * 17u)) & 0xFFFFF;
        uint8_t imm0 = static_cast<uint8_t>(i & 0x7);
        uint8_t rm_g = static_cast<uint8_t>((i + 3) & 0x7);
        uint64_t src = 0xDEADBEEFCAFEBABEULL ^ (static_cast<uint64_t>(i) << 32);
        scenarios.push_back({func, imm0, rm_g, src});
    }
    uint32_t lcg = 1u;
    auto next = [&]() {
        lcg = lcg * 1103515245u + 12345u;
        return lcg;
    };
    for (int i = 0; i < 10000; ++i) {
        uint32_t func = next() & 0xFFFFF;
        uint8_t imm0 = static_cast<uint8_t>(next() & 0x7);
        uint8_t rm_g = static_cast<uint8_t>(next() & 0x7);
        uint64_t src = (static_cast<uint64_t>(next()) << 32) | next();
        scenarios.push_back({func, imm0, rm_g, src});
    }

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        Expected e = compute_expected(sc.func, sc.imm0, sc.rm_global);

        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_imm0 = sc.imm0;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src;
        dut.vfpu_yy_xx_rm = sc.rm_global;

        dut.ex1_pipedown = 1;
        dut.eval();

        if (!expect(dut.dp_ex1_src == sc.src, "dp_ex1_src passthrough mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_src_l64 == e.ex1_src_l64, "ex1_src_l64 mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_src_l32 == e.ex1_src_l32, "ex1_src_l32 mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_src_l16 == e.ex1_src_l16, "ex1_src_l16 mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_widden == e.ex1_widden, "ex1_widden mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_narrow == e.ex1_narrow, "ex1_narrow mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_sover == e.ex1_sover, "ex1_sover mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_dest_float == e.ex1_dest_float, "ex1_dest_float mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_dest_double == e.ex1_dest_double, "ex1_dest_double mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_dest_single == e.ex1_dest_single, "ex1_dest_single mismatch")) {
            return 1;
        }
        if (!expect(dut.ex1_rm == e.ex1_rm, "ex1_rm mismatch")) {
            return 1;
        }

        tick(dut);
        dut.ex1_pipedown = 0;
        dut.eval();

        if (!expect(dut.ex2_dest_double == e.ex1_dest_double, "ex2_dest_double mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_single == e.ex1_dest_single, "ex2_dest_single mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_half == e.ex1_dest_half, "ex2_dest_half mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_float == e.ex1_dest_float, "ex2_dest_float mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_l64 == e.ex1_dest_l64, "ex2_dest_l64 mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_l32 == e.ex1_dest_l32, "ex2_dest_l32 mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_l16 == e.ex1_dest_l16, "ex2_dest_l16 mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_dest_si == e.ex1_dest_si, "ex2_dest_si mismatch")) {
            return 1;
        }
        if (!expect(dut.ex2_src_float == e.ex1_src_float, "ex2_src_float mismatch")) {
            return 1;
        }
    }

    dut.fcnvt_ex3_result = 0x1122334455667788ULL;
    dut.fcnvt_ex3_expt = 0x1F;
    dut.ex3_pipedown = 1;
    dut.eval();
    if (!expect(dut.fcnvt_forward_r_vld == 1, "forward_r_vld mismatch")) {
        return 1;
    }
    if (!expect(dut.fcnvt_ereg_forward_r_vld == 1, "ereg_forward_vld mismatch")) {
        return 1;
    }
    if (!expect(dut.fcnvt_forward_result == 0x1122334455667788ULL, "forward_result mismatch")) {
        return 1;
    }
    if (!expect(dut.fcnvt_ereg_forward_result == 0x1F, "ereg_forward_result mismatch")) {
        return 1;
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case017/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
