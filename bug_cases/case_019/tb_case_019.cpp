#include "Vct_l2c_data.h"
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

static inline uint8_t rand_u4(uint32_t &state) {
    return static_cast<uint8_t>(xorshift32(state) & 0xfu);
}

static inline uint16_t rand_u13(uint32_t &state) {
    return static_cast<uint16_t>(xorshift32(state) & 0x1fffu);
}

static inline uint32_t rand_u5(uint32_t &state) {
    return static_cast<uint32_t>(xorshift32(state) & 0x1fu);
}

static inline uint64_t rand_u33(uint32_t &state) {
    uint64_t hi = static_cast<uint64_t>(xorshift32(state) & 0x1u);
    uint64_t lo = static_cast<uint64_t>(xorshift32(state));
    return (hi << 32) | lo;
}

static void set_clocks(Vct_l2c_data *dut, uint8_t value) {
    dut->forever_cpuclk = value;
    dut->l2c_clk = value;
}

static void tick(VerilatedContext *ctx, Vct_l2c_data *dut) {
    set_clocks(dut, 0);
    dut->eval();
    ctx->timeInc(1);
    set_clocks(dut, 1);
    dut->eval();
    ctx->timeInc(1);
}

static void init_inputs(Vct_l2c_data *dut) {
    set_clocks(dut, 0);
    dut->cpurst_b = 0;
    dut->l2c_icg_en = 1;
    dut->pad_yy_icg_scan_en = 0;

    dut->ciu_l2c_data_acc_cycle = 0;
    dut->ciu_l2c_data_setup = 0;
    zero_signal(dut->cmp_data_din);
    dut->cmp_data_index = 0;
    dut->cmp_data_req = 0;
    dut->cmp_data_req_gate = 0;
    dut->cmp_data_wen = 0;
    dut->cmp_stage_addr = 0;
    dut->cmp_stage_cp = 0;
    dut->cmp_stage_resp = 0;
    dut->cmp_stage_sid = 0;
    dut->cmp_stage_src = 0;
    dut->cmp_stage_write = 0;
    dut->icc_data_cen = 0;
    dut->icc_data_flop = 0;
    dut->icc_data_index = 0;
    dut->icc_data_req = 0;
    zero_signal(dut->l2c_data_dout);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_l2c_data(ctx);
    init_inputs(dut);

    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    dut->cpurst_b = 1;
    tick(ctx, dut);

    dut->cmp_data_req = 0;
    dut->cmp_data_req_gate = 0;
    tick(ctx, dut);
    if (dut->l2c_data_ram_clk_en_x != 0) {
        std::cerr << "[TB] expected l2c_data_ram_clk_en_x low with no request" << std::endl;
        return 1;
    }

    dut->cmp_data_req = 1;
    dut->cmp_data_req_gate = 1;
    dut->icc_data_cen = 0;
    dut->ciu_l2c_data_setup = 0;
    tick(ctx, dut);
    if (dut->l2c_data_ram_clk_en_x != 1) {
        std::cerr << "[TB] expected l2c_data_ram_clk_en_x high with cmp_data_req" << std::endl;
        return 1;
    }

    uint32_t rng = 0x12345678u;
    for (int i = 0; i < 64; ++i) {
        dut->ciu_l2c_data_acc_cycle = rand_u4(rng);
        dut->ciu_l2c_data_setup = rand_bit(rng);
        fill_random(dut->cmp_data_din, rng);
        dut->cmp_data_index = rand_u13(rng);
        dut->cmp_data_req = rand_bit(rng);
        dut->cmp_data_req_gate = dut->cmp_data_req;
        dut->cmp_data_wen = rand_bit(rng);
        dut->cmp_stage_addr = rand_u33(rng);
        dut->cmp_stage_cp = rand_u4(rng);
        dut->cmp_stage_resp = rand_u5(rng);
        dut->cmp_stage_sid = rand_u5(rng);
        dut->cmp_stage_src = static_cast<uint8_t>(xorshift32(rng) & 0x3u);
        dut->cmp_stage_write = rand_bit(rng);
        dut->icc_data_cen = rand_u5(rng);
        dut->icc_data_flop = rand_bit(rng);
        dut->icc_data_index = rand_u13(rng);
        dut->icc_data_req = rand_bit(rng);
        fill_random(dut->l2c_data_dout, rng);
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
