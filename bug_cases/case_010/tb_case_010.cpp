#include "Vcase_010_top.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdlib>
#include <iostream>

static void tick(VerilatedContext *ctx, Vcase_010_top *dut) {
    dut->cpu_clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->cpu_clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void init_inputs(Vcase_010_top *dut) {
    dut->cpu_clk = 0;
    dut->pad_cpu_rst_b = 0;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vcase_010_top(ctx);
    init_inputs(dut);

    for (int i = 0; i < 2; ++i) {
        tick(ctx, dut);
    }

    if (dut->pmu_cpu_pwr_on != 0) {
        std::cerr << "[TB] pmu_cpu_pwr_on should be 0 during reset" << std::endl;
        return 1;
    }

    dut->pad_cpu_rst_b = 1;
    tick(ctx, dut);
    if (dut->pmu_cpu_pwr_on != 1) {
        std::cerr << "[TB] pmu_cpu_pwr_on should toggle high after reset" << std::endl;
        return 1;
    }

    tick(ctx, dut);
    if (dut->pmu_cpu_pwr_on != 0) {
        std::cerr << "[TB] pmu_cpu_pwr_on should toggle low on next cycle" << std::endl;
        return 1;
    }

    dut->pad_cpu_rst_b = 0;
    tick(ctx, dut);
    if (dut->pmu_cpu_pwr_on != 0) {
        std::cerr << "[TB] pmu_cpu_pwr_on should be 0 after reset reassert" << std::endl;
        return 1;
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
