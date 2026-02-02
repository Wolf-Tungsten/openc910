#include "Vc910_inst_reg_mem_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vc910_inst_reg_mem_top *dut) {
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vc910_inst_reg_mem_top(ctx);

    dut->clk = 0;
    dut->rst_b = 0;
    dut->en = 0;
    dut->data_in = 0;

    for (int i = 0; i < 3; ++i) {
        tick(ctx, dut);
    }

    dut->rst_b = 1;
    tick(ctx, dut);
    if (dut->data_out0 != 0) {
        std::cerr << "[TB] expected reset output 0, got " << std::hex
                  << dut->data_out0 << std::dec << std::endl;
        return 1;
    }

    dut->en = 1;
    dut->data_in = 0xA5A5A5A5u;
    tick(ctx, dut);
    if (dut->data_out0 != 0xA5A5A5A5u) {
        std::cerr << "[TB] data_out0 mismatch after load" << std::endl;
        return 1;
    }

    dut->en = 0;
    dut->data_in = 0x5A5A5A5Au;
    tick(ctx, dut);
    if (dut->data_out0 != 0xA5A5A5A5u) {
        std::cerr << "[TB] data_out0 should hold when en=0" << std::endl;
        return 1;
    }

    for (int i = 0; i < 5; ++i) {
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
