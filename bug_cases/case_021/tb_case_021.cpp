#include "Vaxi_interconnect128.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>

static void tick(VerilatedContext *ctx, Vaxi_interconnect128 *dut) {
    dut->aclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->aclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

// Helper to set VlWide<4> (128-bit) from uint64_t
static void set_rdata(VlWide<4> &rdata, uint64_t val) {
    rdata.m_storage[0] = static_cast<uint32_t>(val);
    rdata.m_storage[1] = static_cast<uint32_t>(val >> 32);
    rdata.m_storage[2] = 0;
    rdata.m_storage[3] = 0;
}

static void set_rdata_full(VlWide<4> &rdata, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    rdata.m_storage[0] = a;
    rdata.m_storage[1] = b;
    rdata.m_storage[2] = c;
    rdata.m_storage[3] = d;
}

static void clear_rdata(VlWide<4> &rdata) {
    rdata.m_storage[0] = 0;
    rdata.m_storage[1] = 0;
    rdata.m_storage[2] = 0;
    rdata.m_storage[3] = 0;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vaxi_interconnect128(ctx);

    // Initialize inputs
    dut->aresetn = 0;
    dut->araddr = 0;
    dut->arvalid = 0;
    dut->arready_s0 = 0;
    dut->arready_s1 = 0;
    dut->arready_s2 = 0;
    dut->arready_s3 = 0;
    dut->awaddr = 0;
    dut->awid = 0;
    dut->awvalid = 0;
    dut->awready_s0 = 0;
    dut->awready_s1 = 0;
    dut->awready_s2 = 0;
    dut->awready_s3 = 0;
    dut->bid_s0 = 0;
    dut->bid_s1 = 0;
    dut->bid_s2 = 0;
    dut->bid_s3 = 0;
    dut->bready = 0;
    dut->bresp_s0 = 0;
    dut->bresp_s1 = 0;
    dut->bresp_s2 = 0;
    dut->bresp_s3 = 0;
    dut->bvalid_s0 = 0;
    dut->bvalid_s1 = 0;
    dut->bvalid_s2 = 0;
    dut->bvalid_s3 = 0;
    clear_rdata(dut->rdata_s0);
    clear_rdata(dut->rdata_s1);
    clear_rdata(dut->rdata_s2);
    clear_rdata(dut->rdata_s3);
    dut->rid_s0 = 0;
    dut->rid_s1 = 0;
    dut->rid_s2 = 0;
    dut->rid_s3 = 0;
    dut->rlast_s0 = 0;
    dut->rlast_s1 = 0;
    dut->rlast_s2 = 0;
    dut->rlast_s3 = 0;
    dut->rready = 0;
    dut->rresp_s0 = 0;
    dut->rresp_s1 = 0;
    dut->rresp_s2 = 0;
    dut->rresp_s3 = 0;
    dut->rvalid_s0 = 0;
    dut->rvalid_s1 = 0;
    dut->rvalid_s2 = 0;
    dut->rvalid_s3 = 0;
    dut->wid = 0;
    dut->wlast = 0;
    dut->wready_s0 = 0;
    dut->wready_s1 = 0;
    dut->wready_s2 = 0;
    dut->wready_s3 = 0;
    dut->wvalid = 0;

    // Reset
    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->aresetn = 1;

    // Test different address ranges to exercise awsel/arsel logic
    struct AddrTest {
        uint64_t addr;
        uint8_t expected_sel;
    };
    
    const AddrTest tests[] = {
        // SRAM range: 0x0000_0000 - 0x01ff_ffff
        {0x0000000000ULL, 0x01}, {0x0000FFFFFFULL, 0x01}, {0x0001FFFFFFULL, 0x01},
        // ERR1 range: 0x0200_0000 - 0x0fff_ffff
        {0x0002000000ULL, 0x02}, {0x0005ABCDEFULL, 0x02}, {0x000FFFFFFFULL, 0x02},
        // APB range: 0x1000_0000 - 0x1fff_ffff
        {0x0010000000ULL, 0x04}, {0x0012ABCDEFULL, 0x04}, {0x001FFFFFFFULL, 0x04},
        // ERR2 range: 0x2000_0000 - 0xff_ffffffff
        {0x0020000000ULL, 0x08}, {0x0050000000ULL, 0x08}, {0x00FFFFFFFFULL, 0x08}, {0x3FFFFFFFFFULL, 0x08},
    };

    // Test AW channel address decoding for all slaves
    for (int iter = 0; iter < 2; ++iter) {
        for (const auto &t : tests) {
            dut->awaddr = t.addr & 0xFFFFFFFFFFULL;
            dut->awvalid = 1;
            dut->awid = 0x10 + iter;
            dut->awready_s0 = (t.expected_sel == 0x01);
            dut->awready_s1 = (t.expected_sel == 0x02);
            dut->awready_s2 = (t.expected_sel == 0x04);
            dut->awready_s3 = (t.expected_sel == 0x08);
            tick(ctx, dut);
            dut->awvalid = 0;
            tick(ctx, dut);
        }
    }

    // Test AR channel address decoding for all slaves
    for (int iter = 0; iter < 2; ++iter) {
        for (const auto &t : tests) {
            dut->araddr = t.addr & 0xFFFFFFFFFFULL;
            dut->arvalid = 1;
            dut->arready_s0 = (t.expected_sel == 0x01) || iter;
            dut->arready_s1 = (t.expected_sel == 0x02) || iter;
            dut->arready_s2 = (t.expected_sel == 0x04) || iter;
            dut->arready_s3 = (t.expected_sel == 0x08) || iter;
            tick(ctx, dut);
            dut->arvalid = 0;
            tick(ctx, dut);
        }
    }

    // Test W channel with stored addresses for all slaves
    // First, store addresses via AW channel
    const uint64_t w_test_addrs[] = {0x100, 0x02000000, 0x10000000, 0x20000000};
    for (int slave = 0; slave < 4; ++slave) {
        dut->awaddr = w_test_addrs[slave];
        dut->awvalid = 1;
        dut->awready_s0 = (slave == 0);
        dut->awready_s1 = (slave == 1);
        dut->awready_s2 = (slave == 2);
        dut->awready_s3 = (slave == 3);
        dut->awid = slave + 1;
        tick(ctx, dut);
        dut->awvalid = 0;
        tick(ctx, dut);

        // Now send W with matching ID
        dut->wid = slave + 1;
        dut->wvalid = 1;
        dut->wlast = 1;
        dut->wready_s0 = (slave == 0);
        dut->wready_s1 = (slave == 1);
        dut->wready_s2 = (slave == 2);
        dut->wready_s3 = (slave == 3);
        tick(ctx, dut);
        dut->wvalid = 0;
        dut->wlast = 0;
        tick(ctx, dut);
    }

    // Test W channel with non-matching IDs
    dut->wid = 0;
    dut->wvalid = 1;
    dut->wlast = 1;
    dut->wready_s0 = 1;
    tick(ctx, dut);
    dut->wvalid = 0;
    dut->wlast = 0;
    tick(ctx, dut);

    dut->wid = 0xFF;
    dut->wvalid = 1;
    dut->wlast = 1;
    tick(ctx, dut);
    dut->wvalid = 0;
    dut->wlast = 0;
    tick(ctx, dut);

    // Test R channel for all slaves with various patterns
    for (int slave = 0; slave < 4; ++slave) {
        dut->rvalid_s0 = (slave == 0);
        dut->rvalid_s1 = (slave == 1);
        dut->rvalid_s2 = (slave == 2);
        dut->rvalid_s3 = (slave == 3);
        dut->rid_s0 = slave + 1;
        dut->rid_s1 = slave + 1;
        dut->rid_s2 = slave + 1;
        dut->rid_s3 = slave + 1;
        set_rdata_full(dut->rdata_s0, 0x11111111 * (slave + 1), 0x22222222 * (slave + 1), 
                       0x33333333 * (slave + 1), 0x44444444 * (slave + 1));
        set_rdata_full(dut->rdata_s1, 0x55555555 * (slave + 1), 0x66666666 * (slave + 1),
                       0x77777777 * (slave + 1), 0x88888888 * (slave + 1));
        set_rdata_full(dut->rdata_s2, 0x99999999 * (slave + 1), 0xAAAAAAAA * (slave + 1),
                       0xBBBBBBBB * (slave + 1), 0xCCCCCCCC * (slave + 1));
        set_rdata_full(dut->rdata_s3, 0xDDDDDDDD * (slave + 1), 0xEEEEEEEE * (slave + 1),
                       0xFFFFFFFF * (slave + 1), 0x12345678 * (slave + 1));
        dut->rresp_s0 = 0;
        dut->rresp_s1 = 0;
        dut->rresp_s2 = 0;
        dut->rresp_s3 = 0;
        dut->rlast_s0 = 1;
        dut->rlast_s1 = 1;
        dut->rlast_s2 = 1;
        dut->rlast_s3 = 1;
        dut->rready = 1;
        tick(ctx, dut);
    }
    dut->rvalid_s0 = 0;
    dut->rvalid_s1 = 0;
    dut->rvalid_s2 = 0;
    dut->rvalid_s3 = 0;
    tick(ctx, dut);

    // Test B channel for all slaves
    for (int slave = 0; slave < 4; ++slave) {
        dut->bvalid_s0 = (slave == 0);
        dut->bvalid_s1 = (slave == 1);
        dut->bvalid_s2 = (slave == 2);
        dut->bvalid_s3 = (slave == 3);
        dut->bid_s0 = slave + 1;
        dut->bid_s1 = slave + 1;
        dut->bid_s2 = slave + 1;
        dut->bid_s3 = slave + 1;
        dut->bresp_s0 = 0;
        dut->bresp_s1 = 0;
        dut->bresp_s2 = 0;
        dut->bresp_s3 = 0;
        dut->bready = 1;
        tick(ctx, dut);
    }
    dut->bvalid_s0 = 0;
    dut->bvalid_s1 = 0;
    dut->bvalid_s2 = 0;
    dut->bvalid_s3 = 0;
    tick(ctx, dut);

    // Test simultaneous valid signals for arbitration
    dut->rvalid_s0 = 1;
    dut->rvalid_s1 = 1;
    dut->rvalid_s2 = 1;
    dut->rvalid_s3 = 1;
    dut->rid_s0 = 1;
    dut->rid_s1 = 2;
    dut->rid_s2 = 3;
    dut->rid_s3 = 4;
    tick(ctx, dut);
    
    // Release one by one
    dut->rvalid_s0 = 0;
    tick(ctx, dut);
    dut->rvalid_s1 = 0;
    tick(ctx, dut);
    dut->rvalid_s2 = 0;
    tick(ctx, dut);
    dut->rvalid_s3 = 0;
    tick(ctx, dut);

    // Run some additional cycles
    for (int i = 0; i < 20; ++i) {
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
    
    std::cout << "[TB] Test completed" << std::endl;
    return 0;
}
