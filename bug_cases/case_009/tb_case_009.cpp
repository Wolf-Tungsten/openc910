#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "verilated.h"
#include "Vcpu_sub_system_axi.h"

namespace {

struct Wide128 {
    uint64_t lo;
    uint64_t hi;
};

Wide128 read_wdata(const Vcpu_sub_system_axi &dut) {
    Wide128 out{};
    out.lo = static_cast<uint64_t>(dut.biu_pad_wdata[0]) |
             (static_cast<uint64_t>(dut.biu_pad_wdata[1]) << 32);
    out.hi = static_cast<uint64_t>(dut.biu_pad_wdata[2]) |
             (static_cast<uint64_t>(dut.biu_pad_wdata[3]) << 32);
    return out;
}

bool expect_eq(uint64_t actual, uint64_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

bool check_pad_plic(Vcpu_sub_system_axi &dut, uint64_t pattern, const char *label) {
    constexpr uint64_t kMask32 = 0xFFFFFFFFULL;

    dut.xx_intc_vld = pattern;
    dut.eval();

    const Wide128 wdata = read_wdata(dut);
    if (!expect_eq(wdata.lo & kMask32, pattern & kMask32, label)) {
        return false;
    }
    if (!expect_eq(wdata.lo >> 32, 0, "pad_plic_bits32_63_zero")) {
        return false;
    }
    if (!expect_eq(wdata.hi, 0, "pad_plic_bits64_127_zero")) {
        return false;
    }
    if (!expect_eq(static_cast<uint64_t>(dut.biu_pad_wstrb), 0,
                   "pad_plic_bits128_143_zero")) {
        return false;
    }
    return true;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    Vcpu_sub_system_axi dut;
    dut.axim_clk_en = 0;
    dut.pad_biu_arready = 0;
    dut.pad_biu_awready = 0;
    dut.pad_biu_bid = 0;
    dut.pad_biu_bresp = 0;
    dut.pad_biu_bvalid = 0;
    dut.pad_biu_rdata[0] = 0;
    dut.pad_biu_rdata[1] = 0;
    dut.pad_biu_rdata[2] = 0;
    dut.pad_biu_rdata[3] = 0;
    dut.pad_biu_rid = 0;
    dut.pad_biu_rlast = 0;
    dut.pad_biu_rresp = 0;
    dut.pad_biu_rvalid = 0;
    dut.pad_biu_wready = 0;
    dut.pad_cpu_rst_b = 1;
    dut.pad_had_jtg_tclk = 0;
    dut.pad_had_jtg_tdi = 0;
    dut.pad_had_jtg_trst_b = 0;
    dut.pad_yy_dft_clk_rst_b = 0;
    dut.pll_cpu_clk = 0;
    dut.xx_intc_vld = 0;
    dut.per_clk = 0;
    dut.i_pad_jtg_tms = 0;

    dut.eval();

    bool ok = true;
    ok &= expect_eq(dut.biu_pad_awvalid, 0, "biu_pad_awvalid");
    ok &= expect_eq(dut.biu_pad_bready, 0, "biu_pad_bready");
    ok &= expect_eq(dut.biu_pad_wid, 0, "biu_pad_wid");
    ok &= expect_eq(dut.had_pad_jtg_tdo, 0, "had_pad_jtg_tdo");
    ok &= check_pad_plic(dut, 0x00ABCDEF12ULL, "pad_plic_low32_pattern1");
    ok &= check_pad_plic(dut, 0x0012345678ULL, "pad_plic_low32_pattern2");

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
