#include "Vct_iu_alu.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

// ALU result select parameters (from RTL)
constexpr uint32_t ADDER_ADD   = 0x000001;
constexpr uint32_t ADDER_ADDW  = 0x000002;
constexpr uint32_t ADDER_SUB   = 0x000004;
constexpr uint32_t ADDER_SUBW  = 0x000008;
constexpr uint32_t ADDER_SLT   = 0x000010;
constexpr uint32_t SHIFTER_SL  = 0x000020;
constexpr uint32_t SHIFTER_SR  = 0x000040;
constexpr uint32_t SHIFTER_SLW = 0x000080;
constexpr uint32_t SHIFTER_SRW = 0x000100;
constexpr uint32_t SHIFTER_EXT = 0x000200;
constexpr uint32_t LOGIC_AND   = 0x000400;
constexpr uint32_t LOGIC_OR    = 0x000800;
constexpr uint32_t LOGIC_XOR   = 0x001000;
constexpr uint32_t LOGIC_LUI   = 0x002000;
constexpr uint32_t LOGIC_CLI   = 0x004000;
constexpr uint32_t MISC_MV     = 0x008000;
constexpr uint32_t MISC_TSTNBZ = 0x010000;
constexpr uint32_t MISC_TST    = 0x020000;
constexpr uint32_t MISC_FF1    = 0x040000;
constexpr uint32_t MISC_REV    = 0x080000;
constexpr uint32_t MISC_REVW   = 0x100000;

// Long operations (bits 5-0)
constexpr uint32_t ADDER_MAX   = 0x01;
constexpr uint32_t ADDER_MAXW  = 0x02;
constexpr uint32_t ADDER_MIN   = 0x04;
constexpr uint32_t ADDER_MINW  = 0x08;
constexpr uint32_t ADDER_ADDSL = 0x20;

static void tick(VerilatedContext *ctx, Vct_iu_alu *dut) {
    dut->forever_cpuclk = 0;
    dut->eval();
    ctx->timeInc(1);
    dut->forever_cpuclk = 1;
    dut->eval();
    ctx->timeInc(1);
}

// Compute expected result for adder fwd operations
static uint64_t compute_adder_fwd(uint32_t rslt_sel, uint64_t src0, uint64_t src1) {
    uint32_t sel = rslt_sel & 0x1F;
    switch (sel) {
        case 0x01: return src0 + src1;                          // ADD
        case 0x02: return (int64_t)(int32_t)(src0 + src1);      // ADDW
        case 0x04: return src0 - src1;                          // SUB
        case 0x08: return (int64_t)(int32_t)(src0 - src1);      // SUBW
        case 0x10: return (src0 < src1) ? 1 : 0;                // SLT
        default: return 0;
    }
}

// Compute expected result for adder long operations
static uint64_t compute_adder_long(uint32_t rslt_sel, uint64_t src0, uint64_t src1, uint8_t imm) {
    uint32_t sel = rslt_sel & 0x3F;
    switch (sel) {
        case 0x01: return (src0 < src1) ? src1 : src0;          // MAX
        case 0x02: {                                            // MAXW
            int32_t s0 = (int32_t)src0;
            int32_t s1 = (int32_t)src1;
            return (int64_t)((s0 < s1) ? s1 : s0);
        }
        case 0x04: return (src0 < src1) ? src0 : src1;          // MIN
        case 0x08: {                                            // MINW
            int32_t s0 = (int32_t)src0;
            int32_t s1 = (int32_t)src1;
            return (int64_t)((s0 < s1) ? s0 : s1);
        }
        case 0x20: {                                            // ADDSL
            uint64_t shifted_src1 = src1 << (imm & 0x3);
            return src0 + shifted_src1;
        }
        default: return 0;
    }
}

