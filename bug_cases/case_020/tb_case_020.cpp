#include "Vinout_vec_oe.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vinout_vec_oe *dut) {
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

    auto *dut = new Vinout_vec_oe(ctx);

    dut->reset = 1;
    dut->drive_data = 0;
    dut->drive_oe = 0;
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }
    dut->reset = 0;

    struct Pattern {
        uint8_t data;
        uint8_t oe;
    };
    const Pattern patterns[] = {
        {0xA5, 0xFF},
        {0x3C, 0x0F},
        {0xF0, 0xF0},
        {0x5A, 0xAA},
    };

    for (const auto &pat : patterns) {
        dut->drive_data = pat.data;
        dut->drive_oe = pat.oe;
        tick(ctx, dut);

        const uint8_t sampled = static_cast<uint8_t>(dut->sampled);
        if ((sampled & pat.oe) != (pat.data & pat.oe)) {
            std::cerr << "[TB] sampled mismatch: got=0x"
                      << std::hex << static_cast<int>(sampled)
                      << " expect=0x" << static_cast<int>(pat.data)
                      << " oe=0x" << static_cast<int>(pat.oe) << std::dec << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < 10; ++i) {
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
