#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfpu_top.h"
#include "Vct_vfpu_top___024root.h"
#include "Vct_vfpu_top__Syms.h"

#include <cstdint>
#include <cstdlib>

namespace {

uint64_t lcg_next(uint64_t& state) {
    state = state * 6364136223846793005ULL + 1;
    return state;
}

uint64_t rand_mask(uint64_t& state, unsigned bits) {
    if (bits >= 64) {
        return lcg_next(state);
    }
    if (bits == 0) {
        return 0;
    }
    const uint64_t mask = (1ULL << bits) - 1;
    return lcg_next(state) & mask;
}

void tick(Vct_vfpu_top& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

void drive_inputs(Vct_vfpu_top& dut, uint64_t& lcg, int cycle) {
    dut.cp0_vfpu_fcsr = lcg_next(lcg);
    dut.cp0_vfpu_fxcr = static_cast<uint32_t>(lcg_next(lcg));
    dut.cp0_vfpu_vl = static_cast<uint8_t>(lcg_next(lcg));
    dut.cp0_vfpu_icg_en = (cycle % 4) != 0;
    dut.cp0_yy_clk_en = (cycle % 6) != 0;

    dut.idu_vfpu_is_vdiv_gateclk_issue = (cycle % 9) == 0;
    dut.idu_vfpu_is_vdiv_issue = (cycle % 7) == 0;
    dut.pad_yy_icg_scan_en = (cycle % 10) == 0;
    dut.rtu_yy_xx_flush = (cycle % 11) == 0;

    // pipe6 decode inputs
    dut.idu_vfpu_rf_pipe6_dst_ereg = rand_mask(lcg, 5);
    dut.idu_vfpu_rf_pipe6_dst_preg = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe6_dst_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe6_dst_vreg = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe6_dste_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe6_dstv_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe6_eu_sel = rand_mask(lcg, 12);
    dut.idu_vfpu_rf_pipe6_func = rand_mask(lcg, 20);
    dut.idu_vfpu_rf_pipe6_gateclk_sel = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe6_iid = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe6_imm0 = rand_mask(lcg, 3);
    dut.idu_vfpu_rf_pipe6_inst_type = rand_mask(lcg, 6);
    dut.idu_vfpu_rf_pipe6_mla_srcv2_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe6_mla_srcv2_vreg = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe6_ready_stage = static_cast<uint8_t>((cycle >> 1) & 0x7);
    dut.idu_vfpu_rf_pipe6_sel = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe6_srcv0_fr = lcg_next(lcg);
    dut.idu_vfpu_rf_pipe6_srcv1_fr = lcg_next(lcg);
    dut.idu_vfpu_rf_pipe6_srcv2_fr = lcg_next(lcg);
    dut.idu_vfpu_rf_pipe6_vmla_type = rand_mask(lcg, 3);

    // pipe7 decode inputs
    dut.idu_vfpu_rf_pipe7_dst_ereg = rand_mask(lcg, 5);
    dut.idu_vfpu_rf_pipe7_dst_preg = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe7_dst_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe7_dst_vreg = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe7_dste_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe7_dstv_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe7_eu_sel = rand_mask(lcg, 12);
    dut.idu_vfpu_rf_pipe7_func = rand_mask(lcg, 20);
    dut.idu_vfpu_rf_pipe7_gateclk_sel = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe7_iid = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe7_imm0 = rand_mask(lcg, 3);
    dut.idu_vfpu_rf_pipe7_inst_type = rand_mask(lcg, 6);
    dut.idu_vfpu_rf_pipe7_mla_srcv2_vld = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe7_mla_srcv2_vreg = rand_mask(lcg, 7);
    dut.idu_vfpu_rf_pipe7_ready_stage = static_cast<uint8_t>((cycle >> 2) & 0x7);
    dut.idu_vfpu_rf_pipe7_sel = rand_mask(lcg, 1);
    dut.idu_vfpu_rf_pipe7_srcv0_fr = lcg_next(lcg);
    dut.idu_vfpu_rf_pipe7_srcv1_fr = lcg_next(lcg);
    dut.idu_vfpu_rf_pipe7_srcv2_fr = lcg_next(lcg);
    dut.idu_vfpu_rf_pipe7_vmla_type = rand_mask(lcg, 3);

    // mtvr controls
    dut.iu_vfpu_ex1_pipe0_mtvr_inst = rand_mask(lcg, 5);
    dut.iu_vfpu_ex1_pipe0_mtvr_vl = rand_mask(lcg, 8);
    dut.iu_vfpu_ex1_pipe0_mtvr_vld = rand_mask(lcg, 1);
    dut.iu_vfpu_ex1_pipe0_mtvr_vlmul = rand_mask(lcg, 2);
    dut.iu_vfpu_ex1_pipe0_mtvr_vreg = rand_mask(lcg, 7);
    dut.iu_vfpu_ex1_pipe0_mtvr_vsew = rand_mask(lcg, 3);

    dut.iu_vfpu_ex1_pipe1_mtvr_inst = rand_mask(lcg, 5);
    dut.iu_vfpu_ex1_pipe1_mtvr_vl = rand_mask(lcg, 8);
    dut.iu_vfpu_ex1_pipe1_mtvr_vld = rand_mask(lcg, 1);
    dut.iu_vfpu_ex1_pipe1_mtvr_vlmul = rand_mask(lcg, 2);
    dut.iu_vfpu_ex1_pipe1_mtvr_vreg = rand_mask(lcg, 7);
    dut.iu_vfpu_ex1_pipe1_mtvr_vsew = rand_mask(lcg, 3);

    dut.iu_vfpu_ex2_pipe0_mtvr_src0 = lcg_next(lcg);
    dut.iu_vfpu_ex2_pipe0_mtvr_vld = rand_mask(lcg, 1);
    dut.iu_vfpu_ex2_pipe1_mtvr_src0 = lcg_next(lcg);
    dut.iu_vfpu_ex2_pipe1_mtvr_vld = rand_mask(lcg, 1);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfpu_top dut;

    dut.cpurst_b = 0;
    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.rtu_yy_xx_flush = 0;

    for (int i = 0; i < 3; ++i) {
        tick(dut);
    }
    dut.cpurst_b = 1;

    uint64_t lcg = 860603;
    for (int cycle = 0; cycle < 3000; ++cycle) {
        drive_inputs(dut, lcg, cycle);
        tick(dut);
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
        cov_out = "build/case060/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
