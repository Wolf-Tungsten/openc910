#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vsync_level2level.h"

namespace {

constexpr int kFlopNum = 3;
constexpr uint32_t kMask = 0x1u;

void tick(Vsync_level2level *dut, VerilatedContext *context) {
    dut->clk = 0;
    dut->eval();
    context->timeInc(1);
    dut->clk = 1;
    dut->eval();
    context->timeInc(1);
}

bool expect_eq(uint32_t actual, uint32_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

void shift_history(uint32_t *history, uint32_t next) {
    for (int i = kFlopNum - 1; i > 0; --i) {
        history[i] = history[i - 1];
    }
    history[0] = next & kMask;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vsync_level2level>(context.get());
    dut->clk = 0;
    dut->rst_b = 0;
    dut->sync_in = 0;
    dut->eval();

    uint32_t history[kFlopNum] = {0};
    if (!expect_eq(dut->sync_out, 0, "reset_init")) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 3; ++i) {
        dut->sync_in = static_cast<uint32_t>(i) & kMask;
        tick(dut.get(), context.get());
        if (!expect_eq(dut->sync_out, 0, "reset_hold")) {
            return EXIT_FAILURE;
        }
    }

    dut->rst_b = 1;
    dut->eval();

    const uint32_t patterns[] = {0x0, 0x1, 0x1, 0x0, 0x1, 0x0, 0x1, 0x1};
    for (std::size_t i = 0; i < sizeof(patterns) / sizeof(patterns[0]); ++i) {
        const uint32_t next = patterns[i] & kMask;
        dut->sync_in = next;
        dut->eval();
        if (!expect_eq(dut->sync_out, history[kFlopNum - 1], "hold_between_edges")) {
            return EXIT_FAILURE;
        }
        tick(dut.get(), context.get());
        shift_history(history, next);
        if (!expect_eq(dut->sync_out, history[kFlopNum - 1], "sync_out_track")) {
            return EXIT_FAILURE;
        }
    }

    tick(dut.get(), context.get());
    shift_history(history, dut->sync_in);
    if (!expect_eq(dut->sync_out, history[kFlopNum - 1], "pre_async_shift")) {
        return EXIT_FAILURE;
    }

    dut->rst_b = 0;
    dut->eval();
    if (!expect_eq(dut->sync_out, 0, "async_reset")) {
        return EXIT_FAILURE;
    }
    for (int i = 0; i < kFlopNum; ++i) {
        history[i] = 0;
    }

    dut->sync_in = 0x1;
    tick(dut.get(), context.get());
    if (!expect_eq(dut->sync_out, 0, "reset_tick_hold")) {
        return EXIT_FAILURE;
    }

    dut->rst_b = 1;
    dut->eval();
    dut->sync_in = 0x1;
    dut->eval();
    if (!expect_eq(dut->sync_out, 0, "post_async_hold")) {
        return EXIT_FAILURE;
    }
    tick(dut.get(), context.get());
    shift_history(history, dut->sync_in);
    if (!expect_eq(dut->sync_out, history[kFlopNum - 1], "post_async_reset")) {
        return EXIT_FAILURE;
    }

    uint32_t lfsr = 1;
    for (int i = 0; i < 16; ++i) {
        lfsr = (lfsr * 3u + 1u) & kMask;
        dut->sync_in = lfsr;
        dut->eval();
        if (!expect_eq(dut->sync_out, history[kFlopNum - 1], "lfsr_hold")) {
            return EXIT_FAILURE;
        }
        tick(dut.get(), context.get());
        shift_history(history, lfsr);
        if (!expect_eq(dut->sync_out, history[kFlopNum - 1], "lfsr_track")) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] case_004 passed: sync_out tracks sync_in through 3 flops\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
