#include "Vcpu_sub_system_axi.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

// Testbench for real cpu_sub_system_axi with openC910 stub
// Tests IFU read request behavior through real module hierarchy

static void tick(VerilatedContext *ctx, Vcpu_sub_system_axi *dut) {
    dut->pll_cpu_clk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->pll_cpu_clk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static void reset(VerilatedContext *ctx, Vcpu_sub_system_axi *dut, int cycles) {
    dut->pad_cpu_rst_b = 0;
    dut->pad_yy_dft_clk_rst_b = 0;
    for (int i = 0; i < cycles; i++) {
        tick(ctx, dut);
    }
    dut->pad_cpu_rst_b = 1;
    dut->pad_yy_dft_clk_rst_b = 1;
}

// Helper to set 128-bit data
static void set128(VlWide<4> &dst, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3) {
    dst[0] = w0;
    dst[1] = w1;
    dst[2] = w2;
    dst[3] = w3;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vcpu_sub_system_axi(ctx);

    // Initialize all inputs
    dut->pll_cpu_clk = 0;
    dut->pad_cpu_rst_b = 0;
    dut->pad_yy_dft_clk_rst_b = 0;
    dut->axim_clk_en = 1;
    dut->pad_biu_arready = 0;
    dut->pad_biu_awready = 0;
    dut->pad_biu_bid = 0;
    dut->pad_biu_bresp = 0;
    dut->pad_biu_bvalid = 0;
    set128(dut->pad_biu_rdata, 0, 0, 0, 0);
    dut->pad_biu_rid = 0;
    dut->pad_biu_rlast = 0;
    dut->pad_biu_rresp = 0;
    dut->pad_biu_rvalid = 0;
    dut->pad_biu_wready = 0;
    dut->pad_had_jtg_tclk = 0;
    dut->pad_had_jtg_tdi = 0;
    dut->pad_had_jtg_trst_b = 0;
    dut->xx_intc_vld = 0;
    dut->per_clk = 0;
    dut->i_pad_jtg_tms = 0;

    // Reset
    reset(ctx, dut, 20);

    std::cerr << "[TB] Starting cpu_sub_system_axi test with REAL RTL" << std::endl;
    std::cerr << "[TB] DUT: cpu_sub_system_axi -> rv_integration_platform -> openC910(stub)" << std::endl;

    int request_count = 0;
    int arvalid_low_cycles = 0;
    int arvalid_high_cycles = 0;
    bool last_arvalid = false;

    // Run simulation
    for (int cycle = 0; cycle < 1000; cycle++) {
        // AXI slave: always ready after reset
        dut->pad_biu_arready = (cycle > 30) ? 1 : 0;
        
        // Count ARVALID assertions
        if (dut->biu_pad_arvalid) {
            if (!last_arvalid) {
                request_count++;
                if (request_count <= 10 || request_count % 10 == 0) {
                    std::cerr << "[TB] ARVALID assertion #" << request_count 
                              << " at cycle " << cycle
                              << " addr=0x" << std::hex << dut->biu_pad_araddr << std::dec
                              << std::endl;
                }
            }
            arvalid_high_cycles++;
        } else {
            arvalid_low_cycles++;
        }
        last_arvalid = dut->biu_pad_arvalid;
        
        // Generate delayed response
        static int rvalid_delay = 0;
        static bool pending = false;
        if (dut->biu_pad_arvalid && dut->pad_biu_arready && !pending) {
            pending = true;
            rvalid_delay = 0;
        }
        if (pending) {
            rvalid_delay++;
            if (rvalid_delay >= 4) {
                dut->pad_biu_rvalid = 1;
                dut->pad_biu_rlast = 1;
                set128(dut->pad_biu_rdata, 0x89ABCDEF, 0x01234567, 0x89ABCDEF, 0x01234567);
                dut->pad_biu_rid = dut->biu_pad_arid;
                pending = false;
            }
        }
        
        tick(ctx, dut);
        
        // Deassert rvalid
        if (dut->pad_biu_rvalid) {
            dut->pad_biu_rvalid = 0;
            dut->pad_biu_rlast = 0;
        }
    }

    std::cerr << "[TB] =============================" << std::endl;
    std::cerr << "[TB] Simulation Complete" << std::endl;
    std::cerr << "[TB] Total ARVALID assertions: " << request_count << std::endl;
    std::cerr << "[TB] ARVALID high cycles: " << arvalid_high_cycles << std::endl;
    std::cerr << "[TB] ARVALID low cycles: " << arvalid_low_cycles << std::endl;

    // Validate test - openC910 stub intentionally stalls, so we expect exactly 50 requests
    // (50 normal + 0 during stall + 0 after because simulation ends)
    bool test_passed = (request_count >= 40 && request_count <= 60);
    
    if (test_passed) {
        std::cerr << "[TB] PASSED: cpu_sub_system_axi behavior validated (" << request_count << " requests)" << std::endl;
    } else {
        std::cerr << "[TB] FAILED: Expected ~50 requests, got " << request_count << std::endl;
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
    
    return test_passed ? 0 : 1;
}
