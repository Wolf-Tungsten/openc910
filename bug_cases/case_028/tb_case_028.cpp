#include "Vf_spsram_large.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void set128(WData *dst, const uint32_t words[4]) {
    dst[0] = words[0];
    dst[1] = words[1];
    dst[2] = words[2];
    dst[3] = words[3];
}

static bool eq128(const WData *lhs, const uint32_t words[4]) {
    return lhs[0] == words[0] && lhs[1] == words[1] && lhs[2] == words[2] && lhs[3] == words[3];
}

static void dump128(const WData *val) {
    std::cerr << "0x" << std::hex << val[3] << "_" << val[2] << "_" << val[1] << "_" << val[0]
              << std::dec;
}

static void tick(VerilatedContext *ctx, Vf_spsram_large *dut) {
    dut->CLK = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->CLK = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void init_write(VerilatedContext *ctx, Vf_spsram_large *dut, uint32_t addr,
                       const uint32_t words[4]) {
    dut->tb_init_en = 1;
    dut->tb_init_addr = addr;
    set128(dut->tb_init_wdata, words);
    dut->tb_init_wen = 0x0000;
    tick(ctx, dut);
}

static bool read_check(VerilatedContext *ctx, Vf_spsram_large *dut, uint32_t addr,
                       const uint32_t words[4], const char *label) {
    dut->CEN = 0;
    dut->WEN = 0xFFFF;
    dut->A = addr;
    tick(ctx, dut);
    if (!eq128(dut->Q, words)) {
        std::cerr << "[TB] " << label << " read mismatch at addr 0x" << std::hex << addr
                  << std::dec << std::endl;
        return false;
    }
    return true;
}

static bool hold_check(VerilatedContext *ctx, Vf_spsram_large *dut, uint32_t toggle_addr0,
                       uint32_t toggle_addr1, const uint32_t expected[4], int cycles) {
    dut->CEN = 1;
    dut->WEN = 0xFFFF;
    dut->A = toggle_addr0;
    tick(ctx, dut);
    for (int i = 0; i < cycles; ++i) {
        dut->A = (i & 1) ? toggle_addr1 : toggle_addr0;
        tick(ctx, dut);
        if (!eq128(dut->Q, expected)) {
            std::cerr << "[TB] hold broken while CEN=1, cycle " << i
                      << " expected=";
            dump128(expected);
            std::cerr << " got=";
            dump128(dut->Q);
            std::cerr << std::endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vf_spsram_large(ctx);

    dut->CLK = 0;
    dut->CEN = 1;
    dut->A = 0;
    dut->WEN = 0xFFFF;
    const uint32_t zeros[4] = {0, 0, 0, 0};
    set128(dut->D, zeros);
    dut->tb_init_en = 0;
    dut->tb_init_addr = 0;
    set128(dut->tb_init_wdata, zeros);
    dut->tb_init_wen = 0xFFFF;

    const uint32_t pattern0[4] = {0x03020100u, 0x07060504u, 0x0B0A0908u, 0x0F0E0D0Cu};
    const uint32_t pattern1[4] = {0x13121110u, 0x17161514u, 0x1B1A1918u, 0x1F1E1D1Cu};
    const uint32_t pattern2[4] = {0x23222120u, 0x27262524u, 0x2B2A2928u, 0x2F2E2D2Cu};
    const uint32_t pattern3[4] = {0x33323130u, 0x37363534u, 0x3B3A3938u, 0x3F3E3D3Cu};
    const uint32_t pattern_hold[4] = {0xA1A0A3A2u, 0xA5A4A7A6u, 0xA9A8ABAAu, 0xADACAFEAu};

    init_write(ctx, dut, 0, pattern0);
    init_write(ctx, dut, 1, pattern1);
    init_write(ctx, dut, 2, pattern2);

    dut->tb_init_en = 0;
    dut->tb_init_wen = 0xFFFF;

    if (!read_check(ctx, dut, 0, pattern0, "init")) {
        return 1;
    }
    if (!read_check(ctx, dut, 1, pattern1, "init")) {
        return 1;
    }
    if (!read_check(ctx, dut, 2, pattern2, "init")) {
        return 1;
    }

    dut->CEN = 0;
    dut->WEN = 0x0000;
    dut->A = 3;
    set128(dut->D, pattern3);
    tick(ctx, dut);

    dut->WEN = 0xFFFF;
    if (!read_check(ctx, dut, 3, pattern3, "write")) {
        return 1;
    }

    dut->CEN = 1;
    dut->A = 4;
    tick(ctx, dut);
    if (!eq128(dut->Q, pattern3)) {
        std::cerr << "[TB] hold read mismatch" << std::endl;
        return 1;
    }

    dut->CEN = 0;
    dut->WEN = 0x0000;
    dut->A = 0x7FFFu;
    set128(dut->D, pattern_hold);
    tick(ctx, dut);

    if (!read_check(ctx, dut, 0x7FFFu, pattern_hold, "hold-seed")) {
        return 1;
    }

    // Ensure addr_holding does not update while CEN=1.
    if (!hold_check(ctx, dut, 0, 1, pattern_hold, 4)) {
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