static uint64_t compute_shifter_result(uint32_t rslt_sel, uint64_t src0, uint64_t src1, uint8_t func, uint8_t imm) {
    uint32_t sel = rslt_sel & 0x3E0;
    uint8_t shift_amt = src1 & 0x3F;
    uint8_t shift_amt_w = src1 & 0x1F;
    
    switch (sel) {
        case 0x020: return src0 << shift_amt;                       // SL
        case 0x040: {                                               // SR
            bool arithmetic = (func & 0x2) != 0;
            uint64_t sign_ext = arithmetic ? ((src0 >> 63) ? ~0ULL : 0) : 0;
            return ((sign_ext << (64 - shift_amt)) | (src0 >> shift_amt));
        }
        case 0x080: return (int64_t)(int32_t)(src0 << shift_amt_w); // SLW
        case 0x100: {                                               // SRW
            bool arithmetic = (func & 0x2) != 0;
            uint32_t s0 = (uint32_t)src0;
            uint32_t sign_ext = arithmetic ? ((s0 >> 31) ? ~0U : 0) : 0;
            return (int64_t)(int32_t)((sign_ext << (32 - shift_amt_w)) | (s0 >> shift_amt_w));
        }
        case 0x200: {                                               // EXT
            uint8_t ext_bits = imm & 0x3F;
            uint64_t mask = (1ULL << (ext_bits + 1)) - 1;
            uint64_t val = src0 >> src1;
            bool sign = (src0 >> ext_bits) & 1;
            bool exts = (func & 0x4) != 0;
            if (exts && sign) {
                return (val & mask) | ~mask;
            } else {
                return val & mask;
            }
        }
        default: return 0;
    }
}

static uint64_t compute_logic_result(uint32_t rslt_sel, uint64_t src0, uint64_t src1) {
    uint32_t sel = rslt_sel & 0x7C00;
    switch (sel) {
        case 0x400: return src0 & src1;                                     // AND
        case 0x800: return src0 | src1;                                     // OR
        case 0x1000: return src0 ^ src1;                                    // XOR
        case 0x2000: return ((int64_t)(int32_t)(src1 << 12)) & ~0xFFFULL;   // LUI
        case 0x4000: {                                                      // CLI (6-bit sign extend)
            uint8_t val = src1 & 0x3F;
            return (val & 0x20) ? (~0ULL << 6) | val : val;
        }
        default: return 0;
    }
}

