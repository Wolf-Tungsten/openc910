#include "Vplic_hreg_busif.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

static void tick(VerilatedContext *ctx, Vplic_hreg_busif *dut) {
    dut->plic_clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->plic_clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void clear_inputs(Vplic_hreg_busif *dut) {
    dut->plic_clk = 0;
    dut->plicrst_b = 0;
    dut->bus_mtx_ict_psel = 0;
    dut->bus_mtx_ict_pprot = 0;
    dut->bus_mtx_ict_penable = 0;
    dut->bus_mtx_ict_paddr = 0;
    dut->bus_mtx_ict_pwrite = 0;
    dut->bus_mtx_ict_pwdata = 0;
    dut->bus_mtx_ict_psec = 0;
    dut->bus_mtx_ie_psel = 0;
    dut->bus_mtx_ie_pprot = 0;
    dut->bus_mtx_ie_penable = 0;
    dut->bus_mtx_ie_paddr = 0;
    dut->bus_mtx_ie_pwrite = 0;
    dut->bus_mtx_ie_pwdata = 0;
    dut->bus_mtx_ie_psec = 0;
    dut->arbx_hreg_claim_reg_ready = 0;
    dut->arbx_hreg_claim_mmode = 0;
    dut->arbx_hreg_claim_id = 0;
    dut->arbx_hreg_arb_start_ack = 0;
    dut->kid_hreg_new_int_pulse = 0;
    dut->pad_yy_icg_scan_en = 0;
    dut->kid_hreg_ip_prio_reg_we = 0;
    dut->ciu_plic_icg_en = 1;
    dut->ctrl_xx_amp_mode = 0;
    dut->ctrl_xx_core_sec = 0;
    std::memset(&dut->int_sec_infor, 0, sizeof(dut->int_sec_infor));
}

static bool buffer_is_zero(const void *buf, size_t size) {
    const auto *bytes = static_cast<const uint8_t *>(buf);
    for (size_t i = 0; i < size; ++i) {
        if (bytes[i] != 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vplic_hreg_busif(ctx);

    clear_inputs(dut);
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    dut->plicrst_b = 1;
    tick(ctx, dut);

    if (!buffer_is_zero(&dut->hreg_arbx_int_en, sizeof(dut->hreg_arbx_int_en))) {
        std::cerr << "[TB] expected hreg_arbx_int_en to reset to 0" << std::endl;
        return 1;
    }
    if (!buffer_is_zero(&dut->hreg_arbx_int_mmode, sizeof(dut->hreg_arbx_int_mmode))) {
        std::cerr << "[TB] expected hreg_arbx_int_mmode to reset to 0" << std::endl;
        return 1;
    }

    dut->pad_yy_icg_scan_en = 1;
    dut->ctrl_xx_amp_mode = 1;
    auto *int_sec_words = reinterpret_cast<uint32_t *>(&dut->int_sec_infor);
    size_t word_count = sizeof(dut->int_sec_infor) / sizeof(uint32_t);
    for (size_t i = 0; i < word_count && i < 4; ++i) {
        int_sec_words[i] = 0x11110000u | static_cast<uint32_t>(i);
    }

    for (int hart = 0; hart < 4; ++hart) {
        dut->ctrl_xx_core_sec = static_cast<uint8_t>(1u << hart);
        dut->arbx_hreg_claim_reg_ready = static_cast<uint8_t>(1u << hart);
        dut->arbx_hreg_claim_mmode = static_cast<uint8_t>(hart & 0x3);
        dut->arbx_hreg_claim_id = static_cast<uint64_t>(0x1000u + hart);
        dut->arbx_hreg_arb_start_ack = static_cast<uint8_t>(1u << hart);
        dut->kid_hreg_new_int_pulse = (hart & 1);
        dut->kid_hreg_ip_prio_reg_we = 1;

        uint32_t ie_addr = static_cast<uint32_t>(hart << 8);
        dut->bus_mtx_ie_paddr = ie_addr;
        dut->bus_mtx_ie_pwdata = 0xA5A50000u | static_cast<uint32_t>(hart);
        dut->bus_mtx_ie_psec = (hart & 1);
        dut->bus_mtx_ie_psel = 1;
        dut->bus_mtx_ie_pwrite = 1;
        dut->bus_mtx_ie_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ie_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ie_psel = 0;
        dut->bus_mtx_ie_penable = 0;
        tick(ctx, dut);

        dut->bus_mtx_ie_pwrite = 0;
        dut->bus_mtx_ie_psel = 1;
        dut->bus_mtx_ie_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ie_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ie_psel = 0;
        dut->bus_mtx_ie_penable = 0;
        tick(ctx, dut);

        uint32_t ie_addr_sie = ie_addr | 0x80u;
        dut->bus_mtx_ie_paddr = ie_addr_sie;
        dut->bus_mtx_ie_pwrite = 0;
        dut->bus_mtx_ie_psel = 1;
        dut->bus_mtx_ie_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ie_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ie_psel = 0;
        dut->bus_mtx_ie_penable = 0;
        tick(ctx, dut);

        uint32_t ict_addr = static_cast<uint32_t>(hart << 13);
        dut->bus_mtx_ict_paddr = ict_addr;
        dut->bus_mtx_ict_pwdata = 0x5A5A0000u | static_cast<uint32_t>(hart);
        dut->bus_mtx_ict_psec = (hart & 1);
        dut->bus_mtx_ict_pprot = 0x3;
        dut->bus_mtx_ict_psel = 1;
        dut->bus_mtx_ict_pwrite = 1;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ict_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ict_psel = 0;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);

        dut->bus_mtx_ict_pwrite = 0;
        dut->bus_mtx_ict_psel = 1;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ict_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ict_psel = 0;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);

        uint32_t mclaim_addr = ict_addr | 0x4u;
        dut->bus_mtx_ict_paddr = mclaim_addr;
        dut->bus_mtx_ict_pwrite = 0;
        dut->bus_mtx_ict_psel = 1;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ict_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ict_psel = 0;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);

        uint32_t sclaim_addr = ict_addr | 0x1000u | 0x4u;
        dut->bus_mtx_ict_paddr = sclaim_addr;
        dut->bus_mtx_ict_pwrite = 0;
        dut->bus_mtx_ict_psel = 1;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);
        dut->bus_mtx_ict_penable = 1;
        tick(ctx, dut);
        dut->bus_mtx_ict_psel = 0;
        dut->bus_mtx_ict_penable = 0;
        tick(ctx, dut);
    }

    dut->kid_hreg_new_int_pulse = 0;
    dut->kid_hreg_ip_prio_reg_we = 0;
    tick(ctx, dut);

    for (int i = 0; i < 6; ++i) {
        tick(ctx, dut);
        if (ctx->gotFinish()) {
            break;
        }
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
