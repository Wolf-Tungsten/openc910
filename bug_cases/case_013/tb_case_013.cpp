#include "Vct_vfmau_mult1.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

static void tick(VerilatedContext *ctx, Vct_vfmau_mult1 *dut) {
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void set_wide68(void *dst, uint64_t lo, uint32_t hi) {
    auto *words = reinterpret_cast<uint32_t *>(dst);
    words[0] = static_cast<uint32_t>(lo);
    words[1] = static_cast<uint32_t>(lo >> 32);
    words[2] = hi & 0xF;
}

static bool buffer_equal(const void *lhs, const void *rhs, size_t size) {
    return std::memcmp(lhs, rhs, size) == 0;
}

static void clear_inputs(Vct_vfmau_mult1 *dut) {
    dut->cp0_vfpu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->cpurst_b = 0;
    dut->dp_mult1_ex1_clk_en = 1;
    dut->dp_mult1_ex2_clk_en = 1;
    dut->dp_mult1_ex3_clk_en = 1;
    dut->dp_mult1_ex4_clk_en = 1;
    dut->dp_mult1_ex1_op0_slicex = 0;
    dut->dp_mult1_ex1_op0_slicex_high = 0;
    dut->dp_mult1_ex1_op1_slicex = 0;
    dut->dp_mult1_ex1_op1_slicex_high = 0;
    dut->dp_mult1_ex1_op2_slicex = 0;
    dut->dp_mult1_ex1_op2_slicex_high = 0;
    dut->dp_mult1_op2_slicex_vl_mask = 0;
    dut->dp_mult1_op2_slicex_vm_mask = 0;
    dut->dp_mult_ex1_op0_slicex_half0 = 0;
    dut->dp_mult_ex1_op0_slicex_half0_high = 0;
    dut->dp_mult_ex1_op1_slicex_half0 = 0;
    dut->dp_mult_ex1_op1_slicex_half0_high = 0;
    dut->dp_mult_ex1_op2_slicex_half0 = 0;
    dut->dp_mult_ex1_op2_slicex_half0_high = 0;
    dut->dp_mult_op2_slicex_vl_half0_mask = 0;
    dut->dp_mult_op2_slicex_vm_half0_mask = 0;
    dut->dp_xx_ex1_double = 0;
    dut->dp_xx_ex1_fma = 0;
    dut->dp_xx_ex1_half = 0;
    dut->dp_xx_ex1_neg = 0;
    dut->dp_xx_ex1_op0_frac = 0;
    dut->dp_xx_ex1_op1_frac = 0;
    dut->dp_xx_ex1_rm = 0;
    dut->dp_xx_ex1_simd = 0;
    dut->dp_xx_ex1_single = 0;
    dut->dp_xx_ex1_sub = 0;
    dut->dp_xx_ex1_widen = 0;
    dut->dp_xx_ex2_double = 0;
    dut->dp_xx_ex2_fma = 0;
    dut->dp_xx_ex2_half = 0;
    dut->dp_xx_ex2_mult_id = 0;
    dut->dp_xx_ex2_neg = 0;
    dut->dp_xx_ex2_rm = 0;
    dut->dp_xx_ex2_simd = 0;
    dut->dp_xx_ex2_sub = 0;
    dut->dp_xx_ex2_widen = 0;
    dut->dp_xx_ex3_double = 0;
    dut->dp_xx_ex3_fma = 0;
    dut->dp_xx_ex3_half = 0;
    dut->dp_xx_ex3_mult_id = 0;
    dut->dp_xx_ex3_rm = 0;
    dut->dp_xx_ex3_simd = 0;
    dut->dp_xx_ex3_widen = 0;
    dut->dp_xx_ex4_double = 0;
    dut->dp_xx_ex4_half = 0;
    dut->dp_xx_ex4_rm = 0;
    dut->mult1_ex1_ex2_pipedown = 0;
    dut->mult1_ex2_ex3_pipedown = 0;
    dut->mult1_ex3_ex4_pipedown = 0;
    dut->mult1_ex4_ex5_pipedown = 0;
    dut->mult_ex1_ex2_half_pipedown = 0;
    dut->mult_ex2_ex3_half_pipedown = 0;
    dut->mult_ex3_ex4_half_pipedown = 0;
    dut->mult_ex4_ex5_half_pipedown = 0;
    dut->pad_yy_icg_scan_en = 0;
    dut->pipe6_pipex_ex4_fmla_fwd_vld = 0;
    dut->pipe6_pipex_ex5_ex1_fmla_fwd_vld = 0;
    dut->pipe6_pipex_ex5_ex2_fmla_fwd_vld = 0;
    dut->pipe6_vfmau_ex4_fmla_slicex_half0_data = 0;
    std::memset(&dut->pipe6_vfmau_ex5_fmla_slicex_data, 0,
                sizeof(dut->pipe6_vfmau_ex5_fmla_slicex_data));
    dut->pipe7_pipex_ex4_fmla_fwd_vld = 0;
    dut->pipe7_pipex_ex5_ex1_fmla_fwd_vld = 0;
    dut->pipe7_pipex_ex5_ex2_fmla_fwd_vld = 0;
    dut->pipe7_vfmau_ex4_fmla_slicex_half0_data = 0;
    std::memset(&dut->pipe7_vfmau_ex5_fmla_slicex_data, 0,
                sizeof(dut->pipe7_vfmau_ex5_fmla_slicex_data));
    dut->vfpu_yy_xx_dqnan = 0;
}

static uint64_t next_lfsr(uint64_t &state) {
    const uint64_t lsb = state & 1u;
    state >>= 1u;
    if (lsb) {
        state ^= 0xD800000000000000ULL;
    }
    return state;
}

static void drive_random_inputs(Vct_vfmau_mult1 *dut, uint64_t &state) {
    uint64_t v0 = next_lfsr(state);
    uint64_t v1 = next_lfsr(state);
    uint64_t v2 = next_lfsr(state);
    uint64_t v3 = next_lfsr(state);
    uint64_t v4 = next_lfsr(state);

    dut->dp_mult1_ex1_op0_slicex = v0;
    dut->dp_mult1_ex1_op1_slicex = v1;
    dut->dp_mult1_ex1_op2_slicex = v2;
    dut->dp_mult1_ex1_op0_slicex_high = static_cast<uint32_t>(v3);
    dut->dp_mult1_ex1_op1_slicex_high = static_cast<uint32_t>(v4);
    dut->dp_mult1_ex1_op2_slicex_high = static_cast<uint32_t>(next_lfsr(state));

    dut->dp_mult_ex1_op0_slicex_half0 = static_cast<uint16_t>(next_lfsr(state));
    dut->dp_mult_ex1_op1_slicex_half0 = static_cast<uint16_t>(next_lfsr(state));
    dut->dp_mult_ex1_op2_slicex_half0 = static_cast<uint32_t>(next_lfsr(state));
    dut->dp_mult_ex1_op0_slicex_half0_high =
        static_cast<uint64_t>(next_lfsr(state)) & 0xFFFFFFFFFFFFULL;
    dut->dp_mult_ex1_op1_slicex_half0_high =
        static_cast<uint64_t>(next_lfsr(state)) & 0xFFFFFFFFFFFFULL;
    dut->dp_mult_ex1_op2_slicex_half0_high =
        static_cast<uint64_t>(next_lfsr(state)) & 0xFFFFFFFFFFFFULL;

    dut->dp_mult1_op2_slicex_vl_mask = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_mult1_op2_slicex_vm_mask = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_mult_op2_slicex_vl_half0_mask = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_mult_op2_slicex_vm_half0_mask = static_cast<uint8_t>(next_lfsr(state) & 1u);

    dut->dp_xx_ex1_double = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_fma = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_half = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_neg = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_op0_frac = static_cast<uint64_t>(next_lfsr(state)) & 0xFFFFFFFFFFFFFULL;
    dut->dp_xx_ex1_op1_frac = static_cast<uint64_t>(next_lfsr(state)) & 0xFFFFFFFFFFFFFULL;
    dut->dp_xx_ex1_rm = static_cast<uint8_t>(next_lfsr(state) & 0x7u);
    dut->dp_xx_ex1_simd = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_single = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_sub = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex1_widen = static_cast<uint8_t>(next_lfsr(state) & 1u);

    dut->dp_xx_ex2_double = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_fma = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_half = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_mult_id = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_neg = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_rm = static_cast<uint8_t>(next_lfsr(state) & 0x7u);
    dut->dp_xx_ex2_simd = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_sub = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex2_widen = static_cast<uint8_t>(next_lfsr(state) & 1u);

    dut->dp_xx_ex3_double = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex3_fma = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex3_half = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex3_mult_id = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex3_rm = static_cast<uint8_t>(next_lfsr(state) & 0x7u);
    dut->dp_xx_ex3_simd = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex3_widen = static_cast<uint8_t>(next_lfsr(state) & 1u);

    dut->dp_xx_ex4_double = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex4_half = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->dp_xx_ex4_rm = static_cast<uint8_t>(next_lfsr(state) & 0x7u);

    dut->pipe6_pipex_ex4_fmla_fwd_vld = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->pipe6_pipex_ex5_ex1_fmla_fwd_vld = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->pipe6_pipex_ex5_ex2_fmla_fwd_vld = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->pipe6_vfmau_ex4_fmla_slicex_half0_data = static_cast<uint16_t>(next_lfsr(state));
    set_wide68(&dut->pipe6_vfmau_ex5_fmla_slicex_data, next_lfsr(state),
               static_cast<uint32_t>(next_lfsr(state)));

    dut->pipe7_pipex_ex4_fmla_fwd_vld = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->pipe7_pipex_ex5_ex1_fmla_fwd_vld = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->pipe7_pipex_ex5_ex2_fmla_fwd_vld = static_cast<uint8_t>(next_lfsr(state) & 1u);
    dut->pipe7_vfmau_ex4_fmla_slicex_half0_data = static_cast<uint16_t>(next_lfsr(state));
    set_wide68(&dut->pipe7_vfmau_ex5_fmla_slicex_data, next_lfsr(state),
               static_cast<uint32_t>(next_lfsr(state)));

    dut->vfpu_yy_xx_dqnan = static_cast<uint8_t>(next_lfsr(state) & 1u);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_vfmau_mult1(ctx);

    clear_inputs(dut);
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    dut->cpurst_b = 1;
    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
    }

    const uint8_t ex3_mult_id_half0 = dut->slicex_dp_half0_mult_id;
    const uint8_t ex3_mult_id = dut->slicex_dp_mult1_mult_id;
    uint8_t ex3_mult_expt_snapshot[sizeof(dut->slicex_mult1_dp_ex3_mult_expt)];
    uint8_t ex3_mult_result_snapshot[sizeof(dut->slicex_mult1_dp_ex3_mult_result)];
    uint8_t ex4_expt_snapshot[sizeof(dut->slicex_mult1_dp_ex4_expt)];
    uint8_t ex4_half_fma_snapshot[sizeof(dut->slicex_mult1_dp_ex4_half_fma_result)];
    uint8_t ex4_mult_result_snapshot[sizeof(dut->slicex_mult1_dp_ex4_mult_result)];
    uint8_t ex5_fma_expt_snapshot[sizeof(dut->slicex_mult1_dp_ex5_fma_expt)];
    uint8_t ex5_fma_result_snapshot[sizeof(dut->slicex_mult1_dp_ex5_fma_result)];
    uint8_t ex5_fwd_data_snapshot[sizeof(dut->slicex_mult1_dp_ex5_fwd_data)];

    std::memcpy(ex3_mult_expt_snapshot, &dut->slicex_mult1_dp_ex3_mult_expt,
                sizeof(ex3_mult_expt_snapshot));
    std::memcpy(ex3_mult_result_snapshot, &dut->slicex_mult1_dp_ex3_mult_result,
                sizeof(ex3_mult_result_snapshot));
    std::memcpy(ex4_expt_snapshot, &dut->slicex_mult1_dp_ex4_expt,
                sizeof(ex4_expt_snapshot));
    std::memcpy(ex4_half_fma_snapshot, &dut->slicex_mult1_dp_ex4_half_fma_result,
                sizeof(ex4_half_fma_snapshot));
    std::memcpy(ex4_mult_result_snapshot, &dut->slicex_mult1_dp_ex4_mult_result,
                sizeof(ex4_mult_result_snapshot));
    std::memcpy(ex5_fma_expt_snapshot, &dut->slicex_mult1_dp_ex5_fma_expt,
                sizeof(ex5_fma_expt_snapshot));
    std::memcpy(ex5_fma_result_snapshot, &dut->slicex_mult1_dp_ex5_fma_result,
                sizeof(ex5_fma_result_snapshot));
    std::memcpy(ex5_fwd_data_snapshot, &dut->slicex_mult1_dp_ex5_fwd_data,
                sizeof(ex5_fwd_data_snapshot));

    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
        if (dut->slicex_dp_half0_mult_id != ex3_mult_id_half0 ||
            dut->slicex_dp_mult1_mult_id != ex3_mult_id) {
            std::cerr << "[TB] mult_id outputs changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex3_mult_expt_snapshot, &dut->slicex_mult1_dp_ex3_mult_expt,
                          sizeof(ex3_mult_expt_snapshot))) {
            std::cerr << "[TB] ex3 mult expt changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex3_mult_result_snapshot, &dut->slicex_mult1_dp_ex3_mult_result,
                          sizeof(ex3_mult_result_snapshot))) {
            std::cerr << "[TB] ex3 mult result changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex4_expt_snapshot, &dut->slicex_mult1_dp_ex4_expt,
                          sizeof(ex4_expt_snapshot))) {
            std::cerr << "[TB] ex4 expt changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex4_half_fma_snapshot, &dut->slicex_mult1_dp_ex4_half_fma_result,
                          sizeof(ex4_half_fma_snapshot))) {
            std::cerr << "[TB] ex4 fma result changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex4_mult_result_snapshot, &dut->slicex_mult1_dp_ex4_mult_result,
                          sizeof(ex4_mult_result_snapshot))) {
            std::cerr << "[TB] ex4 mult result changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex5_fma_expt_snapshot, &dut->slicex_mult1_dp_ex5_fma_expt,
                          sizeof(ex5_fma_expt_snapshot))) {
            std::cerr << "[TB] ex5 fma expt changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex5_fma_result_snapshot, &dut->slicex_mult1_dp_ex5_fma_result,
                          sizeof(ex5_fma_result_snapshot))) {
            std::cerr << "[TB] ex5 fma result changed without pipedown" << std::endl;
            return 1;
        }
        if (!buffer_equal(ex5_fwd_data_snapshot, &dut->slicex_mult1_dp_ex5_fwd_data,
                          sizeof(ex5_fwd_data_snapshot))) {
            std::cerr << "[TB] ex5 fwd data changed without pipedown" << std::endl;
            return 1;
        }
    }

    dut->mult1_ex1_ex2_pipedown = 1;
    dut->mult1_ex2_ex3_pipedown = 1;
    dut->mult1_ex3_ex4_pipedown = 1;
    dut->mult1_ex4_ex5_pipedown = 1;
    dut->mult_ex1_ex2_half_pipedown = 1;
    dut->mult_ex2_ex3_half_pipedown = 1;
    dut->mult_ex3_ex4_half_pipedown = 1;
    dut->mult_ex4_ex5_half_pipedown = 1;

    uint64_t lfsr = 0x9E3779B97F4A7C15ULL;
    for (int i = 0; i < 200; ++i) {
        drive_random_inputs(dut, lfsr);
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
