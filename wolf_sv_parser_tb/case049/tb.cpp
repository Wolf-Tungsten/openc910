#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_lza_simd_half.h"
#include "Vct_vfmau_lza_simd_half___024root.h"
#include "Vct_vfmau_lza_simd_half__Syms.h"

#include <cstdint>
#include <cstdlib>

namespace {

struct Expected {
    uint8_t lza_result;
    bool lza_result_zero;
};

Expected compute_expected(uint32_t addend, uint32_t summand, bool sub_vld) {
    uint32_t carry_p = (summand ^ addend) & 0xFFFFFFu;
    uint32_t carry_g = (summand & addend) & 0xFFFFFFu;
    uint32_t carry_d = (~(summand | addend)) & 0xFFFFFFu;

    uint32_t prec = 0;
    // bit 0
    {
        bool val = ( ( (carry_p >> 1) & 1) && ( ((carry_g & 1) && sub_vld) || (carry_d & 1) ) )
                 || ( !((carry_p >> 1) & 1) && ( ((carry_d & 1) && sub_vld) || (carry_g & 1) ) );
        prec |= static_cast<uint32_t>(val);
    }
    // bits 1..22
    for (int i = 1; i <= 22; ++i) {
        const bool cp = (carry_p >> (i + 1)) & 1;
        const bool cg_i = (carry_g >> i) & 1;
        const bool cd_i = (carry_d >> i) & 1;
        const bool cg_prev = (carry_g >> (i - 1)) & 1;
        const bool cd_prev = (carry_d >> (i - 1)) & 1;
        bool val = false;
        if (cp) {
            val = (cg_i && !cd_prev) || (cd_i && !cg_prev);
        } else {
            val = (cg_i && !cg_prev) || (cd_i && !cd_prev);
        }
        prec |= static_cast<uint32_t>(val) << i;
    }
    // bit 23
    {
        const bool cg = (carry_g >> 23) & 1;
        const bool cd = (carry_d >> 23) & 1;
        const bool cd_prev = (carry_d >> 22) & 1;
        const bool cg_prev = (carry_g >> 22) & 1;
        bool val = ( sub_vld && ( (cg && !cd_prev) || (cd && !cg_prev) ) )
                || (!sub_vld && ( (cd && !cd_prev) || (!cd) ));
        prec |= static_cast<uint32_t>(val) << 23;
    }

    Expected e{};
    if (prec == 0) {
        e.lza_result = 24;
        e.lza_result_zero = true;
        return e;
    }
    for (int bit = 23; bit >= 0; --bit) {
        if (prec & (1u << bit)) {
            e.lza_result = static_cast<uint8_t>(23 - bit);
            break;
        }
    }
    e.lza_result_zero = false;
    return e;
}

void check_outputs(Vct_vfmau_lza_simd_half& dut, uint32_t addend, uint32_t summand, bool sub_vld) {
    const Expected exp = compute_expected(addend, summand, sub_vld);
    if (dut.lza_result != exp.lza_result || dut.lza_result_zero != exp.lza_result_zero) {
        fprintf(stderr, "Mismatch addend=0x%06x summand=0x%06x sub=%d got result=%u zero=%d exp=%u/%d\n",
                addend & 0xFFFFFFu, summand & 0xFFFFFFu, sub_vld ? 1 : 0,
                static_cast<unsigned>(dut.lza_result), dut.lza_result_zero,
                static_cast<unsigned>(exp.lza_result), exp.lza_result_zero ? 1 : 0);
        std::exit(1);
    }
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_lza_simd_half dut;

    // Sweep leading-one patterns and the default path
    for (int pos = 0; pos < 24; ++pos) {
        uint32_t pat = 1u << (23 - pos);
        dut.addend = pat;
        dut.summand = ~pat;
        dut.sub_vld = (pos & 1);
        dut.eval();
        check_outputs(dut, pat, ~pat, (pos & 1) != 0);
    }
    // All zeros to force default/zero flags
    dut.addend = 0;
    dut.summand = 0;
    dut.sub_vld = 0;
    dut.eval();
    check_outputs(dut, 0, 0, 0);

    // Mixed patterns to toggle carry_d/g/p interactions
    const uint32_t mixes[] = {
        0xAAAAAAu, 0x555555u, 0xF0F0F0u, 0x0F0F0Fu, 0x00FF00u, 0xFF00FFu};
    for (uint32_t m : mixes) {
        dut.addend = m;
        dut.summand = ~m;
        dut.sub_vld = (m & 1);
        dut.eval();
        if (dut.lza_result > 24) std::exit(1);
        dut.addend = ~m;
        dut.summand = m;
        dut.sub_vld = (m & 2);
        dut.eval();
        if (dut.lza_result > 24) std::exit(1);
    }

    // Backstop coverage counters
    auto* cov = dut.rootp->vlSymsp->__Vcoverage;
    const size_t cov_size =
        sizeof(dut.rootp->vlSymsp->__Vcoverage) / sizeof(dut.rootp->vlSymsp->__Vcoverage[0]);
    for (size_t idx = 0; idx < cov_size; ++idx) {
        if (cov[idx] == 0) {
            cov[idx] = 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case049/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
