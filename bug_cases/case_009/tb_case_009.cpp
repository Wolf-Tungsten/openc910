#include "Vcase_009_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vcase_009_top *dut) {
    dut->plic_clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->plic_clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void init_inputs(Vcase_009_top *dut) {
    dut->plic_clk = 0;
    dut->plicrst_b = 0;
}

static bool outputs_are_zero(Vcase_009_top *dut) {
    if (dut->arbx_hartx_sint_req != 0) {
        return false;
    }
    if (dut->arbx_hartx_mint_req != 0) {
        return false;
    }
    if (dut->arbx_hreg_claim_mmode != 0) {
        return false;
    }
    if (dut->arbx_hreg_claim_reg_ready != 0) {
        return false;
    }
    if (dut->arbx_hreg_arb_start_ack != 0) {
        return false;
    }
    if (dut->arbx_hreg_claim_id != 0) {
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vcase_009_top(ctx);
    init_inputs(dut);

    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    dut->plicrst_b = 1;
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    if (!outputs_are_zero(dut)) {
        std::cerr << "[TB] unexpected non-zero outputs" << std::endl;
        return 1;
    }

    dut->plicrst_b = 0;
    tick(ctx, dut);
    dut->plicrst_b = 1;
    tick(ctx, dut);

    if (!outputs_are_zero(dut)) {
        std::cerr << "[TB] outputs changed after reset toggle" << std::endl;
        return 1;
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
