#include "Vct_idu_is_lsiq_entry.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstddef>
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

static inline uint32_t xorshift32(uint32_t &state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

template <std::size_t W>
static inline void fill_random(VlWide<W> &signal, uint32_t &state) {
    for (std::size_t i = 0; i < W; ++i) {
        signal[i] = xorshift32(state);
    }
}

static inline uint8_t rand_bit(uint32_t &state) {
    return static_cast<uint8_t>(xorshift32(state) & 1u);
}

static inline uint8_t rand_u2(uint32_t &state) {
    return static_cast<uint8_t>(xorshift32(state) & 0x3u);
}

static inline uint8_t rand_u3(uint32_t &state) {
    return static_cast<uint8_t>(xorshift32(state) & 0x7u);
}

static inline uint8_t rand_u7(uint32_t &state) {
    return static_cast<uint8_t>(xorshift32(state) & 0x7fu);
}

static inline uint16_t rand_u11(uint32_t &state) {
    return static_cast<uint16_t>(xorshift32(state) & 0x7ffu);
}

static void set_clocks(Vct_idu_is_lsiq_entry *dut, uint8_t value) {
    dut->forever_cpuclk = value;
    dut->lq_full_clk = value;
    dut->rb_full_clk = value;
    dut->sq_full_clk = value;
    dut->tlb_busy_clk = value;
    dut->wait_fence_clk = value;
    dut->wait_old_clk = value;
}

static void tick(VerilatedContext *ctx, Vct_idu_is_lsiq_entry *dut) {
    set_clocks(dut, 0);
    dut->eval();
    ctx->timeInc(1);
    set_clocks(dut, 1);
    dut->eval();
    ctx->timeInc(1);
}

static void init_inputs(Vct_idu_is_lsiq_entry *dut) {
    set_clocks(dut, 0);
    dut->cpurst_b = 0;
    dut->cp0_idu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;

    dut->ctrl_xx_rf_pipe0_preg_lch_vld_dupx = 0;
    dut->ctrl_xx_rf_pipe1_preg_lch_vld_dupx = 0;
    dut->dp_xx_rf_pipe0_dst_preg_dupx = 0;
    dut->dp_xx_rf_pipe1_dst_preg_dupx = 0;
    dut->iu_idu_div_inst_vld = 0;
    dut->iu_idu_div_preg_dupx = 0;
    dut->iu_idu_ex2_pipe0_wb_preg_dupx = 0;
    dut->iu_idu_ex2_pipe0_wb_preg_vld_dupx = 0;
    dut->iu_idu_ex2_pipe1_mult_inst_vld_dupx = 0;
    dut->iu_idu_ex2_pipe1_preg_dupx = 0;
    dut->iu_idu_ex2_pipe1_wb_preg_dupx = 0;
    dut->iu_idu_ex2_pipe1_wb_preg_vld_dupx = 0;
    dut->lsiq_bar_mode = 0;
    dut->lsu_idu_ag_pipe3_load_inst_vld = 0;
    dut->lsu_idu_ag_pipe3_preg_dupx = 0;
    dut->lsu_idu_ag_pipe3_vload_inst_vld = 0;
    dut->lsu_idu_ag_pipe3_vreg_dupx = 0;
    dut->lsu_idu_dc_pipe3_load_fwd_inst_vld_dupx = 0;
    dut->lsu_idu_dc_pipe3_load_inst_vld_dupx = 0;
    dut->lsu_idu_dc_pipe3_preg_dupx = 0;
    dut->lsu_idu_dc_pipe3_vload_fwd_inst_vld = 0;
    dut->lsu_idu_dc_pipe3_vload_inst_vld_dupx = 0;
    dut->lsu_idu_dc_pipe3_vreg_dupx = 0;
    dut->lsu_idu_lq_not_full = 0;
    dut->lsu_idu_lsiq_pop_vld = 0;
    dut->lsu_idu_no_fence = 0;
    dut->lsu_idu_rb_not_full = 0;
    dut->lsu_idu_sq_not_full = 0;
    dut->lsu_idu_wb_pipe3_wb_preg_dupx = 0;
    dut->lsu_idu_wb_pipe3_wb_preg_vld_dupx = 0;
    dut->lsu_idu_wb_pipe3_wb_vreg_dupx = 0;
    dut->lsu_idu_wb_pipe3_wb_vreg_vld_dupx = 0;
    dut->rtu_idu_flush_fe = 0;
    dut->rtu_idu_flush_is = 0;
    dut->vfpu_idu_ex1_pipe6_mfvr_inst_vld_dupx = 0;
    dut->vfpu_idu_ex1_pipe6_preg_dupx = 0;
    dut->vfpu_idu_ex1_pipe7_mfvr_inst_vld_dupx = 0;
    dut->vfpu_idu_ex1_pipe7_preg_dupx = 0;
    dut->vfpu_idu_ex5_pipe6_wb_vreg_dupx = 0;
    dut->vfpu_idu_ex5_pipe6_wb_vreg_vld_dupx = 0;
    dut->vfpu_idu_ex5_pipe7_wb_vreg_dupx = 0;
    dut->vfpu_idu_ex5_pipe7_wb_vreg_vld_dupx = 0;
    dut->x_already_da_set = 0;
    dut->x_alu0_reg_fwd_vld = 0;
    dut->x_alu1_reg_fwd_vld = 0;
    dut->x_bkpta_data_set = 0;
    dut->x_bkptb_data_set = 0;
    dut->x_create_agevec = 0;
    dut->x_create_agevec_all = 0;
    zero_signal(dut->x_create_data);
    dut->x_create_dp_en = 0;
    dut->x_create_en = 0;
    dut->x_create_frz = 0;
    dut->x_create_gateclk_en = 0;
    dut->x_frz_clr = 0;
    dut->x_issue_en = 0;
    dut->x_lq_full_set = 0;
    dut->x_other_aft_load = 0;
    dut->x_other_aft_store = 0;
    dut->x_other_bar = 0;
    dut->x_other_frz = 0;
    dut->x_other_load = 0;
    dut->x_other_no_spec = 0;
    dut->x_other_raw_rdy = 0;
    dut->x_other_store = 0;
    dut->x_pop_cur_entry = 0;
    dut->x_pop_other_entry = 0;
    dut->x_rb_full_set = 0;
    dut->x_rdy_clr = 0;
    dut->x_spec_fail_set = 0;
    dut->x_sq_full_set = 0;
    dut->x_tlb_busy_set = 0;
    dut->x_tlb_wakeup = 0;
    dut->x_unalign_2nd_set = 0;
    dut->x_unalign_gateclk_en = 0;
    dut->x_wait_fence_set = 0;
    dut->x_wait_old_set = 0;
}

static void drive_random_inputs(Vct_idu_is_lsiq_entry *dut, uint32_t &rng) {
    dut->cp0_idu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;

    dut->ctrl_xx_rf_pipe0_preg_lch_vld_dupx = rand_bit(rng);
    dut->ctrl_xx_rf_pipe1_preg_lch_vld_dupx = rand_bit(rng);
    dut->dp_xx_rf_pipe0_dst_preg_dupx = rand_u7(rng);
    dut->dp_xx_rf_pipe1_dst_preg_dupx = rand_u7(rng);

    dut->iu_idu_div_inst_vld = rand_bit(rng);
    dut->iu_idu_div_preg_dupx = rand_u7(rng);
    dut->iu_idu_ex2_pipe0_wb_preg_dupx = rand_u7(rng);
    dut->iu_idu_ex2_pipe0_wb_preg_vld_dupx = rand_bit(rng);
    dut->iu_idu_ex2_pipe1_mult_inst_vld_dupx = rand_bit(rng);
    dut->iu_idu_ex2_pipe1_preg_dupx = rand_u7(rng);
    dut->iu_idu_ex2_pipe1_wb_preg_dupx = rand_u7(rng);
    dut->iu_idu_ex2_pipe1_wb_preg_vld_dupx = rand_bit(rng);

    dut->lsiq_bar_mode = rand_bit(rng);
    dut->lsu_idu_ag_pipe3_load_inst_vld = rand_bit(rng);
    dut->lsu_idu_ag_pipe3_preg_dupx = rand_u7(rng);
    dut->lsu_idu_ag_pipe3_vload_inst_vld = rand_bit(rng);
    dut->lsu_idu_ag_pipe3_vreg_dupx = rand_u7(rng);
    dut->lsu_idu_dc_pipe3_load_fwd_inst_vld_dupx = rand_bit(rng);
    dut->lsu_idu_dc_pipe3_load_inst_vld_dupx = rand_bit(rng);
    dut->lsu_idu_dc_pipe3_preg_dupx = rand_u7(rng);
    dut->lsu_idu_dc_pipe3_vload_fwd_inst_vld = rand_bit(rng);
    dut->lsu_idu_dc_pipe3_vload_inst_vld_dupx = rand_bit(rng);
    dut->lsu_idu_dc_pipe3_vreg_dupx = rand_u7(rng);
    dut->lsu_idu_lq_not_full = rand_bit(rng);
    dut->lsu_idu_lsiq_pop_vld = rand_bit(rng);
    dut->lsu_idu_no_fence = rand_bit(rng);
    dut->lsu_idu_rb_not_full = rand_bit(rng);
    dut->lsu_idu_sq_not_full = rand_bit(rng);
    dut->lsu_idu_wb_pipe3_wb_preg_dupx = rand_u7(rng);
    dut->lsu_idu_wb_pipe3_wb_preg_vld_dupx = rand_bit(rng);
    dut->lsu_idu_wb_pipe3_wb_vreg_dupx = rand_u7(rng);
    dut->lsu_idu_wb_pipe3_wb_vreg_vld_dupx = rand_bit(rng);

    dut->rtu_idu_flush_fe = rand_bit(rng);
    dut->rtu_idu_flush_is = rand_bit(rng);

    dut->vfpu_idu_ex1_pipe6_mfvr_inst_vld_dupx = rand_bit(rng);
    dut->vfpu_idu_ex1_pipe6_preg_dupx = rand_u7(rng);
    dut->vfpu_idu_ex1_pipe7_mfvr_inst_vld_dupx = rand_bit(rng);
    dut->vfpu_idu_ex1_pipe7_preg_dupx = rand_u7(rng);
    dut->vfpu_idu_ex5_pipe6_wb_vreg_dupx = rand_u7(rng);
    dut->vfpu_idu_ex5_pipe6_wb_vreg_vld_dupx = rand_bit(rng);
    dut->vfpu_idu_ex5_pipe7_wb_vreg_dupx = rand_u7(rng);
    dut->vfpu_idu_ex5_pipe7_wb_vreg_vld_dupx = rand_bit(rng);

    dut->x_already_da_set = rand_bit(rng);
    dut->x_alu0_reg_fwd_vld = rand_u2(rng);
    dut->x_alu1_reg_fwd_vld = rand_u2(rng);
    dut->x_bkpta_data_set = rand_bit(rng);
    dut->x_bkptb_data_set = rand_bit(rng);
    dut->x_create_agevec = rand_u11(rng);
    dut->x_create_agevec_all = rand_u11(rng);
    fill_random(dut->x_create_data, rng);
    dut->x_create_dp_en = rand_bit(rng);
    dut->x_create_en = rand_bit(rng);
    dut->x_create_frz = rand_bit(rng);
    dut->x_create_gateclk_en = rand_bit(rng);
    dut->x_frz_clr = rand_bit(rng);
    dut->x_issue_en = rand_bit(rng);
    dut->x_lq_full_set = rand_bit(rng);
    dut->x_other_aft_load = rand_u11(rng);
    dut->x_other_aft_store = rand_u11(rng);
    dut->x_other_bar = rand_u11(rng);
    dut->x_other_frz = rand_u11(rng);
    dut->x_other_load = rand_u11(rng);
    dut->x_other_no_spec = rand_u11(rng);
    dut->x_other_raw_rdy = rand_u11(rng);
    dut->x_other_store = rand_u11(rng);
    dut->x_pop_cur_entry = rand_bit(rng);
    dut->x_pop_other_entry = rand_u11(rng);
    dut->x_rb_full_set = rand_bit(rng);
    dut->x_rdy_clr = rand_u3(rng);
    dut->x_spec_fail_set = rand_bit(rng);
    dut->x_sq_full_set = rand_bit(rng);
    dut->x_tlb_busy_set = rand_bit(rng);
    dut->x_tlb_wakeup = rand_bit(rng);
    dut->x_unalign_2nd_set = rand_bit(rng);
    dut->x_unalign_gateclk_en = rand_bit(rng);
    dut->x_wait_fence_set = rand_bit(rng);
    dut->x_wait_old_set = rand_bit(rng);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_idu_is_lsiq_entry(ctx);
    init_inputs(dut);

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->cpurst_b = 1;

    uint32_t rng = 0x4d3a2f17u;
    bool ok = true;

    drive_random_inputs(dut, rng);
    dut->x_create_en = 0;
    dut->x_create_dp_en = 0;
    dut->x_create_gateclk_en = 0;
    dut->lsu_idu_lsiq_pop_vld = 0;
    dut->x_pop_cur_entry = 0;
    dut->rtu_idu_flush_fe = 0;
    dut->rtu_idu_flush_is = 0;
    tick(ctx, dut);
    if (dut->x_vld != 0) {
        std::cerr << "[TB] expected x_vld to be 0 after reset" << std::endl;
        ok = false;
    }

    drive_random_inputs(dut, rng);
    dut->x_create_en = 1;
    dut->x_create_dp_en = 1;
    dut->x_create_gateclk_en = 1;
    dut->x_create_frz = 0;
    dut->lsu_idu_lsiq_pop_vld = 0;
    dut->x_pop_cur_entry = 0;
    dut->rtu_idu_flush_fe = 0;
    dut->rtu_idu_flush_is = 0;
    tick(ctx, dut);
    if (dut->x_vld != 1) {
        std::cerr << "[TB] expected x_vld to be 1 after create" << std::endl;
        ok = false;
    }

    drive_random_inputs(dut, rng);
    dut->x_create_en = 0;
    dut->x_create_dp_en = 0;
    dut->x_create_gateclk_en = 0;
    dut->lsu_idu_lsiq_pop_vld = 1;
    dut->x_pop_cur_entry = 1;
    dut->rtu_idu_flush_fe = 0;
    dut->rtu_idu_flush_is = 0;
    tick(ctx, dut);
    if (dut->x_vld != 0) {
        std::cerr << "[TB] expected x_vld to clear on pop" << std::endl;
        ok = false;
    }

    for (int cycle = 0; cycle < 512; ++cycle) {
        drive_random_inputs(dut, rng);
        tick(ctx, dut);
        if (ctx->gotFinish()) {
            break;
        }
    }

    drive_random_inputs(dut, rng);
    dut->x_create_en = 0;
    dut->lsu_idu_lsiq_pop_vld = 0;
    dut->rtu_idu_flush_fe = 0;
    dut->rtu_idu_flush_is = 0;
    for (int i = 0; i < 4; ++i) {
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

    if (!ok) {
        return 1;
    }
    return 0;
}
