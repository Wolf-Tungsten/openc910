#include "Vct_fifo.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vct_fifo *dut) {
    dut->clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static bool expect_equal(const char *label, uint64_t got, uint64_t expected) {
    if (got != expected) {
        std::cerr << "[TB] " << label << " mismatch: got=0x" << std::hex << got
                  << " expected=0x" << expected << std::dec << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_fifo(ctx);

    // Default inputs
    dut->clk = 0;
    dut->rst_b = 0;
    dut->fifo_create_en = 0;
    dut->fifo_create_en_dp = 0;
    dut->fifo_pop_en = 0;
    dut->fifo_create_data = 0;
    dut->pad_yy_icg_scan_en = 0;
    dut->fifo_icg_en = 1;

    // Reset for a couple cycles
    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
    }

    // Release reset
    dut->rst_b = 1;
    tick(ctx, dut);

    if (!dut->fifo_empty) {
        std::cerr << "[TB] fifo_empty should be 1 after reset" << std::endl;
        return 1;
    }

    // Create first entry
    const uint8_t data1 = 0x20; // 6'b100000
    dut->fifo_create_data = data1;
    dut->fifo_create_en = 1;
    dut->fifo_create_en_dp = 1;
    tick(ctx, dut);

    dut->fifo_create_en = 0;
    dut->fifo_create_en_dp = 0;

    // Hold for several cycles; pop_data should remain stable
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
        if (!dut->fifo_pop_data_vld) {
            std::cerr << "[TB] fifo_pop_data_vld should stay high after create" << std::endl;
            return 1;
        }
        if (!expect_equal("fifo_pop_data", dut->fifo_pop_data, data1)) {
            return 1;
        }
        if (dut->fifo_empty) {
            std::cerr << "[TB] fifo_empty should be 0 after create" << std::endl;
            return 1;
        }
    }

    // Create second entry to fill FIFO
    const uint8_t data2 = 0x15; // 6'b010101
    dut->fifo_create_data = data2;
    dut->fifo_create_en = 1;
    dut->fifo_create_en_dp = 1;
    tick(ctx, dut);

    dut->fifo_create_en = 0;
    dut->fifo_create_en_dp = 0;
    tick(ctx, dut);

    if (!dut->fifo_full) {
        std::cerr << "[TB] fifo_full should be 1 after two creates" << std::endl;
        return 1;
    }

    // Pop one entry
    dut->fifo_pop_en = 1;
    tick(ctx, dut);
    dut->fifo_pop_en = 0;
    tick(ctx, dut);

    if (!expect_equal("fifo_pop_data (after pop)", dut->fifo_pop_data, data2)) {
        return 1;
    }

    // Pop again to empty
    dut->fifo_pop_en = 1;
    tick(ctx, dut);
    dut->fifo_pop_en = 0;
    tick(ctx, dut);

    if (!dut->fifo_empty) {
        std::cerr << "[TB] fifo_empty should be 1 after popping all entries" << std::endl;
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
