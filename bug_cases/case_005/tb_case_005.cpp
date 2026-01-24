#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vct_mmu_dplru.h"

namespace {

constexpr uint16_t kAllValid = 0xFFFFu;
constexpr uint16_t kRefillEntry0 = 0x0001u;
constexpr uint16_t kRefillEntry1 = 0x0002u;

void set_entry_vld(Vct_mmu_dplru *dut, uint16_t mask);

void tick(Vct_mmu_dplru *dut, VerilatedContext *context) {
    dut->forever_cpuclk = 0;
    dut->eval();
    context->timeInc(1);
    dut->forever_cpuclk = 1;
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

uint16_t expected_refill_onehot(uint16_t vld_mask, uint16_t plru_onehot) {
    if (vld_mask == kAllValid) {
        return plru_onehot;
    }
    for (int i = 0; i < 16; ++i) {
        if (((vld_mask >> i) & 0x1u) == 0) {
            return static_cast<uint16_t>(1u << i);
        }
    }
    return plru_onehot;
}

bool pulse_refill_and_check(Vct_mmu_dplru *dut,
                            VerilatedContext *context,
                            uint16_t vld_mask,
                            uint16_t expected,
                            const char *label) {
    set_entry_vld(dut, vld_mask);
    dut->utlb_plru_refill_on = 1;
    tick(dut, context);
    dut->utlb_plru_refill_on = 0;
    dut->eval();
    if (!expect_eq(dut->plru_dutlb_ref_num, expected, label)) {
        std::cerr << "[TB] vld_mask=0x" << std::hex << vld_mask << std::dec
                  << std::endl;
        return false;
    }
    return true;
}

bool sweep_refill_patterns(Vct_mmu_dplru *dut,
                           VerilatedContext *context,
                           uint16_t plru_onehot) {
    for (uint32_t vld = 0; vld <= kAllValid; ++vld) {
        const uint16_t vld_mask = static_cast<uint16_t>(vld);
        const uint16_t expected = expected_refill_onehot(vld_mask, plru_onehot);
        if (!pulse_refill_and_check(dut, context, vld_mask, expected, "sweep")) {
            return false;
        }
    }
    return true;
}

void set_entry_vld(Vct_mmu_dplru *dut, uint16_t mask) {
    dut->entry0_vld = (mask >> 0) & 0x1u;
    dut->entry1_vld = (mask >> 1) & 0x1u;
    dut->entry2_vld = (mask >> 2) & 0x1u;
    dut->entry3_vld = (mask >> 3) & 0x1u;
    dut->entry4_vld = (mask >> 4) & 0x1u;
    dut->entry5_vld = (mask >> 5) & 0x1u;
    dut->entry6_vld = (mask >> 6) & 0x1u;
    dut->entry7_vld = (mask >> 7) & 0x1u;
    dut->entry8_vld = (mask >> 8) & 0x1u;
    dut->entry9_vld = (mask >> 9) & 0x1u;
    dut->entry10_vld = (mask >> 10) & 0x1u;
    dut->entry11_vld = (mask >> 11) & 0x1u;
    dut->entry12_vld = (mask >> 12) & 0x1u;
    dut->entry13_vld = (mask >> 13) & 0x1u;
    dut->entry14_vld = (mask >> 14) & 0x1u;
    dut->entry15_vld = (mask >> 15) & 0x1u;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vct_mmu_dplru>(context.get());
    dut->forever_cpuclk = 0;
    dut->cp0_mmu_icg_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->utlb_plru_refill_on = 0;
    dut->utlb_plru_refill_vld = 0;
    dut->utlb_plru_read_hit_vld0 = 0;
    dut->utlb_plru_read_hit_vld1 = 0;
    dut->utlb_plru_read_hit0 = 0;
    dut->utlb_plru_read_hit1 = 0;
    set_entry_vld(dut.get(), 0);

    dut->cpurst_b = 1;
    dut->eval();
    dut->cpurst_b = 0;
    dut->eval();
    tick(dut.get(), context.get());

    if (!expect_eq(dut->plru_dutlb_ref_num, kRefillEntry0, "reset_refill0")) {
        return EXIT_FAILURE;
    }
    const uint16_t plru_onehot = static_cast<uint16_t>(dut->plru_dutlb_ref_num);

    dut->cpurst_b = 1;
    dut->eval();
    tick(dut.get(), context.get());

    const uint16_t entry1_invalid = static_cast<uint16_t>(kAllValid & ~(1u << 1));
    set_entry_vld(dut.get(), entry1_invalid);
    if (!pulse_refill_and_check(dut.get(),
                                context.get(),
                                entry1_invalid,
                                kRefillEntry1,
                                "refill_entry1")) {
        return EXIT_FAILURE;
    }

    if (!pulse_refill_and_check(dut.get(),
                                context.get(),
                                kAllValid,
                                plru_onehot,
                                "refill_plru0")) {
        return EXIT_FAILURE;
    }

    set_entry_vld(dut.get(), 0);
    tick(dut.get(), context.get());
    if (!expect_eq(dut->plru_dutlb_ref_num, kRefillEntry0, "hold_no_refill")) {
        return EXIT_FAILURE;
    }

    if (!sweep_refill_patterns(dut.get(), context.get(), plru_onehot)) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_005 passed: refill selection outputs expected onehot\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
