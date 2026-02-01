#include "Vct_piu_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>

static uint64_t next_rand(uint64_t &state) {
    if (state == 0) {
        state = 0x9e3779b97f4a7c15ULL;
    }
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

static void drive_inputs(Vct_piu_top *dut, uint64_t &state) {
    auto next = [&]() -> uint64_t { return next_rand(state); };

    dut->bmbif_piu_ctcq_grant = next();
    dut->bmbif_piu_ncq_grant = next();
    dut->bmbif_piu_snb0_grant = next();
    dut->bmbif_piu_snb1_grant = next();
    dut->ciu_chr2_bar_dis = next();
    dut->ctcq_piu_bar_cmplt = next();
    dut->ctcq_piu_rvalid = next();
    dut->ebiu_piu_no_op = next();
    dut->ncq_piu_bar_cmplt = next();
    dut->ncq_piu_bvalid = next();
    dut->ncq_piu_rvalid = next();
    dut->snb0_piu_bar_cmplt = next();
    dut->snb0_piu_bvalid = next();
    dut->snb0_piu_rvalid = next();
    dut->snb1_piu_bar_cmplt = next();
    dut->snb1_piu_bvalid = next();
    dut->snb1_piu_rvalid = next();
    dut->ctcq_piu_acvalid = next();
    dut->ctcq_piu_ar_grant = next();
    dut->ctcq_piu_cr_grant = next();
    dut->ibiu_ciu_acready = next();
    dut->ibiu_ciu_arbar = next();
    dut->ibiu_ciu_arburst = next();
    dut->ibiu_ciu_arcache = next();
    dut->ibiu_ciu_ardomain = next();
    dut->ibiu_ciu_arid = next();
    dut->ibiu_ciu_arlen = next();
    dut->ibiu_ciu_arlock = next();
    dut->ibiu_ciu_arprot = next();
    dut->ibiu_ciu_arsize = next();
    dut->ibiu_ciu_arsnoop = next();
    dut->ibiu_ciu_aruser = next();
    dut->ibiu_ciu_arvalid = next();
    dut->ibiu_ciu_awbar = next();
    dut->ibiu_ciu_awburst = next();
    dut->ibiu_ciu_awcache = next();
    dut->ibiu_ciu_awdomain = next();
    dut->ibiu_ciu_awid = next();
    dut->ibiu_ciu_awlen = next();
    dut->ibiu_ciu_awlock = next();
    dut->ibiu_ciu_awprot = next();
    dut->ibiu_ciu_awsize = next();
    dut->ibiu_ciu_awsnoop = next();
    dut->ibiu_ciu_awunique = next();
    dut->ibiu_ciu_awuser = next();
    dut->ibiu_ciu_awvalid = next();
    dut->ibiu_ciu_back = next();
    dut->ibiu_ciu_bready = next();
    dut->ibiu_ciu_cderr = next();
    dut->ibiu_ciu_cdlast = next();
    dut->ibiu_ciu_cdvalid = next();
    dut->ibiu_ciu_crresp = next();
    dut->ibiu_ciu_crvalid = next();
    dut->ibiu_ciu_rack = next();
    dut->ibiu_ciu_rready = next();
    dut->ibiu_ciu_werr = next();
    dut->ibiu_ciu_wlast = next();
    dut->ibiu_ciu_wns = next();
    dut->ibiu_ciu_wvalid = next();
    dut->ncq_piu_ar_grant = next();
    dut->ncq_piu_aw_grant = next();
    dut->ncq_piu_wcd_grant = next();
    dut->piu_no = next();
    dut->snb0_piu_acvalid = next();
    dut->snb0_piu_ar_grant = next();
    dut->snb0_piu_aw_grant = next();
    dut->snb0_piu_cr_grant = next();
    dut->snb0_piu_wcd_grant = next();
    dut->snb0_piux_aw_sid = next();
    dut->snb1_piu_acvalid = next();
    dut->snb1_piu_ar_grant = next();
    dut->snb1_piu_aw_grant = next();
    dut->snb1_piu_cr_grant = next();
    dut->snb1_piu_wcd_grant = next();
    dut->snb1_piux_aw_sid = next();
    dut->ibiu_ciu_wstrb = next();
    dut->ncq_piu_bbus = next();
    dut->snb0_piux_bbus = next();
    dut->snb1_piux_bbus = next();
    for (int i = 0; i < 17; ++i) {
        dut->ctcq_piux_rbus[i] = static_cast<vluint32_t>(next());
    }
    for (int i = 0; i < 4; ++i) {
        dut->ibiu_ciu_cddata[i] = static_cast<vluint32_t>(next());
    }
    for (int i = 0; i < 4; ++i) {
        dut->ibiu_ciu_wdata[i] = static_cast<vluint32_t>(next());
    }
    for (int i = 0; i < 17; ++i) {
        dut->ncq_piux_rbus[i] = static_cast<vluint32_t>(next());
    }
    for (int i = 0; i < 17; ++i) {
        dut->snb0_piux_rbus[i] = static_cast<vluint32_t>(next());
    }
    for (int i = 0; i < 17; ++i) {
        dut->snb1_piux_rbus[i] = static_cast<vluint32_t>(next());
    }
    dut->ctcq_piu_acbus = next();
    dut->ibiu_ciu_araddr = next();
    dut->ibiu_ciu_awaddr = next();
    dut->snb0_piu_acbus = next();
    dut->snb1_piu_acbus = next();
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
    uint64_t seed = 0x1234abcd5678ef00ULL;

    dut->ciu_top_clk = 0;
    dut->forever_cpuclk = 0;
    dut->ciu_icg_en = 1;
    dut->pad_yy_icg_scan_en = 0;

    dut->cpurst_b = 0;
    for (int i = 0; i < 10; ++i) {
        drive_inputs(dut, seed);
        tick(ctx, dut);
    }
    dut->cpurst_b = 1;

    for (int i = 0; i < 500; ++i) {
        drive_inputs(dut, seed);
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
