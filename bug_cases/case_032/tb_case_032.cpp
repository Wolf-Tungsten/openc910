#include "Vfpga_ram.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vfpga_ram *dut) {
    dut->PortAClk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->PortAClk = 1;
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

    auto *dut = new Vfpga_ram(ctx);

    // Default inputs
    dut->PortAClk = 0;
    dut->PortAAddr = 0;
    dut->PortADataIn = 0;
    dut->PortAWriteEnable = 0;

    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
    }

    // Write known data into each address.
    dut->PortAWriteEnable = 1;

    dut->PortAAddr = 0;
    dut->PortADataIn = 1;
    tick(ctx, dut);
    if (!expect_equal("write_through addr0", dut->PortADataOut, 1)) {
        return 1;
    }

    dut->PortAAddr = 1;
    dut->PortADataIn = 2;
    tick(ctx, dut);
    if (!expect_equal("write_through addr1", dut->PortADataOut, 2)) {
        return 1;
    }

    dut->PortAAddr = 2;
    dut->PortADataIn = 3;
    tick(ctx, dut);
    if (!expect_equal("write_through addr2", dut->PortADataOut, 3)) {
        return 1;
    }

    dut->PortAAddr = 3;
    dut->PortADataIn = 0;
    tick(ctx, dut);
    if (!expect_equal("write_through addr3", dut->PortADataOut, 0)) {
        return 1;
    }

    // Read values back; output should reflect mem[addr] on the same edge.
    dut->PortAWriteEnable = 0;

    dut->PortAAddr = 0;
    tick(ctx, dut);
    if (!expect_equal("read addr0", dut->PortADataOut, 1)) {
        return 1;
    }

    dut->PortAAddr = 1;
    tick(ctx, dut);
    if (!expect_equal("read addr1", dut->PortADataOut, 2)) {
        return 1;
    }

    dut->PortAAddr = 2;
    tick(ctx, dut);
    if (!expect_equal("read addr2", dut->PortADataOut, 3)) {
        return 1;
    }

    dut->PortAAddr = 3;
    tick(ctx, dut);
    if (!expect_equal("read addr3", dut->PortADataOut, 0)) {
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
