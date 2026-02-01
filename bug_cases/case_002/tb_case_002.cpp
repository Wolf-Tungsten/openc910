#include "Vct_ciu_ncq.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static uint64_t next_rand(uint64_t &state) {
    if (state == 0) {
        state = 0x9e3779b97f4a7c15ULL;
    }
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

template <typename T>
static void fill_wide(T &bus, int words, uint64_t &state, bool zero) {
    for (int i = 0; i < words; ++i) {
        bus[i] = zero ? 0u : static_cast<vluint32_t>(next_rand(state));
    }
}

static void drive_inputs(Vct_ciu_ncq *dut, uint64_t &state, bool zero) {
    auto pick = [&]() -> vluint64_t {
        return zero ? 0u : static_cast<vluint64_t>(next_rand(state));
    };

    dut->apbif_ncq_ar_grant = pick();
    dut->apbif_ncq_aw_grant = pick();
    dut->apbif_ncq_bid = pick();
    dut->apbif_ncq_bresp = pick();
    dut->apbif_ncq_bvalid = pick();
    fill_wide(dut->apbif_ncq_rdata, 4, state, zero);
    dut->apbif_ncq_rid = pick();
    dut->apbif_ncq_rlast = pick();
    dut->apbif_ncq_rresp = pick();
    dut->apbif_ncq_rvalid = pick();
    dut->apbif_ncq_w_grant = pick();
    dut->bmbif_ncq_bar_req = pick();
    dut->bmbif_ncq_mid = pick();
    dut->bmbif_ncq_req_bus = pick();
    dut->ciu_chr2_bar_dis = pick();
    dut->ciu_so_ostd_dis = pick();
    dut->ebiu_ncq_ar_grant = pick();
    dut->ebiu_ncq_ar_grant_gate = pick();
    dut->ebiu_ncq_aw_grant = pick();
    dut->ebiu_ncq_aw_grant_gated = pick();
    dut->ebiu_ncq_bid = pick();
    dut->ebiu_ncq_bresp = pick();
    dut->ebiu_ncq_bvalid = pick();
    fill_wide(dut->ebiu_ncq_rdata, 4, state, zero);
    dut->ebiu_ncq_rid = pick();
    dut->ebiu_ncq_rlast = pick();
    dut->ebiu_ncq_rresp = pick();
    dut->ebiu_ncq_rvalid = pick();
    dut->ebiu_ncq_w_grant = pick();
    fill_wide(dut->piu0_ncq_ar_bus, 3, state, zero);
    dut->piu0_ncq_ar_req = pick();
    fill_wide(dut->piu0_ncq_aw_bus, 3, state, zero);
    dut->piu0_ncq_aw_req = pick();
    dut->piu0_ncq_b_grant = pick();
    dut->piu0_ncq_r_grant = pick();
    fill_wide(dut->piu0_ncq_wcd_bus, 5, state, zero);
    dut->piu0_ncq_wcd_req = pick();
    fill_wide(dut->piu1_ncq_ar_bus, 3, state, zero);
    dut->piu1_ncq_ar_req = pick();
    fill_wide(dut->piu1_ncq_aw_bus, 3, state, zero);
    dut->piu1_ncq_aw_req = pick();
    dut->piu1_ncq_b_grant = pick();
    dut->piu1_ncq_r_grant = pick();
    fill_wide(dut->piu1_ncq_wcd_bus, 5, state, zero);
    dut->piu1_ncq_wcd_req = pick();
    fill_wide(dut->piu2_ncq_ar_bus, 3, state, zero);
    dut->piu2_ncq_ar_req = pick();
    fill_wide(dut->piu2_ncq_aw_bus, 3, state, zero);
    dut->piu2_ncq_aw_req = pick();
    dut->piu2_ncq_b_grant = pick();
    dut->piu2_ncq_r_grant = pick();
    fill_wide(dut->piu2_ncq_wcd_bus, 5, state, zero);
    dut->piu2_ncq_wcd_req = pick();
    fill_wide(dut->piu3_ncq_ar_bus, 3, state, zero);
    dut->piu3_ncq_ar_req = pick();
    fill_wide(dut->piu3_ncq_aw_bus, 3, state, zero);
    dut->piu3_ncq_aw_req = pick();
    dut->piu3_ncq_b_grant = pick();
    dut->piu3_ncq_r_grant = pick();
    fill_wide(dut->piu3_ncq_wcd_bus, 5, state, zero);
    dut->piu3_ncq_wcd_req = pick();
    dut->sysio_ciu_apb_base = pick();
}

static void tick(VerilatedContext *ctx, Vct_ciu_ncq *dut) {
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static bool check_quiescent_outputs(Vct_ciu_ncq *dut) {
    return dut->ncq_ebiu_arvalid == 0 && dut->ncq_ebiu_awvalid == 0 &&
           dut->ncq_ebiu_wvalid == 0 && dut->ncq_apbif_arvalid == 0 &&
           dut->ncq_apbif_awvalid == 0 && dut->ncq_apbif_wvalid == 0;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::randReset(0);

    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_ciu_ncq(ctx);
    uint64_t seed = 0x1234abcd5678ef00ULL;

    dut->forever_cpuclk = 0;
    dut->ciu_icg_en = 1;
    dut->pad_yy_icg_scan_en = 0;

    dut->cpurst_b = 0;
    for (int i = 0; i < 10; ++i) {
        drive_inputs(dut, seed, true);
        tick(ctx, dut);
    }
    dut->cpurst_b = 1;

    for (int i = 0; i < 8; ++i) {
        drive_inputs(dut, seed, true);
        tick(ctx, dut);
        if (!check_quiescent_outputs(dut)) {
            std::cerr << "[case_002] quiescent outputs not zero after reset" << std::endl;
            dut->final();
            delete dut;
            delete ctx;
            return 1;
        }
    }

    for (int i = 0; i < 600; ++i) {
        drive_inputs(dut, seed, false);
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
