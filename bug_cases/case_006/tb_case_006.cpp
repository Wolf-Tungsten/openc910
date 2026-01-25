#include <cstdint>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <string>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vct_had_pcfifo.h"

namespace {

constexpr uint64_t kWidth = 40;
constexpr uint64_t kMask40 = (1ULL << kWidth) - 1;
constexpr uint64_t kMask39 = (1ULL << 39) - 1;

void tick(Vct_had_pcfifo *dut, VerilatedContext *context) {
    dut->cpuclk = 0;
    dut->eval();
    context->timeInc(1);
    dut->cpuclk = 1;
    dut->eval();
    context->timeInc(1);
}

bool expect_eq(uint64_t actual, uint64_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

void apply_reset(Vct_had_pcfifo *dut, VerilatedContext *context) {
    dut->ctrl_pcfifo_ren = 0;
    dut->ctrl_pcfifo_wen = 0;
    dut->rtu_had_xx_pcfifo_inst0_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst1_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst2_chgflow = 0;
    dut->cpurst_b = 1;
    dut->eval();
    dut->cpurst_b = 0;
    dut->eval();
    tick(dut, context);
    dut->cpurst_b = 1;
    dut->eval();
    tick(dut, context);
}

uint64_t encode_pc(uint64_t next_pc) {
    return ((next_pc & kMask39) << 1) & kMask40;
}

uint64_t format_output(uint64_t stored, bool mmu_en) {
    stored &= kMask40;
    if (mmu_en && (stored & (1ULL << (kWidth - 1)))) {
        return stored | (~kMask40);
    }
    return stored;
}

void push_model(std::deque<uint64_t> &fifo,
                bool v0,
                bool v1,
                bool v2,
                uint64_t pc0,
                uint64_t pc1,
                uint64_t pc2) {
    const int count = static_cast<int>(v0) + static_cast<int>(v1) + static_cast<int>(v2);
    if (count == 3) {
        fifo.push_back(encode_pc(pc0));
        fifo.push_back(encode_pc(pc1));
        fifo.push_back(encode_pc(pc2));
        return;
    }
    if (count == 2) {
        if (v0 && v1) {
            fifo.push_back(encode_pc(pc0));
            fifo.push_back(encode_pc(pc1));
        } else if (v0 && v2) {
            fifo.push_back(encode_pc(pc0));
            fifo.push_back(encode_pc(pc2));
        } else if (v1 && v2) {
            fifo.push_back(encode_pc(pc1));
            fifo.push_back(encode_pc(pc2));
        }
        return;
    }
    if (count == 1) {
        if (v0) {
            fifo.push_back(encode_pc(pc0));
        } else if (v1) {
            fifo.push_back(encode_pc(pc1));
        } else if (v2) {
            fifo.push_back(encode_pc(pc2));
        }
    }
}

void push_entries(Vct_had_pcfifo *dut,
                  VerilatedContext *context,
                  bool v0,
                  bool v1,
                  bool v2,
                  uint64_t pc0,
                  uint64_t pc1,
                  uint64_t pc2) {
    dut->rtu_had_xx_pcfifo_inst0_chgflow = v0;
    dut->rtu_had_xx_pcfifo_inst1_chgflow = v1;
    dut->rtu_had_xx_pcfifo_inst2_chgflow = v2;
    dut->rtu_had_xx_pcfifo_inst0_next_pc = pc0;
    dut->rtu_had_xx_pcfifo_inst1_next_pc = pc1;
    dut->rtu_had_xx_pcfifo_inst2_next_pc = pc2;
    dut->ctrl_pcfifo_wen = 1;
    tick(dut, context);

    dut->rtu_had_xx_pcfifo_inst0_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst1_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst2_chgflow = 0;
    dut->ctrl_pcfifo_wen = 0;
    tick(dut, context);
}

bool pop_and_check(Vct_had_pcfifo *dut,
                   VerilatedContext *context,
                   std::deque<uint64_t> &fifo,
                   bool mmu_en,
                   const char *label) {
    if (fifo.empty()) {
        std::cerr << "[TB] " << label << " model fifo empty" << std::endl;
        return false;
    }
    const uint64_t stored = fifo.front();
    fifo.pop_front();
    const uint64_t expected = format_output(stored, mmu_en);

    dut->mmu_xx_mmu_en = mmu_en ? 1 : 0;
    dut->ctrl_pcfifo_ren = 1;
    tick(dut, context);
    dut->ctrl_pcfifo_ren = 0;
    dut->eval();

    return expect_eq(dut->pcfifo_regs_data, expected, label);
}

bool read_zero(Vct_had_pcfifo *dut,
               VerilatedContext *context,
               const char *label) {
    dut->ctrl_pcfifo_ren = 1;
    tick(dut, context);
    dut->ctrl_pcfifo_ren = 0;
    dut->eval();
    return expect_eq(dut->pcfifo_regs_data & kMask40, 0, label);
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vct_had_pcfifo>(context.get());
    dut->cpuclk = 0;
    dut->cpurst_b = 0;
    dut->ctrl_pcfifo_ren = 0;
    dut->ctrl_pcfifo_wen = 0;
    dut->mmu_xx_mmu_en = 0;
    dut->rtu_had_xx_pcfifo_inst0_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst1_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst2_chgflow = 0;
    dut->rtu_had_xx_pcfifo_inst0_next_pc = 0;
    dut->rtu_had_xx_pcfifo_inst1_next_pc = 0;
    dut->rtu_had_xx_pcfifo_inst2_next_pc = 0;

    apply_reset(dut.get(), context.get());

    if (!expect_eq(dut->pcfifo_regs_data, 0, "reset_out")) {
        return EXIT_FAILURE;
    }

    std::deque<uint64_t> fifo;

    push_entries(dut.get(), context.get(), true, false, false, 0x123456789ULL, 0, 0);
    push_model(fifo, true, false, false, 0x123456789ULL, 0, 0);
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "single_write")) {
        return EXIT_FAILURE;
    }

