#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vct_ifu_icache_if.h"

namespace {

void tick(Vct_ifu_icache_if *dut, VerilatedContext *context) {
    dut->forever_cpuclk = 0;
    dut->eval();
    context->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    context->timeInc(1);
}

bool expect_eq(uint32_t actual, uint32_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

void zero_inputs(Vct_ifu_icache_if *dut) {
    dut->cp0_ifu_icache_en = 0;
    dut->cp0_ifu_icg_en = 0;
    dut->cp0_yy_clk_en = 0;
    dut->cpurst_b = 0;
    dut->forever_cpuclk = 0;
    dut->hpcp_ifu_cnt_en = 0;
    dut->ifctrl_icache_if_index = 0;
    dut->ifctrl_icache_if_inv_fifo = 0;
    dut->ifctrl_icache_if_inv_on = 0;
    dut->ifctrl_icache_if_read_req_data0 = 0;
    dut->ifctrl_icache_if_read_req_data1 = 0;
    dut->ifctrl_icache_if_read_req_index = 0;
    dut->ifctrl_icache_if_read_req_tag = 0;
    dut->ifctrl_icache_if_reset_req = 0;
    dut->ifctrl_icache_if_tag_req = 0;
    dut->ifctrl_icache_if_tag_wen = 0;
    dut->ifu_hpcp_icache_miss_pre = 0;
    dut->ipb_icache_if_index = 0;
    dut->ipb_icache_if_req = 0;
    dut->ipb_icache_if_req_for_gateclk = 0;
    dut->l1_refill_icache_if_fifo = 0;
    dut->l1_refill_icache_if_first = 0;
    dut->l1_refill_icache_if_index = 0;
    std::memset(dut->l1_refill_icache_if_inst_data, 0,
                sizeof(dut->l1_refill_icache_if_inst_data));
    dut->l1_refill_icache_if_last = 0;
    dut->l1_refill_icache_if_pre_code = 0;
    dut->l1_refill_icache_if_ptag = 0;
    dut->l1_refill_icache_if_wr = 0;
    dut->pad_yy_icg_scan_en = 0;
    dut->pcgen_icache_if_chgflw = 0;
    dut->pcgen_icache_if_chgflw_bank0 = 0;
    dut->pcgen_icache_if_chgflw_bank1 = 0;
    dut->pcgen_icache_if_chgflw_bank2 = 0;
    dut->pcgen_icache_if_chgflw_bank3 = 0;
    dut->pcgen_icache_if_chgflw_short = 0;
    dut->pcgen_icache_if_gateclk_en = 0;
    dut->pcgen_icache_if_index = 0;
    dut->pcgen_icache_if_seq_data_req = 0;
    dut->pcgen_icache_if_seq_data_req_short = 0;
    dut->pcgen_icache_if_seq_tag_req = 0;
    dut->pcgen_icache_if_way_pred = 0;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vct_ifu_icache_if>(context.get());
    zero_inputs(dut.get());
    dut->eval();

    for (int i = 0; i < 2; ++i) {
        tick(dut.get(), context.get());
    }

    if (!expect_eq(dut->ifu_hpcp_icache_access, 0, "reset_access") ||
        !expect_eq(dut->ifu_hpcp_icache_miss, 0, "reset_miss")) {
        return EXIT_FAILURE;
    }

    dut->cpurst_b = 1;
    dut->cp0_yy_clk_en = 1;
    dut->cp0_ifu_icg_en = 1;
    dut->cp0_ifu_icache_en = 1;
    dut->hpcp_ifu_cnt_en = 1;
    dut->pcgen_icache_if_gateclk_en = 1;
    dut->pcgen_icache_if_seq_tag_req = 1;
    dut->pcgen_icache_if_seq_data_req = 1;
    dut->pcgen_icache_if_index = 0x55;
    dut->pcgen_icache_if_way_pred = 0x1;
    dut->ifctrl_icache_if_index = 0x123;
    dut->l1_refill_icache_if_index = 0x1ab;
    dut->l1_refill_icache_if_wr = 1;
    dut->l1_refill_icache_if_first = 1;
    dut->l1_refill_icache_if_last = 1;
    dut->l1_refill_icache_if_pre_code = 0x5a5a5a5a;
    dut->l1_refill_icache_if_ptag = 0x1fffff;
    dut->l1_refill_icache_if_inst_data[0] = 0xdeadbeef;
    dut->l1_refill_icache_if_inst_data[1] = 0xcafebabe;
    dut->l1_refill_icache_if_inst_data[2] = 0x12345678;
    dut->l1_refill_icache_if_inst_data[3] = 0x0;
    dut->ifu_hpcp_icache_miss_pre = 1;

    tick(dut.get(), context.get());
    if (!expect_eq(dut->ifu_hpcp_icache_access, 1, "access_capture") ||
        !expect_eq(dut->ifu_hpcp_icache_miss, 1, "miss_capture")) {
        return EXIT_FAILURE;
    }

    dut->pcgen_icache_if_seq_data_req = 0;
    dut->pcgen_icache_if_chgflw = 0;
    dut->ifu_hpcp_icache_miss_pre = 0;
    tick(dut.get(), context.get());
    if (!expect_eq(dut->ifu_hpcp_icache_access, 0, "access_clear") ||
        !expect_eq(dut->ifu_hpcp_icache_miss, 0, "miss_clear")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_001 passed: basic smoke stimulus executed\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
