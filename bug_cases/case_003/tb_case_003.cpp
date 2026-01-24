#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vct_ifu_ifctrl.h"

namespace {

void tick(Vct_ifu_ifctrl *dut, VerilatedContext *context) {
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

bool expect_eq_u64(uint64_t actual, uint64_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

void zero_inputs(Vct_ifu_ifctrl *dut) {
    dut->bht_ifctrl_inv_done = 0;
    dut->bht_ifctrl_inv_on = 0;
    dut->btb_ifctrl_inv_done = 0;
    dut->btb_ifctrl_inv_on = 0;
    dut->cp0_ifu_bht_inv = 0;
    dut->cp0_ifu_btb_inv = 0;
    dut->cp0_ifu_icache_inv = 0;
    dut->cp0_ifu_icache_read_index = 0;
    dut->cp0_ifu_icache_read_req = 0;
    dut->cp0_ifu_icache_read_tag = 0;
    dut->cp0_ifu_icache_read_way = 0;
    dut->cp0_ifu_icg_en = 0;
    dut->cp0_ifu_ind_btb_inv = 0;
    dut->cp0_ifu_no_op_req = 0;
    dut->cp0_yy_clk_en = 0;
    dut->cpurst_b = 0;
    dut->forever_cpuclk = 0;
    dut->hpcp_ifu_cnt_en = 0;
    dut->ind_btb_ifctrl_inv_done = 0;
    dut->ind_btb_ifctrl_inv_on = 0;
    dut->ipb_ifctrl_prefetch_idle = 0;
    dut->ipctrl_ifctrl_bht_stall = 0;
    dut->ipctrl_ifctrl_stall = 0;
    dut->ipctrl_ifctrl_stall_short = 0;
    dut->l0_btb_ifctrl_chgflw_pc = 0;
    dut->l0_btb_ifctrl_chgflw_way_pred = 0;
    dut->l0_btb_ifctrl_chglfw_vld = 0;
    dut->l1_refill_ifctrl_ctc = 0;
    dut->l1_refill_ifctrl_idle = 0;
    dut->l1_refill_ifctrl_pc = 0;
    dut->l1_refill_ifctrl_refill_on = 0;
    dut->l1_refill_ifctrl_reissue = 0;
    dut->l1_refill_ifctrl_start = 0;
    dut->l1_refill_ifctrl_start_for_gateclk = 0;
    dut->l1_refill_ifctrl_trans_cmplt = 0;
    dut->l1_refill_inv_wfd_back = 0;
    dut->lsu_ifu_icache_all_inv = 0;
    dut->lsu_ifu_icache_index = 0;
    dut->lsu_ifu_icache_line_inv = 0;
    dut->lsu_ifu_icache_ptag = 0;
    dut->mmu_ifu_pavld = 0;
    dut->pad_yy_icg_scan_en = 0;
    dut->pcgen_ifctrl_cancel = 0;
    dut->pcgen_ifctrl_pc = 0;
    dut->pcgen_ifctrl_pipe_cancel = 0;
    dut->pcgen_ifctrl_reissue = 0;
    dut->pcgen_ifctrl_way_pred = 0;
    dut->pcgen_ifctrl_way_pred_stall = 0;
    dut->rtu_ifu_xx_dbgon = 0;
    dut->vector_ifctrl_reset_on = 0;
    dut->vector_ifctrl_sm_on = 0;
    dut->vector_ifctrl_sm_start = 0;
    std::memset(dut->icache_if_ifctrl_inst_data0, 0,
                sizeof(dut->icache_if_ifctrl_inst_data0));
    std::memset(dut->icache_if_ifctrl_inst_data1, 0,
                sizeof(dut->icache_if_ifctrl_inst_data1));
    dut->icache_if_ifctrl_tag_data0 = 0;
    dut->icache_if_ifctrl_tag_data1 = 0;
}

void apply_realistic_fetch_inputs(Vct_ifu_ifctrl *dut) {
    dut->cpurst_b = 1;
    dut->cp0_yy_clk_en = 1;
    dut->cp0_ifu_icg_en = 1;
    dut->cp0_ifu_no_op_req = 0;
    dut->ipb_ifctrl_prefetch_idle = 1;
    dut->l1_refill_ifctrl_idle = 1;
    dut->l1_refill_ifctrl_start = 0;
    dut->l1_refill_ifctrl_start_for_gateclk = 0;
    dut->l1_refill_ifctrl_refill_on = 0;
    dut->mmu_ifu_pavld = 1;
    dut->pcgen_ifctrl_way_pred = 1;
    dut->pcgen_ifctrl_pc = 0x0000000012345ULL;
    dut->pcgen_ifctrl_cancel = 0;
    dut->pcgen_ifctrl_pipe_cancel = 0;
    dut->l0_btb_ifctrl_chglfw_vld = 1;
    dut->l0_btb_ifctrl_chgflw_way_pred = 2;
    dut->l0_btb_ifctrl_chgflw_pc = 0x0000000023456ULL;
    dut->l1_refill_ifctrl_pc = 0x0000000034567ULL;
    dut->cp0_ifu_icache_read_index = 0x135;
    dut->cp0_ifu_icache_read_req = 0;
    dut->cp0_ifu_icache_read_tag = 1;
    dut->cp0_ifu_icache_read_way = 1;
    dut->lsu_ifu_icache_index = 0x1a;
    dut->lsu_ifu_icache_ptag = 0x2bcdef;
    dut->icache_if_ifctrl_tag_data0 = 0x15555;
    dut->icache_if_ifctrl_tag_data1 = 0x2aaaa;
    dut->icache_if_ifctrl_inst_data0[0] = 0xdeadbeef;
    dut->icache_if_ifctrl_inst_data0[1] = 0x12345678;
    dut->icache_if_ifctrl_inst_data0[2] = 0x0badf00d;
    dut->icache_if_ifctrl_inst_data0[3] = 0xcafebabe;
    dut->icache_if_ifctrl_inst_data1[0] = 0x89abcdef;
    dut->icache_if_ifctrl_inst_data1[1] = 0x13579bdf;
    dut->icache_if_ifctrl_inst_data1[2] = 0x2468ace0;
    dut->icache_if_ifctrl_inst_data1[3] = 0xfeedface;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vct_ifu_ifctrl>(context.get());
    zero_inputs(dut.get());
    dut->eval();

    if (!expect_eq(dut->ifu_no_op_req, 0, "no_op_req_reset") ||
        !expect_eq(dut->ifu_yy_xx_no_op, 0, "no_op_reset")) {
        return EXIT_FAILURE;
    }

    apply_realistic_fetch_inputs(dut.get());
    tick(dut.get(), context.get());

    if (!expect_eq(dut->ifu_no_op_req, 0, "no_op_req_hold") ||
        !expect_eq(dut->ifu_yy_xx_no_op, 1, "no_op_flop_set") ||
        !expect_eq(dut->ifctrl_debug_if_pc_vld, 1, "pc_vld_on") ||
        !expect_eq(dut->ifctrl_debug_if_vld, 1, "if_vld_on") ||
        !expect_eq(dut->ifctrl_ifdp_pipedown, 1, "if_pipedown") ||
        !expect_eq(dut->ifctrl_pcgen_chgflw_vld, 1, "chgflw_vld") ||
        !expect_eq(dut->ifctrl_pcgen_way_pred, 2, "way_pred_sel") ||
        !expect_eq_u64(dut->ifctrl_pcgen_pcload_pc, 0x0000000023456ULL,
                       "pcload_pc")) {
        return EXIT_FAILURE;
    }

    dut->ipctrl_ifctrl_stall = 1;
    dut->eval();
    if (!expect_eq(dut->ifctrl_pcgen_stall, 1, "stall_on_ipctrl") ||
        !expect_eq(dut->ifctrl_ifdp_pipedown, 0, "pipedown_blocked") ||
        !expect_eq(dut->ifctrl_debug_if_stall, 0, "debug_self_stall")) {
        return EXIT_FAILURE;
    }

    dut->ipctrl_ifctrl_stall = 0;
    dut->l1_refill_ifctrl_start = 1;
    dut->l1_refill_ifctrl_start_for_gateclk = 1;
    tick(dut.get(), context.get());
    if (!expect_eq(dut->ifu_yy_xx_no_op, 0, "no_op_masked")) {
        return EXIT_FAILURE;
    }

    dut->l1_refill_ifctrl_start = 0;
    dut->l1_refill_ifctrl_start_for_gateclk = 0;
    tick(dut.get(), context.get());
    if (!expect_eq(dut->ifu_yy_xx_no_op, 1, "no_op_restored")) {
        return EXIT_FAILURE;
    }

    dut->cp0_ifu_no_op_req = 1;
    dut->eval();
    if (!expect_eq(dut->ifu_no_op_req, 1, "no_op_req_direct") ||
        !expect_eq(dut->ifctrl_debug_if_pc_vld, 0, "pc_vld_blocked")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_003 passed: no-op path checks\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
