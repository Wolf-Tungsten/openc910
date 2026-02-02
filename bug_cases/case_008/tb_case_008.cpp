#include "Vplic_hreg_busif.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

template <typename T>
static inline void zero_signal(T &signal) {
    signal = 0;
}

template <std::size_t W>
static inline void zero_signal(VlWide<W> &signal) {
    VL_ZERO_W(W, signal);
}

static void tick(VerilatedContext *ctx, Vplic_hreg_busif *dut) {
    dut->plic_clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->plic_clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void init_inputs(Vplic_hreg_busif *dut) {
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
    dut->ciu_plic_icg_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->kid_hreg_ip_prio_reg_we = 0;

    dut->ctrl_xx_amp_mode = 0;
    dut->ctrl_xx_core_sec = 0;
    zero_signal(dut->int_sec_infor);
}

static bool wait_for_ready(VerilatedContext *ctx, Vplic_hreg_busif *dut) {
    for (int i = 0; i < 10; ++i) {
        tick(ctx, dut);
        if (dut->ie_bus_mtx_pready) {
            return true;
        }
    }
    return false;
}

static uint8_t do_ie_read(VerilatedContext *ctx, Vplic_hreg_busif *dut, uint8_t addr,
                          uint8_t pprot, uint8_t psec) {
    dut->bus_mtx_ie_paddr = addr;
    dut->bus_mtx_ie_pprot = pprot;
    dut->bus_mtx_ie_psec = psec;
    dut->bus_mtx_ie_pwrite = 0;
    dut->bus_mtx_ie_pwdata = 0;

    dut->bus_mtx_ie_psel = 1;
    dut->bus_mtx_ie_penable = 0;
    tick(ctx, dut);

    dut->bus_mtx_ie_penable = 1;
    if (!wait_for_ready(ctx, dut)) {
        std::cerr << "[TB] timeout waiting for ie_bus_mtx_pready" << std::endl;
        return 0xff;
    }
    const uint8_t slverr = dut->ie_bus_mtx_pslverr;

    dut->bus_mtx_ie_psel = 0;
    dut->bus_mtx_ie_penable = 0;
    tick(ctx, dut);

    return slverr;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vplic_hreg_busif(ctx);
    init_inputs(dut);

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->plicrst_b = 1;

    // Protection mismatch should assert slverr.
    uint8_t slverr = do_ie_read(ctx, dut, 0x00, 0x0, 1);
    if (slverr != 1) {
        std::cerr << "[TB] expected slverr for IE protection mismatch" << std::endl;
        return 1;
    }

    // Legal IE address should not assert slverr.
    slverr = do_ie_read(ctx, dut, 0x00, 0x3, 1);
    if (slverr != 0) {
        std::cerr << "[TB] unexpected slverr for legal IE address" << std::endl;
        return 1;
    }

    // Security mismatch should assert slverr.
    dut->ctrl_xx_core_sec = 1;
    slverr = do_ie_read(ctx, dut, 0x00, 0x3, 0);
    if (slverr != 1) {
        std::cerr << "[TB] expected slverr for security mismatch" << std::endl;
        return 1;
    }

    for (int i = 0; i < 5; ++i) {
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
