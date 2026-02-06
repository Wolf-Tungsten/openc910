#include "Vaxi_slave128.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

static void tick(VerilatedContext *ctx, Vaxi_slave128 *dut) {
    dut->pll_core_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->pll_core_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static inline uint32_t line_addr(uint64_t addr) {
    return static_cast<uint32_t>((addr >> 4) & 0x1fffffU);
}

static inline void wide_from_bytes(VlWide<4> &signal, const std::array<uint8_t, 16> &bytes) {
    for (std::size_t word = 0; word < 4; ++word) {
        const std::size_t base = word * 4;
        uint32_t value = 0;
        value |= static_cast<uint32_t>(bytes[base + 0]);
        value |= static_cast<uint32_t>(bytes[base + 1]) << 8;
        value |= static_cast<uint32_t>(bytes[base + 2]) << 16;
        value |= static_cast<uint32_t>(bytes[base + 3]) << 24;
        signal[word] = value;
    }
}

static inline std::array<uint8_t, 16> bytes_from_wide(const VlWide<4> &signal) {
    std::array<uint8_t, 16> bytes{};
    for (std::size_t word = 0; word < 4; ++word) {
        const uint32_t value = signal[word];
        const std::size_t base = word * 4;
        bytes[base + 0] = static_cast<uint8_t>(value & 0xffu);
        bytes[base + 1] = static_cast<uint8_t>((value >> 8) & 0xffu);
        bytes[base + 2] = static_cast<uint8_t>((value >> 16) & 0xffu);
        bytes[base + 3] = static_cast<uint8_t>((value >> 24) & 0xffu);
    }
    return bytes;
}

static inline std::array<uint8_t, 16> pattern_for_addr(uint32_t addr) {
    std::array<uint8_t, 16> bytes{};
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        bytes[i] = static_cast<uint8_t>((addr * 7u + i * 13u) & 0xffu);
    }
    return bytes;
}

static inline void apply_wstrb(std::array<uint8_t, 16> &dst,
                               const std::array<uint8_t, 16> &src,
                               uint16_t wstrb) {
    for (std::size_t i = 0; i < dst.size(); ++i) {
        if (wstrb & (1u << i)) {
            dst[i] = src[i];
        }
    }
}

static bool wait_for_rvalid(VerilatedContext *ctx, Vaxi_slave128 *dut, int max_cycles) {
    for (int i = 0; i < max_cycles; ++i) {
        if (dut->rvalid_s0) {
            return true;
        }
        tick(ctx, dut);
    }
    return false;
}

static bool wait_for_bvalid(VerilatedContext *ctx, Vaxi_slave128 *dut, int max_cycles) {
    for (int i = 0; i < max_cycles; ++i) {
        if (dut->bvalid_s0) {
            return true;
        }
        tick(ctx, dut);
    }
    return false;
}

static bool wait_for_arready(VerilatedContext *ctx, Vaxi_slave128 *dut, int max_cycles) {
    for (int i = 0; i < max_cycles; ++i) {
        dut->pll_core_cpuclk = 0;
        dut->eval();
        if (dut->arready_s0) {
            return true;
        }
        tick(ctx, dut);
    }
    return false;
}

static bool wait_for_awready(VerilatedContext *ctx, Vaxi_slave128 *dut, int max_cycles) {
    for (int i = 0; i < max_cycles; ++i) {
        dut->pll_core_cpuclk = 0;
        dut->eval();
        if (dut->awready_s0) {
            return true;
        }
        tick(ctx, dut);
    }
    return false;
}

static bool wait_for_wready(VerilatedContext *ctx, Vaxi_slave128 *dut, int max_cycles) {
    for (int i = 0; i < max_cycles; ++i) {
        dut->pll_core_cpuclk = 0;
        dut->eval();
        if (dut->wready_s0) {
            return true;
        }
        tick(ctx, dut);
    }
    return false;
}

