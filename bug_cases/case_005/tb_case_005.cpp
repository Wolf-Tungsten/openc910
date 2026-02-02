#include "Vsim_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace {

void tick(VerilatedContext *ctx, Vsim_top *dut) {
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::randReset(0);

    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vsim_top(ctx);

    dut->clk = 0;
    dut->rst_n = 0;

    tick(ctx, dut);
    tick(ctx, dut);

    if (dut->count != 0) {
        std::cerr << "[case_005] count expected 0 under reset" << std::endl;
        return 1;
    }
    if (dut->pad_sample != 0) {
        std::cerr << "[case_005] pad_sample expected 0 under reset" << std::endl;
        return 1;
    }

    dut->rst_n = 1;

    tick(ctx, dut);
    if (dut->count != 1) {
        std::cerr << "[case_005] count expected 1 after first tick" << std::endl;
        return 1;
    }
    if (dut->pad_sample != 0xA5) {
        std::cerr << "[case_005] pad_sample expected 0xA5 after first tick" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    if (dut->count != 2) {
        std::cerr << "[case_005] count expected 2 after second tick" << std::endl;
        return 1;
    }
    if (dut->pad_sample != 0x3C) {
        std::cerr << "[case_005] pad_sample expected 0x3C after second tick" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    if (dut->count != 3) {
        std::cerr << "[case_005] count expected 3 after third tick" << std::endl;
        return 1;
    }
    if (dut->pad_sample != 0x00) {
        std::cerr << "[case_005] pad_sample expected 0x00 after third tick" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    if (dut->count != 4) {
        std::cerr << "[case_005] count expected 4 after fourth tick" << std::endl;
        return 1;
    }
    if (dut->pad_sample != 0x5A) {
        std::cerr << "[case_005] pad_sample expected 0x5A after fourth tick" << std::endl;
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
