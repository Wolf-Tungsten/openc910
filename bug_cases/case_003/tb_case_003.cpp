#include "Vct_iu_div_entry.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace {

constexpr int kWords = 9;

uint64_t next_rand(uint64_t &state) {
    if (state == 0) {
        state = 0x9e3779b97f4a7c15ULL;
    }
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

std::array<vluint32_t, kWords> make_pattern(uint64_t &state) {
    std::array<vluint32_t, kWords> data{};
    for (int i = 0; i < kWords; ++i) {
        data[i] = static_cast<vluint32_t>(next_rand(state));
    }
    return data;
}

void drive_wide(vluint32_t *bus, const std::array<vluint32_t, kWords> &data) {
    for (int i = 0; i < kWords; ++i) {
        bus[i] = data[i];
    }
}

bool wide_equals(const vluint32_t *bus, const std::array<vluint32_t, kWords> &data) {
    for (int i = 0; i < kWords; ++i) {
        if (bus[i] != data[i]) {
            return false;
        }
    }
    return true;
}

void tick(VerilatedContext *ctx, Vct_iu_div_entry *dut) {
    dut->div_clk = 0;
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->div_clk = 1;
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::randReset(0);

    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_iu_div_entry(ctx);

    const std::array<vluint32_t, kWords> reset_entry0 = {
        0xffffffffu, 0xffffffffu, 0x00000001u, 0x00000000u,
        0xffffffffu, 0xffffffffu, 0x00000000u, 0x00000000u,
        0x00000000u
    };
    const std::array<vluint32_t, kWords> reset_entry1 = {
        0xffffffffu, 0x7fffffffu, 0x00000001u, 0x00000000u,
        0xffffffffu, 0x7fffffffu, 0x00000000u, 0x00000000u,
        0x00000001u
    };

    dut->cp0_yy_clk_en = 1;
    dut->cp0_iu_icg_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->cp0_iu_div_entry_disable_clr = 0;
    dut->div_entry0_read_vld = 0;
    dut->div_entry1_read_vld = 0;
    dut->div_entry_write_en = 0;

    uint64_t seed = 0x12345678abcdef01ULL;
    auto pattern = make_pattern(seed);
    drive_wide(dut->div_entry_write_data, pattern);

    dut->cpurst_b = 0;
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    dut->cpurst_b = 1;
    tick(ctx, dut);

    if (!wide_equals(dut->div_entry0_read_data, reset_entry0)) {
        std::cerr << "[case_003] div_entry0 reset mismatch" << std::endl;
        return 1;
    }
    if (!wide_equals(dut->div_entry1_read_data, reset_entry1)) {
        std::cerr << "[case_003] div_entry1 reset mismatch" << std::endl;
        return 1;
    }

    auto write_a = make_pattern(seed);
    drive_wide(dut->div_entry_write_data, write_a);
    dut->div_entry_write_en = 1;
    tick(ctx, dut);
    dut->div_entry_write_en = 0;
    tick(ctx, dut);

    if (!wide_equals(dut->div_entry1_read_data, write_a)) {
        std::cerr << "[case_003] entry1 write mismatch" << std::endl;
        return 1;
    }

    dut->div_entry1_read_vld = 1;
    tick(ctx, dut);
    dut->div_entry1_read_vld = 0;

    auto write_b = make_pattern(seed);
    drive_wide(dut->div_entry_write_data, write_b);
    dut->div_entry_write_en = 1;
    tick(ctx, dut);
    dut->div_entry_write_en = 0;
    tick(ctx, dut);

    if (!wide_equals(dut->div_entry0_read_data, write_b)) {
        std::cerr << "[case_003] entry0 write mismatch" << std::endl;
        return 1;
    }

    dut->div_entry0_read_vld = 1;
    tick(ctx, dut);
    dut->div_entry0_read_vld = 0;

    dut->cp0_iu_div_entry_disable_clr = 1;
    tick(ctx, dut);
    dut->cp0_iu_div_entry_disable_clr = 0;
    tick(ctx, dut);

    if (!wide_equals(dut->div_entry0_read_data, reset_entry0)) {
        std::cerr << "[case_003] div_entry0 disable_clr mismatch" << std::endl;
        return 1;
    }
    if (!wide_equals(dut->div_entry1_read_data, reset_entry1)) {
        std::cerr << "[case_003] div_entry1 disable_clr mismatch" << std::endl;
        return 1;
    }

    for (int i = 0; i < 24; ++i) {
        pattern = make_pattern(seed);
        drive_wide(dut->div_entry_write_data, pattern);
        dut->div_entry_write_en = (i & 1) != 0;
        dut->div_entry0_read_vld = (i % 3) == 0;
        dut->div_entry1_read_vld = (i % 5) == 0;
        tick(ctx, dut);
    }

    dut->div_entry_write_en = 0;
    dut->div_entry0_read_vld = 0;
    dut->div_entry1_read_vld = 0;
    tick(ctx, dut);

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
