#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vct_prio.h"

namespace {

constexpr uint32_t kSel0 = 0x1u;
constexpr uint32_t kSel1 = 0x2u;

void tick(Vct_prio *dut, VerilatedContext *context) {
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

void apply_reset(Vct_prio *dut, VerilatedContext *context) {
    dut->valid = 0;
    dut->clr = 0;
    dut->rst_b = 1;
    dut->eval();
    dut->rst_b = 0;
    dut->eval();
    tick(dut, context);
    dut->rst_b = 1;
    dut->eval();
    tick(dut, context);
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vct_prio>(context.get());
    dut->clk = 0;
    dut->rst_b = 0;
    dut->valid = 0;
    dut->clr = 0;
    dut->eval();

    apply_reset(dut.get(), context.get());

    dut->valid = 0;
    dut->clr = 0;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), 0, "reset_sel_zero")) {
        return EXIT_FAILURE;
    }

    dut->valid = 0x3u;
    dut->clr = 0;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel0, "reset_priority_sel0")) {
        return EXIT_FAILURE;
    }

    dut->clr = 1;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel0, "clr_assert_sel0")) {
        return EXIT_FAILURE;
    }
    tick(dut.get(), context.get());

    dut->clr = 0;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel1, "post_clr_sel1")) {
        return EXIT_FAILURE;
    }

    dut->clr = 1;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel1, "clr_assert_sel1")) {
        return EXIT_FAILURE;
    }
    tick(dut.get(), context.get());

    dut->clr = 0;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel0, "post_clr_sel0")) {
        return EXIT_FAILURE;
    }

    dut->valid = 0x2u;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel1, "single_valid1")) {
        return EXIT_FAILURE;
    }

    dut->valid = 0x1u;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), kSel0, "single_valid0")) {
        return EXIT_FAILURE;
    }

    dut->valid = 0;
    dut->eval();
    if (!expect_eq(static_cast<uint32_t>(dut->sel), 0, "no_valid")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_008 passed: ct_prio priority rotation\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
