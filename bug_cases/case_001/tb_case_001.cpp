#include "Vct_piu_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void clear_inputs(Vct_piu_top *dut) {
    dut->bmbif_piu_ctcq_grant = 0;
    dut->bmbif_piu_ncq_grant = 0;
    dut->bmbif_piu_snb0_grant = 0;
    dut->bmbif_piu_snb1_grant = 0;
    dut->ciu_chr2_bar_dis = 0;
    dut->ciu_icg_en = 0;
    dut->ciu_top_clk = 0;
    dut->cpurst_b = 0;
    dut->ctcq_piu_acbus = 0;
    dut->ctcq_piu_acvalid = 0;
    dut->ctcq_piu_ar_grant = 0;
    dut->ctcq_piu_bar_cmplt = 0;
    dut->ctcq_piu_cr_grant = 0;
    dut->ctcq_piu_rvalid = 0;
    for (int i = 0; i < 17; ++i) {
        dut->ctcq_piux_rbus[i] = 0;
    }
    dut->ebiu_piu_no_op = 0;
    dut->forever_cpuclk = 0;
    dut->ibiu_ciu_acready = 0;
    dut->ibiu_ciu_araddr = 0;
    dut->ibiu_ciu_arbar = 0;
    dut->ibiu_ciu_arburst = 0;
    dut->ibiu_ciu_arcache = 0;
    dut->ibiu_ciu_ardomain = 0;
    dut->ibiu_ciu_arid = 0;
    dut->ibiu_ciu_arlen = 0;
    dut->ibiu_ciu_arlock = 0;
    dut->ibiu_ciu_arprot = 0;
    dut->ibiu_ciu_arsize = 0;
    dut->ibiu_ciu_arsnoop = 0;
    dut->ibiu_ciu_aruser = 0;
    dut->ibiu_ciu_arvalid = 0;
    dut->ibiu_ciu_awaddr = 0;
    dut->ibiu_ciu_awbar = 0;
    dut->ibiu_ciu_awburst = 0;
    dut->ibiu_ciu_awcache = 0;
    dut->ibiu_ciu_awdomain = 0;
    dut->ibiu_ciu_awid = 0;
    dut->ibiu_ciu_awlen = 0;
    dut->ibiu_ciu_awlock = 0;
    dut->ibiu_ciu_awprot = 0;
    dut->ibiu_ciu_awsize = 0;
    dut->ibiu_ciu_awsnoop = 0;
    dut->ibiu_ciu_awunique = 0;
    dut->ibiu_ciu_awuser = 0;
    dut->ibiu_ciu_awvalid = 0;
    dut->ibiu_ciu_back = 0;
    dut->ibiu_ciu_bready = 0;
    for (int i = 0; i < 4; ++i) {
        dut->ibiu_ciu_cddata[i] = 0;
    }
    dut->ibiu_ciu_cderr = 0;
    dut->ibiu_ciu_cdlast = 0;
    dut->ibiu_ciu_cdvalid = 0;
    dut->ibiu_ciu_crresp = 0;
    dut->ibiu_ciu_crvalid = 0;
    dut->ibiu_ciu_rack = 0;
    dut->ibiu_ciu_rready = 0;
    for (int i = 0; i < 4; ++i) {
        dut->ibiu_ciu_wdata[i] = 0;
    }
    dut->ibiu_ciu_werr = 0;
    dut->ibiu_ciu_wlast = 0;
    dut->ibiu_ciu_wns = 0;
    dut->ibiu_ciu_wstrb = 0;
    dut->ibiu_ciu_wvalid = 0;
    dut->ncq_piu_ar_grant = 0;
    dut->ncq_piu_aw_grant = 0;
    dut->ncq_piu_bar_cmplt = 0;
    dut->ncq_piu_bbus = 0;
    dut->ncq_piu_bvalid = 0;
    dut->ncq_piu_rvalid = 0;
    dut->ncq_piu_wcd_grant = 0;
    for (int i = 0; i < 17; ++i) {
        dut->ncq_piux_rbus[i] = 0;
    }
    dut->pad_yy_icg_scan_en = 0;
    dut->piu_no = 0;
    dut->snb0_piu_acbus = 0;
    dut->snb0_piu_acvalid = 0;
    dut->snb0_piu_ar_grant = 0;
    dut->snb0_piu_aw_grant = 0;
    dut->snb0_piu_bar_cmplt = 0;
    dut->snb0_piu_bvalid = 0;
    dut->snb0_piu_cr_grant = 0;
    dut->snb0_piu_rvalid = 0;
    dut->snb0_piu_wcd_grant = 0;
    dut->snb0_piux_aw_sid = 0;
    dut->snb0_piux_bbus = 0;
    for (int i = 0; i < 17; ++i) {
        dut->snb0_piux_rbus[i] = 0;
    }
    dut->snb1_piu_acbus = 0;
    dut->snb1_piu_acvalid = 0;
    dut->snb1_piu_ar_grant = 0;
    dut->snb1_piu_aw_grant = 0;
    dut->snb1_piu_bar_cmplt = 0;
    dut->snb1_piu_bvalid = 0;
    dut->snb1_piu_cr_grant = 0;
    dut->snb1_piu_rvalid = 0;
    dut->snb1_piu_wcd_grant = 0;
    dut->snb1_piux_aw_sid = 0;
    dut->snb1_piux_bbus = 0;
    for (int i = 0; i < 17; ++i) {
        dut->snb1_piux_rbus[i] = 0;
    }
}

