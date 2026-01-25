#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "VBUFGCE.h"

namespace {

struct Stim {
    uint32_t i;
    uint32_t ce;
    uint32_t expected_o;
    const char *label;
};

void apply(VBUFGCE *dut, VerilatedContext *context, uint32_t i, uint32_t ce) {
    dut->I = i;
    dut->CE = ce;
    dut->eval();
    context->timeInc(1);
}

bool expect_eq(uint32_t actual, uint32_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected " << expected << " got " << actual
                  << std::endl;
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<VBUFGCE>(context.get());
    dut->I = 0;
    dut->CE = 0;
    dut->eval();

    const Stim kTests[] = {
        {0, 0, 0, "init_low"},
        {0, 1, 0, "latch_enable_low"},
        {1, 1, 1, "gate_on_rise"},
        {1, 0, 1, "hold_gate_when_high"},
        {0, 0, 0, "capture_disable_low"},
        {1, 0, 0, "gate_off_rise"},
        {1, 1, 0, "hold_off_when_high"},
        {0, 1, 0, "capture_enable_low_again"},
        {1, 1, 1, "gate_on_after_latch"},
    };

    for (const auto &test : kTests) {
        apply(dut.get(), context.get(), test.i, test.ce);
        if (!expect_eq(static_cast<uint32_t>(dut->O), test.expected_o, test.label)) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] case_007 passed: BUFGCE latch gate behavior\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
