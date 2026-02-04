#include "Vmem_ctrl.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

static void tick(VerilatedContext *ctx, Vmem_ctrl *dut) {
    dut->pll_core_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->pll_core_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

static inline uint32_t xorshift32(uint32_t &state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

template <typename T>
static inline void zero_signal(T &signal) {
    signal = 0;
}

template <std::size_t W>
static inline void zero_signal(VlWide<W> &signal) {
    VL_ZERO_W(W, signal);
}

static void init_inputs(Vmem_ctrl *dut) {
    dut->pll_core_cpuclk = 0;
    dut->pad_cpu_rst_b = 0;
    dut->haddr_s1 = 0;
    dut->hburst_s1 = 0;
    dut->hprot_s1 = 0;
    dut->hsel_s1 = 0;
    dut->hsize_s1 = 0;
    dut->htrans_s1 = 0;
    zero_signal(dut->hwdata_s1);
    dut->hwrite_s1 = 0;
}

static inline std::size_t mem_index(uint64_t addr) {
    return static_cast<std::size_t>((addr >> 4) & 0x1fffU);
}

static inline uint16_t byte_mask(uint8_t size, uint64_t addr) {
    switch (size & 0x7) {
    case 0: {
        const uint8_t byte = static_cast<uint8_t>(addr & 0xfU);
        return static_cast<uint16_t>(1u << byte);
    }
    case 1: {
        const uint8_t group = static_cast<uint8_t>((addr >> 1) & 0x7U);
        return static_cast<uint16_t>(0x3u << (group * 2));
    }
    case 2: {
        const uint8_t group = static_cast<uint8_t>((addr >> 2) & 0x3U);
        return static_cast<uint16_t>(0xfu << (group * 4));
    }
    case 3: {
        const uint8_t group = static_cast<uint8_t>((addr >> 3) & 0x1U);
        return static_cast<uint16_t>(0xffu << (group * 8));
    }
    case 4:
        return 0xffffu;
    default:
        return 0u;
    }
}

static inline void fill_random_bytes(std::array<uint8_t, 16> &bytes, uint32_t &rng) {
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        bytes[i] = static_cast<uint8_t>(xorshift32(rng));
    }
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

static void drive_idle(Vmem_ctrl *dut) {
    dut->hsel_s1 = 0;
    dut->htrans_s1 = 0;
    dut->hwrite_s1 = 0;
    dut->haddr_s1 = 0;
    dut->hsize_s1 = 0;
    dut->hburst_s1 = 0;
    dut->hprot_s1 = 0;
    zero_signal(dut->hwdata_s1);
}

static bool wait_ready(VerilatedContext *ctx, Vmem_ctrl *dut, int max_cycles) {
    for (int i = 0; i < max_cycles; ++i) {
        if (dut->hready_s1) {
            return true;
        }
        drive_idle(dut);
        tick(ctx, dut);
    }
    return false;
}

static void apply_write(std::vector<std::array<uint8_t, 16>> &mem,
                        uint64_t addr,
                        uint8_t size,
                        const std::array<uint8_t, 16> &data) {
    const std::size_t idx = mem_index(addr);
    if (idx >= mem.size()) {
        return;
    }
    const uint16_t mask = byte_mask(size, addr);
    for (std::size_t i = 0; i < 16; ++i) {
        if (mask & (1u << i)) {
            mem[idx][i] = data[i];
        }
    }
}

static bool check_read(Vmem_ctrl *dut,
                       const std::vector<std::array<uint8_t, 16>> &mem,
                       uint64_t addr) {
    const std::size_t idx = mem_index(addr);
    if (idx >= mem.size()) {
        return true;
    }
    const auto expected = mem[idx];
    const auto actual = bytes_from_wide(dut->hrdata_s1);
    if (actual != expected) {
        std::cerr << "[TB] read mismatch at addr=0x" << std::hex << addr << std::dec << "\n";
        return false;
    }
    return true;
}

static bool perform_write(VerilatedContext *ctx,
                          Vmem_ctrl *dut,
                          std::vector<std::array<uint8_t, 16>> &mem,
                          uint64_t addr,
                          uint8_t size,
                          const std::array<uint8_t, 16> &data) {
    if (!wait_ready(ctx, dut, 16)) {
        std::cerr << "[TB] timeout waiting for hready" << std::endl;
        return false;
    }
    dut->hsel_s1 = 1;
    dut->htrans_s1 = 2;
    dut->hwrite_s1 = 1;
    dut->haddr_s1 = addr;
    dut->hsize_s1 = size;
    dut->hburst_s1 = 0;
    dut->hprot_s1 = 0;
    wide_from_bytes(dut->hwdata_s1, data);
    tick(ctx, dut);
    // Data phase: hold write data for the registered write enable.
    dut->hsel_s1 = 0;
    dut->htrans_s1 = 0;
    dut->hwrite_s1 = 0;
    dut->haddr_s1 = addr;
    dut->hsize_s1 = size;
    tick(ctx, dut);
    apply_write(mem, addr, size, data);
    return dut->hresp_s1 == 0;
}

static bool perform_read(VerilatedContext *ctx,
                         Vmem_ctrl *dut,
                         const std::vector<std::array<uint8_t, 16>> &mem,
                         uint64_t addr,
                         uint8_t size) {
    if (!wait_ready(ctx, dut, 16)) {
        std::cerr << "[TB] timeout waiting for hready" << std::endl;
        return false;
    }
    dut->hsel_s1 = 1;
    dut->htrans_s1 = 2;
    dut->hwrite_s1 = 0;
    dut->haddr_s1 = addr;
    dut->hsize_s1 = size;
    dut->hburst_s1 = 0;
    dut->hprot_s1 = 0;
    tick(ctx, dut);
    // Allow a data phase cycle while keeping address stable.
    dut->hsel_s1 = 0;
    dut->htrans_s1 = 0;
    dut->hwrite_s1 = 0;
    dut->haddr_s1 = addr;
    dut->hsize_s1 = size;
    tick(ctx, dut);
    return dut->hresp_s1 == 0 && check_read(dut, mem, addr);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vmem_ctrl(ctx);
    init_inputs(dut);

    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->pad_cpu_rst_b = 1;

    for (int i = 0; i < 4; ++i) {
        tick(ctx, dut);
    }

    if (!dut->hready_s1 || dut->hresp_s1 != 0) {
        std::cerr << "[TB] expected hready high and hresp=0 after reset" << std::endl;
        return 1;
    }

    std::vector<std::array<uint8_t, 16>> mem(8192);
    uint32_t rng = 0x13579bdu;

    const std::array<uint8_t, 5> sizes = {0, 1, 2, 3, 4};
    for (std::size_t s = 0; s < sizes.size(); ++s) {
        const uint8_t size = sizes[s];
        const uint64_t base = static_cast<uint64_t>(s * 0x40);
        const int stride = (size == 0) ? 1 : (size == 1) ? 2 : (size == 2) ? 4 : (size == 3) ? 8 : 16;
        const int limit = (size == 0) ? 16 : (size == 1) ? 16 : (size == 2) ? 16 : (size == 3) ? 16 : 16;
        for (int offset = 0; offset < limit; offset += stride) {
            std::array<uint8_t, 16> data{};
            fill_random_bytes(data, rng);
            const uint64_t addr = base + static_cast<uint64_t>(offset);
            if (!perform_write(ctx, dut, mem, addr, size, data)) {
                std::cerr << "[TB] write failed at size=" << static_cast<int>(size) << std::endl;
                return 1;
            }
            if (!perform_read(ctx, dut, mem, addr, size)) {
                std::cerr << "[TB] read failed at size=" << static_cast<int>(size) << std::endl;
                return 1;
            }
        }
    }

    // Exercise read-after-write bypass and stall paths.
    for (int i = 0; i < 32; ++i) {
        std::array<uint8_t, 16> data{};
        fill_random_bytes(data, rng);
        const uint64_t addr = 0x200 + static_cast<uint64_t>(i * 4);
        const uint8_t size = static_cast<uint8_t>(i % 5);
        if (!perform_write(ctx, dut, mem, addr, size, data)) {
            std::cerr << "[TB] RAW write failed" << std::endl;
            return 1;
        }
        const uint64_t alt_addr = addr ^ 0x10u;
        if (!perform_read(ctx, dut, mem, alt_addr, size)) {
            std::cerr << "[TB] RAW read failed" << std::endl;
            return 1;
        }
    }

    // Extra traffic for coverage without strict checking.
    for (int cycle = 0; cycle < 256; ++cycle) {
        const uint32_t r0 = xorshift32(rng);
        const bool valid = (r0 & 0x3u) != 0;
        if (valid && dut->hready_s1) {
            std::array<uint8_t, 16> data{};
            fill_random_bytes(data, rng);
            const uint64_t addr = static_cast<uint64_t>((r0 >> 4) & 0x1fffU) << 4 | (r0 & 0xf);
            const uint8_t size = static_cast<uint8_t>((r0 >> 2) % 5);
            const bool write = (r0 & 0x40u) != 0;
            dut->hsel_s1 = 1;
            dut->htrans_s1 = 2;
            dut->hwrite_s1 = write ? 1 : 0;
            dut->haddr_s1 = addr;
            dut->hsize_s1 = size;
            dut->hburst_s1 = 0;
            dut->hprot_s1 = 0;
            if (write) {
                wide_from_bytes(dut->hwdata_s1, data);
                tick(ctx, dut);
                dut->hsel_s1 = 0;
                dut->htrans_s1 = 0;
                dut->hwrite_s1 = 0;
                dut->haddr_s1 = addr;
                dut->hsize_s1 = size;
                tick(ctx, dut);
                apply_write(mem, addr, size, data);
            } else {
                tick(ctx, dut);
                dut->hsel_s1 = 0;
                dut->htrans_s1 = 0;
                dut->hwrite_s1 = 0;
                dut->haddr_s1 = addr;
                dut->hsize_s1 = size;
                tick(ctx, dut);
            }
        } else {
            drive_idle(dut);
            tick(ctx, dut);
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
