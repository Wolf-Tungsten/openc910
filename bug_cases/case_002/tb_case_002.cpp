#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vct_l2cache_top.h"

namespace {

constexpr int kDataWords = 16;
constexpr int kTagWords = 12;
constexpr int kDirtyWords = 5;

void tick(Vct_l2cache_top *dut, VerilatedContext *context) {
    dut->l2c_data_clk_x = 0;
    dut->l2c_tag_clk_x = 0;
    dut->eval();
    context->timeInc(1);
    dut->l2c_data_clk_x = 1;
    dut->l2c_tag_clk_x = 1;
    dut->eval();
    context->timeInc(1);
}

void fill_words(uint32_t *target, int words, uint32_t base) {
    for (int i = 0; i < words; ++i) {
        target[i] = base + static_cast<uint32_t>(i) * 0x01010101u;
    }
}

bool expect_words_eq(const uint32_t *actual, const uint32_t *expected, int words,
                     const char *label) {
    for (int i = 0; i < words; ++i) {
        if (actual[i] != expected[i]) {
            std::cerr << "[TB] " << label << " mismatch at word " << i
                      << " expected 0x" << std::hex << expected[i]
                      << " got 0x" << actual[i] << std::dec << std::endl;
            return false;
        }
    }
    return true;
}

bool expect_words_zero(const uint32_t *actual, int words, const char *label) {
    for (int i = 0; i < words; ++i) {
        if (actual[i] != 0) {
            std::cerr << "[TB] " << label << " expected 0, got 0x" << std::hex
                      << actual[i] << std::dec << " at word " << i << std::endl;
            return false;
        }
    }
    return true;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vct_l2cache_top>(context.get());
    dut->l2c_data_clk_x = 0;
    dut->l2c_tag_clk_x = 0;
    dut->l2c_data_index0 = 0;
    dut->l2c_data_index1 = 0;
    dut->l2c_data_index2 = 0;
    dut->l2c_data_index3 = 0;
    dut->l2c_data_ram_cen = 0;
    dut->l2c_data_wen = 0;
    dut->l2c_dirty_gwen = 0;
    dut->l2c_dirty_ram_cen = 0;
    dut->l2c_tag_gwen = 0;
    dut->l2c_tag_index = 0;
    dut->l2c_tag_ram_cen = 0;
    dut->pad_yy_icg_scan_en = 0;
    std::memset(dut->l2c_data_din, 0, sizeof(dut->l2c_data_din));
    std::memset(dut->l2c_dirty_din, 0, sizeof(dut->l2c_dirty_din));
    std::memset(dut->l2c_tag_din, 0, sizeof(dut->l2c_tag_din));
    std::memset(dut->l2c_dirty_wen, 0, sizeof(dut->l2c_dirty_wen));
    std::memset(dut->l2c_tag_wen, 0, sizeof(dut->l2c_tag_wen));
    dut->eval();

    if (!expect_words_zero(dut->l2c_data_dout, kDataWords, "data_zero") ||
        !expect_words_zero(dut->l2c_tag_dout, kTagWords, "tag_zero") ||
        !expect_words_zero(dut->l2c_dirty_dout, kDirtyWords, "dirty_zero")) {
        return EXIT_FAILURE;
    }

    fill_words(dut->l2c_data_din, kDataWords, 0x1000u);
    fill_words(dut->l2c_tag_din, kTagWords, 0x2000u);
    fill_words(dut->l2c_dirty_din, kDirtyWords, 0x3000u);
    dut->l2c_data_ram_cen = 0xf;
    dut->l2c_data_wen = 0xf;
    dut->l2c_tag_ram_cen = 1;
    dut->l2c_tag_gwen = 1;
    dut->l2c_dirty_ram_cen = 1;
    dut->l2c_dirty_gwen = 1;
    dut->eval();

    if (!expect_words_eq(dut->l2c_data_dout, dut->l2c_data_din, kDataWords, "data_match") ||
        !expect_words_eq(dut->l2c_tag_dout, dut->l2c_tag_din, kTagWords, "tag_match") ||
        !expect_words_eq(dut->l2c_dirty_dout, dut->l2c_dirty_din, kDirtyWords, "dirty_match")) {
        return EXIT_FAILURE;
    }

    tick(dut.get(), context.get());

    fill_words(dut->l2c_data_din, kDataWords, 0x4000u);
    fill_words(dut->l2c_tag_din, kTagWords, 0x5000u);
    fill_words(dut->l2c_dirty_din, kDirtyWords, 0x6000u);
    dut->eval();

    if (!expect_words_eq(dut->l2c_data_dout, dut->l2c_data_din, kDataWords, "data_update") ||
        !expect_words_eq(dut->l2c_tag_dout, dut->l2c_tag_din, kTagWords, "tag_update") ||
        !expect_words_eq(dut->l2c_dirty_dout, dut->l2c_dirty_din, kDirtyWords, "dirty_update")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_002 passed: data/tag/dirty stubs reflect inputs\n";

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
