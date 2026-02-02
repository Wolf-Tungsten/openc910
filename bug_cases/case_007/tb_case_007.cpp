#include "Vgpio.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

namespace {

constexpr uint8_t kSwPortaDr = 0x00;
constexpr uint8_t kSwPortaDdr = 0x01;
constexpr uint8_t kSwPortaCtl = 0x02;
constexpr uint8_t kIntEn = 0x0C;
constexpr uint8_t kIntMask = 0x0D;
constexpr uint8_t kIntTypeLevel = 0x0E;
constexpr uint8_t kIntPolarity = 0x0F;
constexpr uint8_t kIntStatus = 0x10;
constexpr uint8_t kRawIntStatus = 0x11;
constexpr uint8_t kPortaEoi = 0x13;
constexpr uint8_t kExtPorta = 0x14;
constexpr uint8_t kLsSync = 0x18;

void tick(VerilatedContext *ctx, Vgpio *dut) {
    dut->pclk = 0;
    dut->pclk_intr = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->pclk = 1;
    dut->pclk_intr = 1;
    dut->eval();
    ctx->timeInc(1);
}

void idle(VerilatedContext *ctx, Vgpio *dut, int cycles) {
    for (int i = 0; i < cycles; ++i) {
        tick(ctx, dut);
    }
}

void apb_write(VerilatedContext *ctx, Vgpio *dut, uint8_t addr, uint32_t data) {
    dut->paddr = addr & 0x1F;
    dut->pwdata = data;
    dut->pwrite = 1;
    dut->psel = 1;
    dut->penable = 0;
    tick(ctx, dut);
    dut->penable = 1;
    tick(ctx, dut);
    dut->psel = 0;
    dut->penable = 0;
    dut->pwrite = 0;
    tick(ctx, dut);
}

uint32_t apb_read(VerilatedContext *ctx, Vgpio *dut, uint8_t addr) {
    dut->paddr = addr & 0x1F;
    dut->pwrite = 0;
    dut->psel = 1;
    dut->penable = 0;
    tick(ctx, dut);
    uint32_t value = dut->prdata;
    dut->psel = 0;
    dut->penable = 0;
    tick(ctx, dut);
    return value;
}

bool expect_eq(const char *label, uint32_t got, uint32_t exp) {
    if (got != exp) {
        std::cerr << "[TB] " << label << " mismatch got=0x" << std::hex << got
                  << " exp=0x" << exp << std::dec << std::endl;
        return false;
    }
    return true;
}

}  // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vgpio(ctx);

    dut->paddr = 0;
    dut->pclk = 0;
    dut->pclk_intr = 0;
    dut->penable = 0;
    dut->psel = 0;
    dut->pwdata = 0;
    dut->pwrite = 0;
    dut->b_pad_gpio_porta = 0;
    dut->presetn = 0;

    idle(ctx, dut, 4);
    dut->presetn = 1;
    idle(ctx, dut, 2);

    bool ok = true;

    apb_write(ctx, dut, kSwPortaDr, 0xA5);
    apb_write(ctx, dut, kSwPortaDdr, 0xFF);
    apb_write(ctx, dut, kSwPortaCtl, 0x00);
    apb_write(ctx, dut, kIntEn, 0x0F);
    apb_write(ctx, dut, kIntMask, 0x55);
    apb_write(ctx, dut, kIntTypeLevel, 0xAA);
    apb_write(ctx, dut, kIntPolarity, 0xF0);
    apb_write(ctx, dut, kPortaEoi, 0xFF);
    apb_write(ctx, dut, kLsSync, 0x01);

    dut->b_pad_gpio_porta = 0x3C;
    idle(ctx, dut, 2);

    ok &= expect_eq("swporta_dr", apb_read(ctx, dut, kSwPortaDr) & 0xFF, 0xA5);
    ok &= expect_eq("swporta_ddr", apb_read(ctx, dut, kSwPortaDdr) & 0xFF, 0xFF);
    ok &= expect_eq("int_en", apb_read(ctx, dut, kIntEn) & 0xFF, 0x0F);
    ok &= expect_eq("int_mask", apb_read(ctx, dut, kIntMask) & 0xFF, 0x55);
    ok &= expect_eq("int_type_level", apb_read(ctx, dut, kIntTypeLevel) & 0xFF, 0xAA);
    ok &= expect_eq("int_polarity", apb_read(ctx, dut, kIntPolarity) & 0xFF, 0xF0);
    ok &= expect_eq("ls_sync", apb_read(ctx, dut, kLsSync) & 0x1, 0x1);

    uint32_t ext_porta_drive = apb_read(ctx, dut, kExtPorta) & 0xFF;
    ok &= expect_eq("ext_porta_driven", ext_porta_drive, 0xA5);

    apb_write(ctx, dut, kSwPortaDdr, 0x00);
    idle(ctx, dut, 2);
    apb_read(ctx, dut, kExtPorta);
    apb_read(ctx, dut, kIntStatus);
    apb_read(ctx, dut, kRawIntStatus);
    apb_read(ctx, dut, 0x1F);

    if (!ok) {
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
