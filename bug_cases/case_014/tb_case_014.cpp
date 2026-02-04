#include "Vplic_32to1_arb.h"
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

template <std::size_t W>
static inline void fill_random_sparse(VlWide<W> &signal, uint32_t &state) {
    for (std::size_t i = 0; i < W; ++i) {
        const uint32_t v0 = xorshift32(state);
        const uint32_t v1 = xorshift32(state);
        signal[i] = v0 & v1;
    }
    const std::size_t word = (state >> 5) % W;
    const uint32_t bit = 1u << (state & 31u);
    signal[word] |= bit;
}

template <std::size_t W>
static inline void fill_pattern(VlWide<W> &signal, uint32_t value) {
    for (std::size_t i = 0; i < W; ++i) {
        signal[i] = value;
    }
}

static void tick(VerilatedContext *ctx, Vplic_32to1_arb *dut) {
    dut->arb_clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->arb_clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void init_inputs(Vplic_32to1_arb *dut) {
    dut->arb_clk = 0;
    dut->plicrst_b = 0;
    dut->int_select_round = 0;
    dut->ctrl_arb_new_arb_start = 0;
    zero_signal(dut->int_in_req);
    zero_signal(dut->int_in_prio);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vplic_32to1_arb(ctx);
    init_inputs(dut);

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->plicrst_b = 1;

    // Sanity: no requests -> no output request.
    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
        if (dut->int_out_req != 0) {
            std::cerr << "[TB] expected no interrupt request after reset" << std::endl;
            return 1;
        }
    }

    // Drive a wide range of request/priority patterns to exercise selection logic.
    uint32_t rng = 0x42f00d13u;
    for (int cycle = 0; cycle < 512; ++cycle) {
        const int phase = cycle % 128;
        if (phase == 0) {
            zero_signal(dut->int_in_req);
            zero_signal(dut->int_in_prio);
        } else if (phase == 32) {
            fill_pattern(dut->int_in_req, 0xffffffffu);
            fill_random(dut->int_in_prio, rng);
        } else if (phase == 64) {
            fill_pattern(dut->int_in_req, 0xaaaaaaaau);
            fill_random(dut->int_in_prio, rng);
        } else if (phase == 96) {
            fill_pattern(dut->int_in_req, 0x55555555u);
            fill_random(dut->int_in_prio, rng);
        } else if (phase == 48) {
            fill_random(dut->int_in_req, rng);
            fill_random(dut->int_in_prio, rng);
        } else {
            fill_random_sparse(dut->int_in_req, rng);
            fill_random(dut->int_in_prio, rng);
        }

        dut->int_select_round = static_cast<uint8_t>((cycle + (rng & 0x1f)) & 0x1f);
        dut->ctrl_arb_new_arb_start = (cycle % 16) == 0;

        if (cycle == 128) {
            dut->plicrst_b = 0;
        }
        if (cycle == 132) {
            dut->plicrst_b = 1;
        }

        tick(ctx, dut);
    }

    // Drop all requests and let the pipeline settle.
    zero_signal(dut->int_in_req);
    zero_signal(dut->int_in_prio);
    dut->ctrl_arb_new_arb_start = 1;
    tick(ctx, dut);
    dut->ctrl_arb_new_arb_start = 0;

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