static uint64_t compute_misc_result(uint32_t rslt_sel, uint64_t src0, uint64_t src1, uint64_t src2, uint8_t func) {
    uint32_t sel = rslt_sel & 0x1F8000;
    
    switch (sel) {
        case 0x8000: {  // MV
            bool src1_eq_0 = (src1 == 0);
            bool mv_sel = ((func & 0x3) == 0x2 && !src1_eq_0) || ((func & 0x3) == 0x3 && src1_eq_0);
            return mv_sel ? src2 : src0;
        }
        case 0x10000: {  // TSTNBZ
            uint64_t rslt = 0;
            for (int i = 0; i < 8; i++) {
                uint8_t byte = (src0 >> (i * 8)) & 0xFF;
                if (byte == 0) rslt |= (0xFFULL << (i * 8));
            }
            return rslt;
        }
        case 0x20000: {  // TST
            uint8_t bit_idx = src1 & 0x3F;
            return (src0 >> bit_idx) & 1;
        }
        case 0x40000: {  // FF1/FF0
            bool ff0_sel = (func & 0x1) != 0;
            uint64_t test_val = ff0_sel ? ~src0 : src0;
            for (int i = 63; i >= 0; i--) {
                if ((test_val >> i) & 1) return i;
            }
            return 64;
        }
        case 0x80000: {  // REV
            uint64_t rslt = 0;
            for (int i = 0; i < 8; i++) {
                rslt |= ((src0 >> (i * 8)) & 0xFF) << ((7 - i) * 8);
            }
            return rslt;
        }
        case 0x100000: {  // REVW
            uint32_t s0 = (uint32_t)src0;
            uint32_t rslt = 0;
            for (int i = 0; i < 4; i++) {
                rslt |= ((s0 >> (i * 8)) & 0xFF) << ((3 - i) * 8);
            }
            return (int64_t)(int32_t)rslt;
        }
        default: return 0;
    }
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_iu_alu(ctx);

    // Initialize control signals
    dut->cp0_iu_icg_en = 1;
    dut->cp0_yy_clk_en = 1;
    dut->pad_yy_icg_scan_en = 0;
    dut->rtu_yy_xx_flush = 0;
    dut->had_idu_wbbr_vld = 0;
    dut->had_idu_wbbr_data = 0;
    
    // Initialize inputs
    dut->idu_iu_rf_pipex_sel = 0;
    dut->idu_iu_rf_pipex_gateclk_sel = 0;
    dut->idu_iu_rf_pipex_dst_vld = 0;
    dut->idu_iu_rf_pipex_dst_preg = 0;
    dut->idu_iu_rf_pipex_dstv_vld = 0;
    dut->idu_iu_rf_pipex_dst_vreg = 0;
    dut->idu_iu_rf_pipex_func = 0;
    dut->idu_iu_rf_pipex_rslt_sel = 0;
    dut->idu_iu_rf_pipex_alu_short = 0;
    dut->idu_iu_rf_pipex_src0 = 0;
    dut->idu_iu_rf_pipex_src1 = 0;
    dut->idu_iu_rf_pipex_src2 = 0;
    dut->idu_iu_rf_pipex_imm = 0;
    dut->idu_iu_rf_pipex_vl = 0;
    dut->idu_iu_rf_pipex_vlmul = 0;
    dut->idu_iu_rf_pipex_vsew = 0;

    // Reset
    dut->cpurst_b = 0;
    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
    }
    dut->cpurst_b = 1;

    int cycle = 5;
    int errors = 0;

    // Test Adder operations
    struct TestCase {
        uint32_t rslt_sel;
        uint64_t src0;
        uint64_t src1;
        uint64_t src2;
        uint8_t func;
        uint8_t imm;
        bool alu_short;
    };

    TestCase tests[] = {
        // Adder operations - fwd path
        {ADDER_ADD,  0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, true},
        {ADDER_ADDW, 0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, true},
        {ADDER_SUB,  0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, true},
        {ADDER_SUBW, 0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, true},
        {ADDER_SLT,  0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, true},
        // Adder long operations
        {ADDER_MAX,  0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, false},
        {ADDER_MIN,  0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0, 0, 0, false},
        {ADDER_MAXW, 0x12345678ULL, 0x0FEDCBA9ULL, 0, 0, 0, false},
        {ADDER_MINW, 0x12345678ULL, 0x0FEDCBA9ULL, 0, 0, 0, false},
        {ADDER_ADDSL, 0x1000ULL, 0x200ULL, 0, 0, 0, false},
        {ADDER_ADDSL, 0x1000ULL, 0x200ULL, 0, 0, 1, false},
        {ADDER_ADDSL, 0x1000ULL, 0x200ULL, 0, 0, 2, false},
        {ADDER_ADDSL, 0x1000ULL, 0x200ULL, 0, 0, 3, false},
        // Shifter operations
        {SHIFTER_SL,  0x123456789ABCDEF0ULL, 4, 0, 0, 0, true},
        {SHIFTER_SR,  0xF23456789ABCDEF0ULL, 4, 0, 0x2, 0, true},
        {SHIFTER_SR,  0xF23456789ABCDEF0ULL, 4, 0, 0, 0, true},  // Logical
        {SHIFTER_SLW, 0x12345678ULL, 4, 0, 0, 0, true},
        {SHIFTER_SRW, 0x82345678ULL, 4, 0, 0x2, 0, true},
        {SHIFTER_SRW, 0x82345678ULL, 4, 0, 0, 0, true},  // Logical
        {SHIFTER_EXT, 0x123456789ABCDEF0ULL, 8, 0, 0x4, 0, true},
        {SHIFTER_EXT, 0x123456789ABCDEF0ULL, 8, 0, 0x4, 32, true},
        {SHIFTER_EXT, 0x123456789ABCDEF0ULL, 8, 0, 0, 16, true},
        {SHIFTER_EXT, 0x123456789ABCDEF0ULL, 8, 0, 0, 32, true},
        // Logic operations
        {LOGIC_AND, 0xFF00FF00FF00FF00ULL, 0x0F0F0F0F0F0F0F0FULL, 0, 0, 0, true},
        {LOGIC_OR,  0xFF00FF00FF00FF00ULL, 0x0F0F0F0F0F0F0F0FULL, 0, 0, 0, true},
        {LOGIC_XOR, 0xFF00FF00FF00FF00ULL, 0x0F0F0F0F0F0F0F0FULL, 0, 0, 0, true},
        {LOGIC_LUI, 0, 0x12345ULL, 0, 0, 0, true},
        {LOGIC_CLI, 0, 0xFFULL, 0, 0, 0, true},
        {LOGIC_CLI, 0, 0x7FULL, 0, 0, 0, true},
        // Misc operations
        {MISC_MV,     0xAAAAULL, 0, 0xBBBBULL, 0x2, 0, true},
        {MISC_MV,     0xAAAAULL, 1, 0xBBBBULL, 0x2, 0, true},
        {MISC_MV,     0xAAAAULL, 0, 0xBBBBULL, 0x3, 0, true},
        {MISC_MV,     0xAAAAULL, 1, 0xBBBBULL, 0x3, 0, true},
        {MISC_TSTNBZ, 0x0011223344556677ULL, 0, 0, 0, 0, true},
        {MISC_TSTNBZ, 0xFF11223344556677ULL, 0, 0, 0, 0, true},
        {MISC_TSTNBZ, 0xFFFFFFFFFFFFFFFFULL, 0, 0, 0, 0, true},
        {MISC_TSTNBZ, 0x0000000000000000ULL, 0, 0, 0, 0, true},
        {MISC_TST,    0x8000000000000000ULL, 63, 0, 0, 0, true},
        {MISC_TST,    0x0000000000000001ULL, 0, 0, 0, 0, true},
        {MISC_TST,    0x0000000100000000ULL, 32, 0, 0, 0, true},
        {MISC_REV,    0x0102030405060708ULL, 0, 0, 0, 0, true},
        {MISC_REVW,   0x01020304ULL, 0, 0, 0, 0, true},
        // Additional edge cases
        {ADDER_ADD,  0x0ULL, 0x0ULL, 0, 0, 0, true},
        {ADDER_ADD,  0xFFFFFFFFFFFFFFFFULL, 0x1ULL, 0, 0, 0, true},
        {ADDER_SUB,  0x0ULL, 0x1ULL, 0, 0, 0, true},
        {SHIFTER_SL, 0x1ULL, 63, 0, 0, 0, true},
        {SHIFTER_SR, 0x8000000000000000ULL, 63, 0, 0x2, 0, true},
    };

    for (const auto& test : tests) {
        // Set inputs
        dut->idu_iu_rf_pipex_sel = 1;
        dut->idu_iu_rf_pipex_gateclk_sel = 1;
        dut->idu_iu_rf_pipex_dst_vld = 1;
        dut->idu_iu_rf_pipex_dst_preg = 1;
        dut->idu_iu_rf_pipex_rslt_sel = test.rslt_sel;
        dut->idu_iu_rf_pipex_func = test.func;
        dut->idu_iu_rf_pipex_alu_short = test.alu_short ? 1 : 0;
        dut->idu_iu_rf_pipex_src0 = test.src0;
        dut->idu_iu_rf_pipex_src1 = test.src1;
        dut->idu_iu_rf_pipex_src2 = test.src2;
        dut->idu_iu_rf_pipex_imm = test.imm;
        
        tick(ctx, dut);
        cycle++;

        // Compute expected result
        uint64_t expected = 0;
        
        if (test.rslt_sel & 0x1F) {  // Adder fwd operations
            expected = compute_adder_fwd(test.rslt_sel, test.src0, test.src1);
        } else if (test.rslt_sel & 0x3E0) {  // Shifter operations
            expected = compute_shifter_result(test.rslt_sel, test.src0, test.src1, test.func, test.imm);
        } else if (test.rslt_sel & 0x7C00) {  // Logic operations
            expected = compute_logic_result(test.rslt_sel, test.src0, test.src1);
        } else if (test.rslt_sel & 0x1F8000) {  // Misc operations
            expected = compute_misc_result(test.rslt_sel, test.src0, test.src1, test.src2, test.func);
        } else if (test.rslt_sel & 0x3F) {  // Long adder operations
            expected = compute_adder_long(test.rslt_sel, test.src0, test.src1, test.imm);
        }

        // For long operations, use different expected
        if (!test.alu_short && (test.rslt_sel & 0x3F)) {
            expected = compute_adder_long(test.rslt_sel, test.src0, test.src1, test.imm);
        }

        uint64_t actual = dut->alu_rbus_ex1_pipex_data;
        
        if (actual != expected) {
            std::cerr << "[TB] Cycle " << cycle << " data mismatch: sel=0x" << std::hex << test.rslt_sel
                      << " expected=0x" << expected << " actual=0x" << actual << std::dec << std::endl;
            errors++;
        }

        // Clear inputs
        dut->idu_iu_rf_pipex_sel = 0;
        dut->idu_iu_rf_pipex_gateclk_sel = 0;
        tick(ctx, dut);
        cycle++;
    }

    // Test MTVR path
    dut->idu_iu_rf_pipex_sel = 1;
    dut->idu_iu_rf_pipex_gateclk_sel = 1;
    dut->idu_iu_rf_pipex_dstv_vld = 1;
    dut->idu_iu_rf_pipex_dst_vreg = 5;
    dut->idu_iu_rf_pipex_vlmul = 2;
    dut->idu_iu_rf_pipex_vsew = 3;
    dut->idu_iu_rf_pipex_vl = 8;
    dut->idu_iu_rf_pipex_src0 = 0xDEADBEEFCAFEBABEULL;
    tick(ctx, dut);
    cycle++;

    // Check MTVR outputs
    if (!dut->iu_vfpu_ex1_pipex_mtvr_vld) {
        std::cerr << "[TB] MTVR vld not set" << std::endl;
        errors++;
    }

    tick(ctx, dut);
    cycle++;

    if (dut->iu_vfpu_ex2_pipex_mtvr_src0 != 0xDEADBEEFCAFEBABEULL) {
        std::cerr << "[TB] MTVR src0 mismatch" << std::endl;
        errors++;
    }

    // Clear MTVR test
    dut->idu_iu_rf_pipex_sel = 0;
    dut->idu_iu_rf_pipex_gateclk_sel = 0;
    dut->idu_iu_rf_pipex_dstv_vld = 0;
    tick(ctx, dut);
    cycle++;

    // Test flush
    dut->idu_iu_rf_pipex_sel = 1;
    dut->idu_iu_rf_pipex_gateclk_sel = 1;
    dut->idu_iu_rf_pipex_dst_vld = 1;
    dut->idu_iu_rf_pipex_rslt_sel = ADDER_ADD;
    dut->idu_iu_rf_pipex_src0 = 0x1111;
    dut->idu_iu_rf_pipex_src1 = 0x2222;
    tick(ctx, dut);
    cycle++;

    dut->rtu_yy_xx_flush = 1;
    tick(ctx, dut);
    cycle++;
    dut->rtu_yy_xx_flush = 0;

    tick(ctx, dut);
    cycle++;

    // Additional cycles for coverage
    for (int i = 0; i < 5; ++i) {
        tick(ctx, dut);
        cycle++;
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

    if (errors > 0) {
        std::cerr << "[TB] " << errors << " test(s) failed" << std::endl;
        return 1;
    }

    std::cout << "[TB] All tests passed" << std::endl;
    return 0;
}