    push_entries(dut.get(), context.get(), false, true, true, 0, 0x111ULL, 0x222ULL);
    push_model(fifo, false, true, true, 0, 0x111ULL, 0x222ULL);
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "two_write_1")) {
        return EXIT_FAILURE;
    }
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "two_write_2")) {
        return EXIT_FAILURE;
    }

    push_entries(dut.get(), context.get(), true, false, true, 0x333ULL, 0, 0x444ULL);
    push_model(fifo, true, false, true, 0x333ULL, 0, 0x444ULL);
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "two_write_mix1")) {
        return EXIT_FAILURE;
    }
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "two_write_mix2")) {
        return EXIT_FAILURE;
    }

    push_entries(dut.get(), context.get(), false, true, false, 0, 0x555ULL, 0);
    push_model(fifo, false, true, false, 0, 0x555ULL, 0);
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "single_inst1")) {
        return EXIT_FAILURE;
    }

    const uint64_t pc_sign = (1ULL << 38) | 0x1234ULL;
    push_entries(dut.get(), context.get(), false, false, true, 0, 0, pc_sign);
    push_model(fifo, false, false, true, 0, 0, pc_sign);
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "sign_zeroext")) {
        return EXIT_FAILURE;
    }

    push_entries(dut.get(),
                 context.get(),
                 true,
                 true,
                 true,
                 0x100ULL,
                 (1ULL << 38) | 0x200ULL,
                 0x300ULL);
    push_model(fifo,
               true,
               true,
               true,
               0x100ULL,
               (1ULL << 38) | 0x200ULL,
               0x300ULL);
    if (!pop_and_check(dut.get(), context.get(), fifo, false, "three_write_0")) {
        return EXIT_FAILURE;
    }
    if (!pop_and_check(dut.get(), context.get(), fifo, true, "three_write_1_signext")) {
        return EXIT_FAILURE;
    }
    if (!pop_and_check(dut.get(), context.get(), fifo, true, "three_write_2")) {
        return EXIT_FAILURE;
    }

    fifo.clear();
    apply_reset(dut.get(), context.get());
    for (int i = 0; i < 16; ++i) {
        const uint64_t pc = 0x1000ULL + static_cast<uint64_t>(i) * 0x111ULL;
        push_entries(dut.get(), context.get(), true, false, false, pc, 0, 0);
        push_model(fifo, true, false, false, pc, 0, 0);
    }
    for (int i = 0; i < 16; ++i) {
        const std::string label = "pre_reset_fill_" + std::to_string(i);
        if (!pop_and_check(dut.get(), context.get(), fifo, false, label.c_str())) {
            return EXIT_FAILURE;
        }
    }

    apply_reset(dut.get(), context.get());
    dut->mmu_xx_mmu_en = 0;
    for (int i = 0; i < 16; ++i) {
        const std::string label = "reset_full_clear_" + std::to_string(i);
        if (!read_zero(dut.get(), context.get(), label.c_str())) {
            return EXIT_FAILURE;
        }
    }

    std::cout << "[TB] case_006 passed: pcfifo multi-pattern readback + reset clear\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