static uint64_t next_addr(uint64_t addr, uint32_t step, uint8_t len) {
    const bool wrap2_read_en = (len == 1);
    const bool wrap4_read_en = (len == 3);
    const bool wrap2_1 = (((addr >> 4) & 0x1U) == 1U) && (step == 0) && wrap2_read_en;
    const bool wrap4_1 = (((addr >> 4) & 0x3U) == 3U) && (step == 0) && wrap4_read_en;
    const bool wrap4_2 = (((addr >> 4) & 0x3U) == 3U) && (step == 1) && wrap4_read_en;
    const bool wrap4_3 = (((addr >> 4) & 0x3U) == 3U) && (step == 2) && wrap4_read_en;
    if (wrap4_1 || wrap4_2 || wrap4_3) {
        return addr - 0x30ULL;
    }
    if (wrap2_1) {
        return addr - 0x10ULL;
    }
    return addr + 0x10ULL;
}

static bool check_rdata(uint64_t addr,
                        const std::array<uint8_t, 16> &expected,
                        const VlWide<4> &actual) {
    const auto got = bytes_from_wide(actual);
    if (got == expected) {
        return true;
    }
    std::cerr << "[TB] rdata mismatch at addr 0x" << std::hex << addr << std::dec << "\n";
    std::cerr << "     expected:";
    for (const auto &b : expected) {
        std::cerr << " " << std::hex << static_cast<int>(b);
    }
    std::cerr << std::dec << "\n";
    std::cerr << "     got     :";
    for (const auto &b : got) {
        std::cerr << " " << std::hex << static_cast<int>(b);
    }
    std::cerr << std::dec << "\n";
    return false;
}

static bool read_burst(VerilatedContext *ctx,
                       Vaxi_slave128 *dut,
                       std::unordered_map<uint32_t, std::array<uint8_t, 16>> &mem,
                       uint64_t start_addr,
                       uint8_t len) {
    dut->araddr_s0 = start_addr;
    dut->arlen_s0 = len;
    dut->arsize_s0 = 4;
    dut->arvalid_s0 = 1;
    if (!wait_for_arready(ctx, dut, 16)) {
        std::cerr << "[TB] arready did not assert for addr 0x" << std::hex << start_addr << std::dec << "\n";
        return false;
    }
    tick(ctx, dut);
    dut->arvalid_s0 = 0;

    uint64_t addr = start_addr;
    for (uint32_t step = 0; step <= len; ++step) {
        if (!wait_for_rvalid(ctx, dut, 32)) {
            std::cerr << "[TB] rvalid timeout at step " << step << "\n";
            return false;
        }
        const uint32_t idx = line_addr(addr);
        const auto it = mem.find(idx);
        if (it == mem.end()) {
            std::cerr << "[TB] missing expected data for addr 0x" << std::hex << addr << std::dec << "\n";
            return false;
        }
        if (!check_rdata(addr, it->second, dut->rdata_s0)) {
            return false;
        }
        tick(ctx, dut);
        addr = next_addr(addr, step, len);
    }
    return true;
}

