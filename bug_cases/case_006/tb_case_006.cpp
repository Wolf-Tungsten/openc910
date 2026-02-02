#include "Vaxi2ahb.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace {

void tick(VerilatedContext *ctx, Vaxi2ahb *dut) {
    dut->pll_core_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->pll_core_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

void drive_wide128(vluint32_t *bus, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3) {
    bus[0] = w0;
    bus[1] = w1;
    bus[2] = w2;
    bus[3] = w3;
}

uint32_t step_lfsr(uint32_t &lfsr) {
    const uint32_t lsb = lfsr & 1u;
    lfsr >>= 1;
    if (lsb) {
        lfsr ^= 0xD0000001u;
    }
    return lfsr;
}

void init_inputs(Vaxi2ahb *dut) {
    dut->biu_pad_araddr = 0;
    dut->biu_pad_arburst = 0;
    dut->biu_pad_arcache = 0;
    dut->biu_pad_arid = 0;
    dut->biu_pad_arlen = 0;
    dut->biu_pad_arlock = 0;
    dut->biu_pad_arprot = 0;
    dut->biu_pad_arsize = 0;
    dut->biu_pad_artrust = 0;
    dut->biu_pad_arvalid = 0;
    dut->biu_pad_awaddr = 0;
    dut->biu_pad_awburst = 0;
    dut->biu_pad_awcache = 0;
    dut->biu_pad_awid = 0;
    dut->biu_pad_awlen = 0;
    dut->biu_pad_awlock = 0;
    dut->biu_pad_awprot = 0;
    dut->biu_pad_awsize = 0;
    dut->biu_pad_awvalid = 0;
    dut->biu_pad_bready = 0;
    dut->biu_pad_rready = 0;
    dut->biu_pad_wid = 0;
    dut->biu_pad_wlast = 0;
    dut->biu_pad_wstrb = 0;
    dut->biu_pad_wvalid = 0;
    dut->pad_biu_hgrant = 1;
    dut->pad_biu_hready = 1;
    dut->pad_biu_hresp = 0;
    drive_wide128(dut->biu_pad_wdata, 0u, 0u, 0u, 0u);
    drive_wide128(dut->pad_biu_hrdata, 0u, 0u, 0u, 0u);
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::randReset(0);

    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vaxi2ahb(ctx);

    dut->pll_core_cpuclk = 0;
    dut->pad_cpu_rst_b = 0;
    init_inputs(dut);

    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
    }

    dut->pad_cpu_rst_b = 1;

    // Basic write handshake check in idle state.
    dut->biu_pad_awvalid = 1;
    dut->biu_pad_awaddr = 0x100;
    dut->biu_pad_awlen = 0;
    dut->biu_pad_awburst = 1;
    dut->biu_pad_awsize = 3;
    dut->biu_pad_awcache = 0xF;
    dut->biu_pad_awid = 0x2A;
    dut->biu_pad_awprot = 0x3;
    dut->biu_pad_wvalid = 1;
    dut->biu_pad_wlast = 1;
    dut->biu_pad_wstrb = 0xFFFF;
    dut->biu_pad_wid = 0x2A;
    dut->biu_pad_bready = 1;
    drive_wide128(dut->biu_pad_wdata, 0x11112222u, 0x33334444u, 0x55556666u, 0x77778888u);

    dut->eval();
    if (dut->pad_biu_awready != 1 || dut->pad_biu_arready != 0) {
        std::cerr << "[case_006] idle AW ready check failed" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    if (dut->pad_biu_wready != 1) {
        std::cerr << "[case_006] write ready not asserted in RD_AXI" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    dut->biu_pad_awvalid = 0;
    dut->biu_pad_wvalid = 0;
    dut->biu_pad_wlast = 0;

    bool saw_bvalid = false;
    for (int i = 0; i < 10; ++i) {
        if (dut->pad_biu_bvalid) {
            saw_bvalid = true;
            tick(ctx, dut);
            break;
        }
        tick(ctx, dut);
    }
    if (!saw_bvalid) {
        std::cerr << "[case_006] write response not observed" << std::endl;
        return 1;
    }

    // Basic read handshake check in idle state.
    dut->biu_pad_arvalid = 1;
    dut->biu_pad_araddr = 0x200;
    dut->biu_pad_arlen = 0;
    dut->biu_pad_arburst = 1;
    dut->biu_pad_arsize = 3;
    dut->biu_pad_arcache = 0xA;
    dut->biu_pad_arid = 0x1C;
    dut->biu_pad_arprot = 0x5;
    dut->biu_pad_artrust = 1;
    dut->biu_pad_rready = 1;

    dut->eval();
    if (dut->pad_biu_arready != 1 || dut->pad_biu_awready != 0) {
        std::cerr << "[case_006] idle AR ready check failed" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    dut->biu_pad_arvalid = 0;

    // Randomized-but-deterministic stimulus for coverage.
    uint32_t lfsr = 0x1ACEB00Cu;
    for (int cycle = 0; cycle < 300; ++cycle) {
        step_lfsr(lfsr);
        dut->biu_pad_awvalid = (lfsr >> 0) & 1u;
        dut->biu_pad_arvalid = (lfsr >> 1) & 1u;
        dut->biu_pad_wvalid = (lfsr >> 2) & 1u;
        dut->biu_pad_wlast = (lfsr >> 3) & 1u;
        dut->biu_pad_bready = (lfsr >> 4) & 1u;
        dut->biu_pad_rready = (lfsr >> 5) & 1u;
        dut->pad_biu_hready = (lfsr >> 6) & 1u;
        dut->pad_biu_hresp = (lfsr >> 7) & 1u;
        dut->biu_pad_awlen = (lfsr >> 8) & 0x3u;
        dut->biu_pad_arlen = (lfsr >> 10) & 0x3u;
        dut->biu_pad_awaddr = (static_cast<uint64_t>(lfsr) << 8) | 0x55u;
        dut->biu_pad_araddr = (static_cast<uint64_t>(lfsr) << 4) | 0xAAu;
        dut->biu_pad_awid = (lfsr >> 12) & 0xFFu;
        dut->biu_pad_arid = (lfsr >> 20) & 0xFFu;
        dut->biu_pad_awburst = (lfsr >> 2) & 0x3u;
        dut->biu_pad_arburst = (lfsr >> 4) & 0x3u;
        dut->biu_pad_awcache = (lfsr >> 8) & 0xFu;
        dut->biu_pad_arcache = (lfsr >> 16) & 0xFu;
        dut->biu_pad_awsize = (lfsr >> 10) & 0x7u;
        dut->biu_pad_arsize = (lfsr >> 13) & 0x7u;
        dut->biu_pad_awprot = (lfsr >> 6) & 0x7u;
        dut->biu_pad_arprot = (lfsr >> 9) & 0x7u;
        dut->biu_pad_awlock = (lfsr >> 18) & 1u;
        dut->biu_pad_arlock = (lfsr >> 19) & 1u;
        dut->biu_pad_artrust = (lfsr >> 11) & 1u;
        dut->biu_pad_wstrb = (lfsr >> 16) & 0xFFFFu;
        dut->biu_pad_wid = (lfsr >> 24) & 0xFFu;
        drive_wide128(dut->biu_pad_wdata, lfsr, lfsr ^ 0xA5A5A5A5u,
                      lfsr ^ 0x5A5A5A5Au, lfsr ^ 0x3C3C3C3Cu);
        drive_wide128(dut->pad_biu_hrdata, lfsr ^ 0x0F0F0F0Fu,
                      lfsr ^ 0xF0F0F0F0u, lfsr ^ 0xAAAAAAAAu,
                      lfsr ^ 0x55555555u);

        tick(ctx, dut);
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
