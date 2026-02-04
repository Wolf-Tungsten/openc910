#include "Vsim_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace {

void tick(VerilatedContext *ctx, Vsim_top *dut) {
    dut->clk = 0;
    dut->jclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->clk = 1;
    dut->jclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

struct Wide128 {
    std::array<uint32_t, 4> w{};
};

Wide128 make_wide(uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3) {
    return Wide128{{w0, w1, w2, w3}};
}

Wide128 rotL1(const Wide128 &v) {
    return Wide128{{v.w[3], v.w[0], v.w[1], v.w[2]}};
}

Wide128 rotL2(const Wide128 &v) {
    return Wide128{{v.w[2], v.w[3], v.w[0], v.w[1]}};
}

Wide128 rotL3(const Wide128 &v) {
    return Wide128{{v.w[1], v.w[2], v.w[3], v.w[0]}};
}

Wide128 xor_const(const Wide128 &v, uint32_t c) {
    Wide128 out = v;
    for (auto &word : out.w) {
        word ^= c;
    }
    return out;
}

void drive_wide128(vluint32_t *bus, const Wide128 &data) {
    for (int i = 0; i < 4; ++i) {
        bus[i] = data.w[i];
    }
}

bool wide128_equals(const vluint32_t *bus, const Wide128 &data) {
    for (int i = 0; i < 4; ++i) {
        if (bus[i] != data.w[i]) {
            return false;
        }
    }
    return true;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::randReset(0);

    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vsim_top(ctx);

    dut->clk = 0;
    dut->jclk = 0;
    dut->rst_b = 0;
    dut->jrst_b = 0;
    dut->jtg_tdi = 0;
    dut->jtg_tms = 0;
    dut->uart0_sin = 0;
    dut->tb_init_en = 0;
    dut->tb_init_addr = 0;
    dut->tb_init_wen = 0;
    drive_wide128(dut->tb_init_wdata, make_wide(0u, 0u, 0u, 0u));

    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
    }

    dut->rst_b = 1;
    dut->jrst_b = 1;
    dut->tb_init_en = 1;
    dut->tb_init_addr = 0x12345;
    dut->tb_init_wen = 0x00ff;
    Wide128 init_wdata = make_wide(0x00112233u, 0x44556677u, 0x8899aabbu, 0xccddeeffu);
    drive_wide128(dut->tb_init_wdata, init_wdata);

    tick(ctx, dut);

    Wide128 stage0 = init_wdata;
    Wide128 stage1 = rotL1(stage0);
    Wide128 stage2 = xor_const(stage1, 0xA5A5A5A5u);
    Wide128 stage3 = rotL2(stage2);
    Wide128 stage4 = xor_const(stage3, 0x5A5A5A5Au);
    Wide128 stage5 = rotL3(stage4);
    Wide128 stage6 = xor_const(stage5, 0x3C3C3C3Cu);
    Wide128 stage7 = rotL1(stage6);
    Wide128 stage8 = xor_const(stage7, 0xC3C3C3C3u);
    Wide128 stage9 = rotL2(stage8);
    Wide128 stage10 = xor_const(stage9, 0x0F0F0F0Fu);
    Wide128 stage11 = rotL3(stage10);
    Wide128 stage12 = xor_const(stage11, 0xF0F0F0F0u);
    Wide128 stage13 = rotL1(stage12);
    Wide128 stage14 = xor_const(stage13, 0xAAAAAAAAu);
    Wide128 stage15 = rotL2(stage14);
    Wide128 stage16 = xor_const(stage15, 0x55555555u);
    Wide128 expected_wdata = rotL3(stage16);

    uint16_t wstrb_stage0 = 0x00ff;
    uint16_t wstrb_stage1 = static_cast<uint16_t>((wstrb_stage0 << 8) | (wstrb_stage0 >> 8));
    uint16_t wstrb_stage2 = static_cast<uint16_t>(wstrb_stage1 ^ 0xA5A5u);
    uint16_t wstrb_stage3 = static_cast<uint16_t>((wstrb_stage2 << 8) | (wstrb_stage2 >> 8));
    uint16_t wstrb_stage4 = static_cast<uint16_t>(wstrb_stage3 ^ 0x5A5Au);
    uint16_t wstrb_stage5 = static_cast<uint16_t>((wstrb_stage4 << 8) | (wstrb_stage4 >> 8));
    uint16_t wstrb_stage6 = static_cast<uint16_t>(wstrb_stage5 ^ 0x3C3Cu);
    uint16_t wstrb_stage7 = static_cast<uint16_t>((wstrb_stage6 << 8) | (wstrb_stage6 >> 8));
    uint16_t wstrb_stage8 = static_cast<uint16_t>(wstrb_stage7 ^ 0xA3A3u);
    uint16_t wstrb_stage9 = static_cast<uint16_t>((wstrb_stage8 << 8) | (wstrb_stage8 >> 8));
    uint16_t expected_wstrb = static_cast<uint16_t>(wstrb_stage9 ^ 0x5C5Cu);

    uint32_t addr = static_cast<uint32_t>(dut->tb_init_addr);
    uint32_t wvalid_expected = 1u;
    wvalid_expected ^= (addr >> 0) & 0x1u;
    wvalid_expected ^= (addr >> 1) & 0x1u;
    wvalid_expected ^= (addr >> 2) & 0x1u;
    wvalid_expected ^= (addr >> 3) & 0x1u;
    wvalid_expected ^= (addr >> 4) & 0x1u;
    wvalid_expected ^= (addr >> 5) & 0x1u;
    if (dut->retire0 != 1 || dut->retire1 != 0 || dut->retire2 != 0) {
        std::cerr << "[case_016] retire signals mismatch after first tick" << std::endl;
        return 1;
    }
    if (dut->axi_awlen != 2 || dut->axi_mem_addr != 8) {
        std::cerr << "[case_016] axi signals mismatch after first tick" << std::endl;
        return 1;
    }
    if (dut->biu_pad_wvalid != wvalid_expected || dut->biu_pad_wstrb != expected_wstrb) {
        std::cerr << "[case_016] biu_pad_w signals mismatch after first tick" << std::endl;
        return 1;
    }
    if (!wide128_equals(dut->biu_pad_wdata, expected_wdata)) {
        std::cerr << "[case_016] biu_pad_wdata mismatch after first tick" << std::endl;
        return 1;
    }

    tick(ctx, dut);

    if (dut->retire0 != 0 || dut->retire1 != 1 || dut->retire2 != 0) {
        std::cerr << "[case_016] retire signals mismatch after second tick" << std::endl;
        return 1;
    }
    if (dut->value0 != 4 || dut->value1 != 7 || dut->value2 != 8) {
        std::cerr << "[case_016] value signals mismatch after second tick" << std::endl;
        return 1;
    }
    if (dut->axi_awlen != 4 || dut->axi_mem_addr != 16) {
        std::cerr << "[case_016] axi signals mismatch after second tick" << std::endl;
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
