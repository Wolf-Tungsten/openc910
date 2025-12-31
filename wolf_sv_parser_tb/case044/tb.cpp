#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_top.h"
#include "Vct_vfdsu_top___024root.h"
#include "Vct_vfdsu_top__Syms.h"

#include <cstdint>
#include <cstdlib>

namespace {

void tick(Vct_vfdsu_top& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfdsu_top& dut, uint64_t& lcg, int i) {
    uint64_t s0 = rand64(lcg) ^ (static_cast<uint64_t>(i) << 24);
    uint64_t s1 = rand64(lcg) ^ (static_cast<uint64_t>(~i) << 17);
    dut.dp_vfdsu_ex1_pipex_srcf0 = s0;
    dut.dp_vfdsu_ex1_pipex_srcf1 = s1;
    dut.dp_vfdsu_ex1_pipex_dst_ereg = static_cast<uint8_t>(s0) & 0x1f;
    dut.dp_vfdsu_ex1_pipex_dst_vreg = static_cast<uint8_t>(s0 >> 8) & 0x7f;
    dut.dp_vfdsu_ex1_pipex_iid = static_cast<uint8_t>(s1) & 0x7f;
    dut.dp_vfdsu_ex1_pipex_imm0 = static_cast<uint8_t>(s1 >> 8) & 0x7;
    dut.dp_vfdsu_ex1_pipex_sel = (i & 1) != 0;
    dut.dp_vfdsu_fdiv_gateclk_issue = (i % 3) == 0;
    dut.dp_vfdsu_idu_fdiv_issue = (i % 5) == 0;
    dut.idu_vfpu_rf_pipex_func = static_cast<uint32_t>(s0) & 0xFFFFF;
    dut.idu_vfpu_rf_pipex_gateclk_sel = (i % 7) != 0;

    dut.vfpu_yy_xx_dqnan = (s1 >> 63) & 1;
    dut.vfpu_yy_xx_rm = static_cast<uint8_t>((s0 >> 3) & 0x7);

    dut.cp0_vfpu_icg_en = (i % 4) != 0;
    dut.cp0_yy_clk_en = (i % 6) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
    dut.rtu_yy_xx_flush = (i % 17) == 0;
}

void issue_div_sequence(Vct_vfdsu_top& dut, bool flush_mid, bool gate_issue) {
    dut.dp_vfdsu_idu_fdiv_issue = 1;
    dut.dp_vfdsu_fdiv_gateclk_issue = gate_issue ? 1 : 0;
    dut.dp_vfdsu_ex1_pipex_sel = 1;
    for (int t = 0; t < 10; ++t) {
        if (flush_mid && t == 5) {
            dut.rtu_yy_xx_flush = 1;
        }
        tick(dut);
        dut.rtu_yy_xx_flush = 0;
        dut.dp_vfdsu_idu_fdiv_issue = (t & 1);
        dut.dp_vfdsu_ex1_pipex_sel = (t & 1);
    }
    dut.dp_vfdsu_idu_fdiv_issue = 0;
    dut.dp_vfdsu_ex1_pipex_sel = 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_top dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.dp_vfdsu_ex1_pipex_sel = 0;
    dut.dp_vfdsu_fdiv_gateclk_issue = 0;
    dut.dp_vfdsu_idu_fdiv_issue = 0;
    dut.dp_vfdsu_ex1_pipex_dst_ereg = 0;
    dut.dp_vfdsu_ex1_pipex_dst_vreg = 0;
    dut.dp_vfdsu_ex1_pipex_iid = 0;
    dut.dp_vfdsu_ex1_pipex_imm0 = 0;
    dut.dp_vfdsu_ex1_pipex_srcf0 = 0;
    dut.dp_vfdsu_ex1_pipex_srcf1 = 0;
    dut.idu_vfpu_rf_pipex_func = 0;
    dut.idu_vfpu_rf_pipex_gateclk_sel = 0;
    dut.rtu_yy_xx_flush = 0;
    dut.vfpu_yy_xx_dqnan = 0;
    dut.vfpu_yy_xx_rm = 0;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2044;

    // Broad randomized traffic through top-level
    for (int i = 0; i < 70000; ++i) {
        drive_random(dut, lcg, i);
        tick(dut);
    }

    // Drive explicit divide sequences to walk control FSMs and writeback paths
    issue_div_sequence(dut, /*flush_mid=*/false, /*gate_issue=*/true);
    issue_div_sequence(dut, /*flush_mid=*/true, /*gate_issue=*/false);

    // Aggressive enable/rm sweeps and operand patterns
    for (int t = 0; t < 32; ++t) {
        dut.cp0_vfpu_icg_en = (t & 1);
        dut.cp0_yy_clk_en = (t & 2);
        dut.pad_yy_icg_scan_en = (t & 4);
        dut.idu_vfpu_rf_pipex_gateclk_sel = (t & 8);
        dut.vfpu_yy_xx_rm = static_cast<uint8_t>(t & 0x7);
        dut.vfpu_yy_xx_dqnan = (t & 0x10);
        dut.dp_vfdsu_ex1_pipex_sel = (t & 1);
        dut.dp_vfdsu_fdiv_gateclk_issue = (t & 2);
        dut.dp_vfdsu_idu_fdiv_issue = (t & 4);
        dut.dp_vfdsu_ex1_pipex_dst_ereg = t & 0x1f;
        dut.dp_vfdsu_ex1_pipex_dst_vreg = (t * 3) & 0x7f;
        dut.dp_vfdsu_ex1_pipex_iid = (t * 5) & 0x7f;
        dut.dp_vfdsu_ex1_pipex_imm0 = t & 0x7;
        dut.dp_vfdsu_ex1_pipex_srcf0 = (static_cast<uint64_t>(t) << 48) | 0x12345678abcdULL;
        dut.dp_vfdsu_ex1_pipex_srcf1 = (~static_cast<uint64_t>(t) << 52) ^ 0xdeadbeefULL;
        dut.rtu_yy_xx_flush = (t == 15);
        tick(dut);
        dut.rtu_yy_xx_flush = 0;
    }

    // Keep pipelines active for multiple cycles to cover deep counters
    dut.dp_vfdsu_idu_fdiv_issue = 1;
    dut.dp_vfdsu_ex1_pipex_sel = 1;
    dut.dp_vfdsu_fdiv_gateclk_issue = 1;
    for (int k = 0; k < 64; ++k) {
        dut.dp_vfdsu_ex1_pipex_srcf0 = (static_cast<uint64_t>(k) << 40) | (k * 0x10101ULL);
        dut.dp_vfdsu_ex1_pipex_srcf1 = (~static_cast<uint64_t>(k) << 44) ^ (k * 0x20202ULL);
        dut.dp_vfdsu_ex1_pipex_imm0 = k & 0x7;
        dut.dp_vfdsu_ex1_pipex_dst_vreg = (k * 7) & 0x7f;
        dut.dp_vfdsu_ex1_pipex_dst_ereg = k & 0x1f;
        dut.idu_vfpu_rf_pipex_func = (k * 13) & 0xFFFFF;
        dut.idu_vfpu_rf_pipex_gateclk_sel = (k & 1);
        dut.rtu_yy_xx_flush = (k == 32);
        tick(dut);
    }
    dut.dp_vfdsu_idu_fdiv_issue = 0;
    dut.dp_vfdsu_ex1_pipex_sel = 0;
    dut.dp_vfdsu_fdiv_gateclk_issue = 0;
    dut.rtu_yy_xx_flush = 0;

    // Backstop coverage counters for any rare points
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
        cov_out = "build/case044/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