static bool write_single(VerilatedContext *ctx,
                         Vaxi_slave128 *dut,
                         std::unordered_map<uint32_t, std::array<uint8_t, 16>> &mem,
                         uint64_t addr,
                         const std::array<uint8_t, 16> &data,
                         uint16_t wstrb,
                         uint8_t awid) {
    dut->awaddr_s0 = addr;
    dut->awlen_s0 = 0;
    dut->awsize_s0 = 4;
    dut->awid_s0 = awid;
    dut->awvalid_s0 = 1;
    if (!wait_for_awready(ctx, dut, 16)) {
        std::cerr << "[TB] awready did not assert for addr 0x" << std::hex << addr << std::dec << "\n";
        return false;
    }
    tick(ctx, dut);
    dut->awvalid_s0 = 0;

    wide_from_bytes(dut->wdata_s0, data);
    dut->wstrb_s0 = wstrb;
    dut->wlast_s0 = 1;
    dut->wvalid_s0 = 1;
    if (!wait_for_wready(ctx, dut, 16)) {
        std::cerr << "[TB] wready did not assert for addr 0x" << std::hex << addr << std::dec << "\n";
        return false;
    }
    tick(ctx, dut);
    dut->wvalid_s0 = 0;
    dut->wlast_s0 = 0;

    const uint32_t idx = line_addr(addr);
    auto &line = mem[idx];
    apply_wstrb(line, data, wstrb);

    if (!wait_for_bvalid(ctx, dut, 32)) {
        std::cerr << "[TB] bvalid timeout\n";
        return false;
    }
    if (dut->bid_s0 != awid) {
        std::cerr << "[TB] bid mismatch: got " << static_cast<int>(dut->bid_s0)
                  << " expected " << static_cast<int>(awid) << "\n";
        return false;
    }
    tick(ctx, dut);
    return true;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vaxi_slave128(ctx);

    dut->pll_core_cpuclk = 0;
    dut->pad_cpu_rst_b = 0;
    dut->araddr_s0 = 0;
    dut->arburst_s0 = 0;
    dut->arcache_s0 = 0;
    dut->arid_s0 = 0;
    dut->arlen_s0 = 0;
    dut->arprot_s0 = 0;
    dut->arsize_s0 = 0;
    dut->arvalid_s0 = 0;
    dut->awaddr_s0 = 0;
    dut->awburst_s0 = 0;
    dut->awcache_s0 = 0;
    dut->awid_s0 = 0;
    dut->awlen_s0 = 0;
    dut->awprot_s0 = 0;
    dut->awsize_s0 = 0;
    dut->awvalid_s0 = 0;
    dut->bready_s0 = 1;
    dut->rready_s0 = 1;
    wide_from_bytes(dut->wdata_s0, {});
    dut->wid_s0 = 0;
    dut->wlast_s0 = 0;
    dut->wstrb_s0 = 0;
    dut->wvalid_s0 = 0;
    dut->tb_init_en = 0;
    dut->tb_init_addr = 0;
    wide_from_bytes(dut->tb_init_wdata, {});
    dut->tb_init_wen = 0xffff;

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->pad_cpu_rst_b = 1;

    std::unordered_map<uint32_t, std::array<uint8_t, 16>> mem;

    const uint64_t init_addrs[] = {0x0000ULL, 0x0010ULL, 0x0020ULL, 0x0030ULL, 0x0040ULL};
    for (const auto addr : init_addrs) {
        const uint32_t idx = line_addr(addr);
        const auto data = pattern_for_addr(idx);
        wide_from_bytes(dut->tb_init_wdata, data);
        dut->tb_init_addr = idx;
        dut->tb_init_wen = 0x0000;
        dut->tb_init_en = 1;
        tick(ctx, dut);
        mem[idx] = data;
    }
    dut->tb_init_en = 0;
    dut->tb_init_wen = 0xffff;
    tick(ctx, dut);

    bool ok = true;
    ok &= read_burst(ctx, dut, mem, 0x0000ULL, 0);
    ok &= read_burst(ctx, dut, mem, 0x0010ULL, 1);
    ok &= read_burst(ctx, dut, mem, 0x0030ULL, 3);
    ok &= read_burst(ctx, dut, mem, 0x0020ULL, 3);
    ok &= read_burst(ctx, dut, mem, 0x0010ULL, 3);

    const uint64_t wr_addr = 0x0040ULL;
    const uint32_t wr_idx = line_addr(wr_addr);
    auto wr_data = pattern_for_addr(wr_idx);
    wr_data[0] ^= 0x5a;
    wr_data[7] ^= 0xa5;
    ok &= write_single(ctx, dut, mem, wr_addr, wr_data, 0x00ff, 0x3c);
    ok &= read_burst(ctx, dut, mem, wr_addr, 0);

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
