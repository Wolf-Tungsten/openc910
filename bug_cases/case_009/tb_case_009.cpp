#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vcpu_sub_system_axi.h"

namespace {

bool expect_eq(uint32_t actual, uint32_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vcpu_sub_system_axi>(context.get());
    dut->xx_intc_vld = 0;
    dut->eval();

    if (!expect_eq(dut->pad_plic_int_vld[0], 0u, "reset_word0") ||
        !expect_eq(dut->pad_plic_int_vld[1], 0u, "reset_word1") ||
        !expect_eq(dut->pad_plic_int_vld[2], 0u, "reset_word2") ||
        !expect_eq(dut->pad_plic_int_vld[3], 0u, "reset_word3") ||
        !expect_eq(dut->pad_plic_int_vld[4] & 0xFFFFu, 0u, "reset_word4")) {
        return EXIT_FAILURE;
    }

    constexpr uint32_t kPattern = 0x5a5aa5a5u;
    dut->xx_intc_vld = static_cast<uint64_t>(kPattern);
    dut->eval();

    if (!expect_eq(dut->pad_plic_int_vld[0], kPattern, "pattern_word0") ||
        !expect_eq(dut->pad_plic_int_vld[1], 0u, "pattern_word1") ||
        !expect_eq(dut->pad_plic_int_vld[2], 0u, "pattern_word2") ||
        !expect_eq(dut->pad_plic_int_vld[3], 0u, "pattern_word3") ||
        !expect_eq(dut->pad_plic_int_vld[4] & 0xFFFFu, 0u, "pattern_word4")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_009 passed: pad_plic_int_vld slice wiring" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