static void tick(VerilatedContext *ctx, Vct_piu_top *dut) {
    dut->ciu_top_clk = 0;
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->ciu_top_clk = 1;
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::randReset(0);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_piu_top(ctx);

    clear_inputs(dut);
    dut->ciu_icg_en = 1;

    dut->cpurst_b = 0;
    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->cpurst_b = 1;

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }

    if (dut->ciu_ibiu_acvalid != 0) {
        std::cerr << "[TB] ciu_ibiu_acvalid expected 0 after reset" << std::endl;
        return 1;
    }

    uint32_t prng = 0x1ACED00Du;
    for (int cycle = 0; cycle < 1024; ++cycle) {
        prng = prng * 1103515245u + 12345u;
        const uint32_t pattern = prng;
        const bool toggle = (pattern & 1u) != 0u;

        dut->ciu_chr2_bar_dis = (pattern >> 3) & 1u;
        dut->ctcq_piu_acvalid = toggle;
        dut->ctcq_piu_rvalid = (pattern >> 2) & 1u;
        dut->ctcq_piu_ar_grant = (pattern >> 4) & 1u;
        dut->ctcq_piu_cr_grant = (pattern >> 5) & 1u;
        dut->ctcq_piu_bar_cmplt = (pattern >> 6) & 1u;
        dut->ctcq_piu_acbus = (pattern << 3) ^ 0x55u;
        for (int i = 0; i < 17; ++i) {
            dut->ctcq_piux_rbus[i] = pattern ^ static_cast<uint32_t>(i * 0x55u);
        }

        dut->ibiu_ciu_arvalid = toggle;
        dut->ibiu_ciu_awvalid = !toggle;
        dut->ibiu_ciu_cdvalid = (pattern >> 1) & 1u;
        dut->ibiu_ciu_crvalid = (pattern >> 2) & 1u;
        dut->ibiu_ciu_wvalid = (pattern >> 3) & 1u;
        dut->ibiu_ciu_araddr = (pattern << 8) | 0xA5u;
        dut->ibiu_ciu_awaddr = (pattern << 7) | 0x5Au;
        dut->ibiu_ciu_arbar = (pattern >> 10) & 0x3u;
        dut->ibiu_ciu_awbar = (pattern >> 12) & 0x3u;
        dut->ibiu_ciu_arburst = (pattern >> 14) & 0x3u;
        dut->ibiu_ciu_awburst = (pattern >> 16) & 0x3u;
        dut->ibiu_ciu_arcache = (pattern >> 18) & 0xFu;
        dut->ibiu_ciu_awcache = (pattern >> 20) & 0xFu;
        dut->ibiu_ciu_ardomain = (pattern >> 22) & 0x3u;
        dut->ibiu_ciu_awdomain = (pattern >> 24) & 0x3u;
        dut->ibiu_ciu_arid = pattern & 0x1Fu;
        dut->ibiu_ciu_awid = (pattern >> 5) & 0x1Fu;
        dut->ibiu_ciu_arlen = (pattern >> 8) & 0x3u;
        dut->ibiu_ciu_awlen = (pattern >> 10) & 0x3u;
        dut->ibiu_ciu_arlock = (pattern >> 12) & 1u;
        dut->ibiu_ciu_awlock = (pattern >> 13) & 1u;
        dut->ibiu_ciu_arprot = (pattern >> 14) & 0x7u;
        dut->ibiu_ciu_awprot = (pattern >> 17) & 0x7u;
        dut->ibiu_ciu_arsize = (pattern >> 20) & 0x7u;
        dut->ibiu_ciu_awsize = (pattern >> 23) & 0x7u;
        dut->ibiu_ciu_arsnoop = (pattern >> 5) & 0xFu;
        dut->ibiu_ciu_awsnoop = (pattern >> 9) & 0x7u;
        dut->ibiu_ciu_aruser = (pattern >> 13) & 0x7u;
        dut->ibiu_ciu_awunique = (pattern >> 16) & 1u;
        dut->ibiu_ciu_awuser = (pattern >> 17) & 1u;
        for (int i = 0; i < 4; ++i) {
            dut->ibiu_ciu_cddata[i] = pattern ^ static_cast<uint32_t>(i * 0x33u);
            dut->ibiu_ciu_wdata[i] = pattern ^ static_cast<uint32_t>(i * 0x77u);
        }
        dut->ibiu_ciu_wstrb = 0xFFFFu;
        dut->ibiu_ciu_wlast = (pattern >> 6) & 1u;
        dut->ibiu_ciu_werr = (pattern >> 7) & 1u;
        dut->ibiu_ciu_wns = (pattern >> 8) & 1u;
        dut->ibiu_ciu_cderr = (pattern >> 9) & 1u;
        dut->ibiu_ciu_cdlast = (pattern >> 10) & 1u;
        dut->ibiu_ciu_crresp = (pattern >> 11) & 0x1Fu;
        dut->ibiu_ciu_bready = (pattern >> 12) & 1u;
        dut->ibiu_ciu_rready = (pattern >> 13) & 1u;
        dut->ibiu_ciu_rack = (pattern >> 14) & 1u;

        dut->ncq_piu_ar_grant = toggle;
        dut->ncq_piu_aw_grant = !toggle;
        dut->ncq_piu_bvalid = (pattern >> 15) & 1u;
        dut->ncq_piu_rvalid = (pattern >> 16) & 1u;
        dut->ncq_piu_wcd_grant = (pattern >> 17) & 1u;
        dut->ncq_piu_bbus = (pattern ^ 0xAAu) & 0x3FFFu;
        dut->ncq_piu_bar_cmplt = (pattern >> 18) & 1u;
        for (int i = 0; i < 17; ++i) {
            dut->ncq_piux_rbus[i] = pattern + static_cast<uint32_t>(i * 3u);
        }

        dut->snb0_piu_acvalid = (pattern >> 19) & 1u;
        dut->snb0_piu_ar_grant = (pattern >> 20) & 1u;
        dut->snb0_piu_aw_grant = (pattern >> 21) & 1u;
        dut->snb0_piu_bvalid = (pattern >> 22) & 1u;
        dut->snb0_piu_rvalid = (pattern >> 23) & 1u;
        dut->snb0_piu_wcd_grant = (pattern >> 24) & 1u;
        dut->snb0_piu_bar_cmplt = (pattern >> 25) & 1u;
        dut->snb0_piu_acbus = (pattern ^ 0x55u) & 0x7FFFFFFu;
        dut->snb0_piux_bbus = (pattern ^ 0x5Au) & 0x3FFFu;
        dut->snb0_piux_aw_sid = (pattern >> 27) & 0x1Fu;
        for (int i = 0; i < 17; ++i) {
            dut->snb0_piux_rbus[i] = pattern ^ static_cast<uint32_t>(i * 0x13u);
        }

        dut->snb1_piu_acvalid = (pattern >> 26) & 1u;
        dut->snb1_piu_ar_grant = (pattern >> 27) & 1u;
        dut->snb1_piu_aw_grant = (pattern >> 28) & 1u;
        dut->snb1_piu_bvalid = (pattern >> 29) & 1u;
        dut->snb1_piu_rvalid = (pattern >> 30) & 1u;
        dut->snb1_piu_wcd_grant = (pattern >> 31) & 1u;
        dut->snb1_piu_bar_cmplt = (pattern >> 4) & 1u;
        dut->snb1_piu_acbus = (pattern ^ 0x33u) & 0x7FFFFFFu;
        dut->snb1_piux_bbus = (pattern ^ 0x66u) & 0x3FFFu;
        dut->snb1_piux_aw_sid = (pattern >> 9) & 0x1Fu;
        for (int i = 0; i < 17; ++i) {
            dut->snb1_piux_rbus[i] = pattern ^ static_cast<uint32_t>(i * 0x1Bu);
        }

        dut->bmbif_piu_ctcq_grant = (pattern >> 1) & 1u;
        dut->bmbif_piu_ncq_grant = (pattern >> 2) & 1u;
        dut->bmbif_piu_snb0_grant = (pattern >> 3) & 1u;
        dut->bmbif_piu_snb1_grant = (pattern >> 4) & 1u;
        dut->ebiu_piu_no_op = (pattern >> 5) & 1u;
        dut->pad_yy_icg_scan_en = (pattern >> 6) & 1u;
        dut->piu_no = pattern & 0x7u;

        tick(ctx, dut);
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
