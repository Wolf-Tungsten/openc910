#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vgpio.h"

namespace {

constexpr uint8_t kAddrSwportaDr = 0x00;
constexpr uint8_t kAddrSwportaDdr = 0x01;
constexpr uint8_t kAddrSwportaCtl = 0x02;
constexpr uint8_t kAddrInten = 0x0c;
constexpr uint8_t kAddrIntmask = 0x0d;
constexpr uint8_t kAddrInttypeLevel = 0x0e;
constexpr uint8_t kAddrIntPolarity = 0x0f;
constexpr uint8_t kAddrIntstatus = 0x10;
constexpr uint8_t kAddrRawIntstatus = 0x11;
constexpr uint8_t kAddrPortaEoi = 0x13;
constexpr uint8_t kAddrExtPorta = 0x14;
constexpr uint8_t kAddrLsSync = 0x18;

void tick(Vgpio *dut, VerilatedContext *context) {
    dut->pclk = 0;
    dut->pclk_intr = 0;
    dut->eval();
    context->timeInc(1);
    dut->pclk = 1;
    dut->pclk_intr = 1;
    dut->eval();
    context->timeInc(1);
}

void idle_cycles(Vgpio *dut, VerilatedContext *context, int cycles) {
    for (int i = 0; i < cycles; ++i) {
        tick(dut, context);
    }
}

bool expect_eq(uint32_t actual, uint32_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

void apply_reset(Vgpio *dut, VerilatedContext *context) {
    dut->paddr = 0;
    dut->pwrite = 0;
    dut->psel = 0;
    dut->penable = 0;
    dut->pwdata = 0;
    dut->b_pad_gpio_porta = 0;
    dut->presetn = 1;
    dut->eval();
    dut->presetn = 0;
    dut->eval();
    idle_cycles(dut, context, 2);
    dut->presetn = 1;
    dut->eval();
    idle_cycles(dut, context, 2);
}

void apb_write(Vgpio *dut, VerilatedContext *context, uint8_t addr, uint32_t data) {
    dut->paddr = addr;
    dut->pwdata = data;
    dut->pwrite = 1;
    dut->psel = 1;
    dut->penable = 0;
    tick(dut, context);
    dut->penable = 1;
    tick(dut, context);
    dut->psel = 0;
    dut->penable = 0;
    dut->pwrite = 0;
    dut->paddr = 0;
    dut->pwdata = 0;
    tick(dut, context);
}

uint32_t apb_read(Vgpio *dut, VerilatedContext *context, uint8_t addr) {
    dut->paddr = addr;
    dut->pwrite = 0;
    dut->psel = 1;
    dut->penable = 0;
    tick(dut, context);
    dut->eval();
    uint32_t data = dut->prdata;
    dut->psel = 0;
    dut->paddr = 0;
    tick(dut, context);
    return data;
}

void drive_pad(Vgpio *dut, uint8_t value) {
    dut->b_pad_gpio_porta = value;
    dut->eval();
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vgpio>(context.get());
    dut->paddr = 0;
    dut->pclk = 0;
    dut->pclk_intr = 0;
    dut->penable = 0;
    dut->presetn = 0;
    dut->psel = 0;
    dut->pwdata = 0;
    dut->pwrite = 0;
    dut->b_pad_gpio_porta = 0;
    dut->eval();

    apply_reset(dut.get(), context.get());

    if (!expect_eq(static_cast<uint32_t>(dut->gpio_intr), 0, "reset_gpio_intr")) {
        return EXIT_FAILURE;
    }

    apb_write(dut.get(), context.get(), kAddrSwportaDr, 0xA5);
    apb_write(dut.get(), context.get(), kAddrSwportaDdr, 0x00);
    apb_write(dut.get(), context.get(), kAddrSwportaCtl, 0x00);
    apb_write(dut.get(), context.get(), kAddrInten, 0xF0);
    apb_write(dut.get(), context.get(), kAddrIntmask, 0x00);
    apb_write(dut.get(), context.get(), kAddrInttypeLevel, 0xAA);
    apb_write(dut.get(), context.get(), kAddrIntPolarity, 0x5A);
    apb_write(dut.get(), context.get(), kAddrLsSync, 0x01);

    uint32_t dr = apb_read(dut.get(), context.get(), kAddrSwportaDr);
    if (!expect_eq(dr & 0xFFu, 0xA5u, "swporta_dr_readback")) {
        return EXIT_FAILURE;
    }

    drive_pad(dut.get(), 0x00);
    idle_cycles(dut.get(), context.get(), 2);
    drive_pad(dut.get(), 0xFF);
    idle_cycles(dut.get(), context.get(), 2);
    drive_pad(dut.get(), 0x3C);
    idle_cycles(dut.get(), context.get(), 2);

    uint32_t ext_porta = apb_read(dut.get(), context.get(), kAddrExtPorta);
    if (!expect_eq(ext_porta & 0xFFu, 0x3Cu, "ext_porta_input_readback")) {
        return EXIT_FAILURE;
    }

    apb_write(dut.get(), context.get(), kAddrPortaEoi, 0xFF);
    idle_cycles(dut.get(), context.get(), 2);

    (void)apb_read(dut.get(), context.get(), kAddrIntstatus);
    (void)apb_read(dut.get(), context.get(), kAddrRawIntstatus);

    apb_write(dut.get(), context.get(), kAddrSwportaDdr, 0xFF);
    apb_write(dut.get(), context.get(), kAddrSwportaDr, 0x5A);
    drive_pad(dut.get(), 0x5A);
    idle_cycles(dut.get(), context.get(), 2);

    ext_porta = apb_read(dut.get(), context.get(), kAddrExtPorta);
    if (!expect_eq(ext_porta & 0xFFu, 0x5Au, "ext_porta_output_readback")) {
        return EXIT_FAILURE;
    }

    apb_write(dut.get(), context.get(), kAddrLsSync, 0x00);
    apb_write(dut.get(), context.get(), kAddrSwportaDdr, 0x00);
    drive_pad(dut.get(), 0xC3);
    idle_cycles(dut.get(), context.get(), 2);

    ext_porta = apb_read(dut.get(), context.get(), kAddrExtPorta);
    if (!expect_eq(ext_porta & 0xFFu, 0xC3u, "ext_porta_input_roundtrip")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_011 passed: gpio apb smoke" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
