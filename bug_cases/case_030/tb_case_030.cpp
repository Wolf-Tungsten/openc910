#include "Vct_idu_id_decd.h"
#include "verilated.h"
#if VM_COVERAGE
#include "verilated_cov.h"
#endif

#include <cstdint>
#include <cstdlib>
#include <iostream>

// Testbench for ct_idu_id_decd - ID instruction decode module
// Tests instruction type decoding, especially LSU (Load/Store Unit) type

static void tick(VerilatedContext *ctx, Vct_idu_id_decd *dut) {
    dut->eval();
    ctx->timeInc(1);
}

// Test instruction encoding helpers
// RISC-V RV64I load instructions
static constexpr uint32_t ENCODE_LB(uint32_t rd, uint32_t rs1, uint32_t imm) {
    return (imm & 0xFFF) << 20 | (rs1 & 0x1F) << 15 | 0b000 << 12 | (rd & 0x1F) << 7 | 0b0000011;
}
static constexpr uint32_t ENCODE_LH(uint32_t rd, uint32_t rs1, uint32_t imm) {
    return (imm & 0xFFF) << 20 | (rs1 & 0x1F) << 15 | 0b001 << 12 | (rd & 0x1F) << 7 | 0b0000011;
}
static constexpr uint32_t ENCODE_LW(uint32_t rd, uint32_t rs1, uint32_t imm) {
    return (imm & 0xFFF) << 20 | (rs1 & 0x1F) << 15 | 0b010 << 12 | (rd & 0x1F) << 7 | 0b0000011;
}
static constexpr uint32_t ENCODE_LD(uint32_t rd, uint32_t rs1, uint32_t imm) {
    return (imm & 0xFFF) << 20 | (rs1 & 0x1F) << 15 | 0b011 << 12 | (rd & 0x1F) << 7 | 0b0000011;
}

// RISC-V RV64I store instructions
static constexpr uint32_t ENCODE_SB(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    return ((imm & 0xFE0) << 20) | ((rs2 & 0x1F) << 20) | ((rs1 & 0x1F) << 15) | 0b000 << 12 | ((imm & 0x1F) << 7) | 0b0100011;
}
static constexpr uint32_t ENCODE_SH(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    return ((imm & 0xFE0) << 20) | ((rs2 & 0x1F) << 20) | ((rs1 & 0x1F) << 15) | 0b001 << 12 | ((imm & 0x1F) << 7) | 0b0100011;
}
static constexpr uint32_t ENCODE_SW(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    return ((imm & 0xFE0) << 20) | ((rs2 & 0x1F) << 20) | ((rs1 & 0x1F) << 15) | 0b010 << 12 | ((imm & 0x1F) << 7) | 0b0100011;
}
static constexpr uint32_t ENCODE_SD(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    return ((imm & 0xFE0) << 20) | ((rs2 & 0x1F) << 20) | ((rs1 & 0x1F) << 15) | 0b011 << 12 | ((imm & 0x1F) << 7) | 0b0100011;
}

// RISC-V RV64I ALU instructions
static constexpr uint32_t ENCODE_ADDI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    return (imm & 0xFFF) << 20 | (rs1 & 0x1F) << 15 | 0b000 << 12 | (rd & 0x1F) << 7 | 0b0010011;
}
static constexpr uint32_t ENCODE_ADD(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    return 0b0000000 << 25 | (rs2 & 0x1F) << 20 | (rs1 & 0x1F) << 15 | 0b000 << 12 | (rd & 0x1F) << 7 | 0b0110011;
}

// RISC-V compressed instructions
static constexpr uint32_t ENCODE_C_LW(uint32_t rd, uint32_t rs1, uint32_t imm) {
    uint32_t uimm = ((imm & 0x4) << 4) | ((imm & 0x38) << 7) | ((imm & 0x40) >> 1);
    return 0b010 << 13 | ((rs1 & 0x7) << 7) | uimm | ((rd & 0x7) << 2) | 0b00;
}
static constexpr uint32_t ENCODE_C_SW(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    uint32_t uimm = ((imm & 0x4) << 7) | ((imm & 0x38) << 7) | ((imm & 0x40) >> 1);
    return 0b110 << 13 | ((rs1 & 0x7) << 7) | uimm | ((rs2 & 0x7) << 2) | 0b00;
}
static constexpr uint32_t ENCODE_C_LD(uint32_t rd, uint32_t rs1, uint32_t imm) {
    uint32_t uimm = ((imm & 0x8) << 7) | ((imm & 0x30) << 7) | ((imm & 0x100) >> 2) | ((imm & 0x40) >> 1);
    return 0b011 << 13 | ((rs1 & 0x7) << 7) | uimm | ((rd & 0x7) << 2) | 0b00;
}
static constexpr uint32_t ENCODE_C_SD(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    uint32_t uimm = ((imm & 0x8) << 7) | ((imm & 0x30) << 7) | ((imm & 0x100) >> 2) | ((imm & 0x40) >> 1);
    return 0b111 << 13 | ((rs1 & 0x7) << 7) | uimm | ((rs2 & 0x7) << 2) | 0b00;
}

// Instruction type parameters from RTL
static constexpr uint32_t TYPE_WIDTH = 10;
static constexpr uint32_t ALU        = 0b0000000001;
static constexpr uint32_t BJU        = 0b0000000010;
static constexpr uint32_t MULT       = 0b0000000100;
static constexpr uint32_t DIV        = 0b0000001000;
static constexpr uint32_t LSU_P5     = 0b0000110000;
static constexpr uint32_t LSU        = 0b0000010000;
static constexpr uint32_t PIPE67     = 0b0001000000;
static constexpr uint32_t PIPE6      = 0b0010000000;
static constexpr uint32_t PIPE7      = 0b0100000000;
static constexpr uint32_t SPECIAL    = 0b1000000000;

struct TestCase {
    const char* name;
    uint32_t inst;
    uint32_t expected_inst_type;
    bool expect_dst_vld;
    bool expect_src0_vld;
    bool expect_src1_vld;
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *ctx = new VerilatedContext();
    ctx->traceEverOn(false);

    auto *dut = new Vct_idu_id_decd(ctx);

    // Initialize control inputs (default inactive values)
    dut->cp0_idu_cskyee = 1;
    dut->cp0_idu_frm = 0;
    dut->cp0_idu_fs = 3;  // FS = dirty to allow FP ops
    dut->cp0_idu_vill = 0;
    dut->cp0_idu_vs = 0;
    dut->cp0_idu_vstart = 0;
    dut->cp0_idu_zero_delay_move_disable = 0;
    dut->cp0_yy_hyper = 0;
    dut->x_vl = 0;
    dut->x_vlmul = 0;
    dut->x_vsew = 0;

    std::cerr << "[TB] Starting ct_idu_id_decd test" << std::endl;
    std::cerr << "[TB] Testing instruction type decoding (LSU focus)" << std::endl;

    // Test cases
    TestCase tests[] = {
        // Load instructions (32-bit)
        {"LB",  ENCODE_LB(1, 2, 0x10), LSU, true, true, false},
        {"LH",  ENCODE_LH(1, 2, 0x10), LSU, true, true, false},
        {"LW",  ENCODE_LW(1, 2, 0x10), LSU, true, true, false},
        {"LD",  ENCODE_LD(1, 2, 0x10), LSU, true, true, false},
        
        // Store instructions (32-bit) - LSU_P5
        {"SB",  ENCODE_SB(2, 3, 0x10), LSU_P5, false, true, true},
        {"SH",  ENCODE_SH(2, 3, 0x10), LSU_P5, false, true, true},
        {"SW",  ENCODE_SW(2, 3, 0x10), LSU_P5, false, true, true},
        {"SD",  ENCODE_SD(2, 3, 0x10), LSU_P5, false, true, true},
        
        // Compressed load instructions
        {"C.LW", ENCODE_C_LW(1, 2, 0x4), LSU, true, true, false},
        {"C.LD", ENCODE_C_LD(1, 2, 0x8), LSU, true, true, false},
        
        // Compressed store instructions
        {"C.SW", ENCODE_C_SW(2, 3, 0x4), LSU_P5, false, true, true},
        {"C.SD", ENCODE_C_SD(2, 3, 0x8), LSU_P5, false, true, true},
        
        // ALU instructions
        {"ADDI", ENCODE_ADDI(1, 2, 0x10), ALU, true, true, false},
        {"ADD",  ENCODE_ADD(1, 2, 3), ALU, true, true, true},
    };

    int passed = 0;
    int failed = 0;
    int total = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < total; i++) {
        dut->x_inst = tests[i].inst;
        tick(ctx, dut);

        uint32_t actual_inst_type = dut->x_inst_type;
        bool type_match = (actual_inst_type == tests[i].expected_inst_type);
        
        // Check key signals
        bool dst_ok = (dut->x_dst_vld == tests[i].expect_dst_vld);
        bool src0_ok = (dut->x_src0_vld == tests[i].expect_src0_vld);
        bool src1_ok = (dut->x_src1_vld == tests[i].expect_src1_vld);
        bool all_ok = type_match && dst_ok && src0_ok && src1_ok;

        if (all_ok) {
            std::cerr << "[TB] PASS: " << tests[i].name 
                      << " inst=0x" << std::hex << tests[i].inst << std::dec
                      << " type=0x" << std::hex << actual_inst_type << std::dec
                      << std::endl;
            passed++;
        } else {
            std::cerr << "[TB] FAIL: " << tests[i].name 
                      << " inst=0x" << std::hex << tests[i].inst << std::dec
                      << " expected_type=0x" << std::hex << tests[i].expected_inst_type 
                      << " actual_type=0x" << actual_inst_type << std::dec
                      << " dst=" << dut->x_dst_vld << "/" << tests[i].expect_dst_vld
                      << " src0=" << dut->x_src0_vld << "/" << tests[i].expect_src0_vld
                      << " src1=" << dut->x_src1_vld << "/" << tests[i].expect_src1_vld
                      << std::endl;
            failed++;
        }
    }

    // Additional test: LSU type bit check
    std::cerr << "[TB] =============================" << std::endl;
    std::cerr << "[TB] LSU type bit validation test" << std::endl;
    
    bool lsu_bit_ok = true;
    // Test LW - should have LSU bit set (bit 4)
    dut->x_inst = ENCODE_LW(1, 2, 0);
    tick(ctx, dut);
    if ((dut->x_inst_type & LSU) == 0) {
        std::cerr << "[TB] ERROR: LW instruction does not have LSU bit set!" << std::endl;
        std::cerr << "[TB]   x_inst_type = 0x" << std::hex << (uint32_t)dut->x_inst_type << std::dec << std::endl;
        lsu_bit_ok = false;
    }
    
    // Test SW - should have LSU_P5 bit set (bits 4 and 5)
    dut->x_inst = ENCODE_SW(2, 3, 0);
    tick(ctx, dut);
    if ((dut->x_inst_type & LSU_P5) != LSU_P5) {
        std::cerr << "[TB] ERROR: SW instruction does not have LSU_P5 bits set!" << std::endl;
        std::cerr << "[TB]   x_inst_type = 0x" << std::hex << (uint32_t)dut->x_inst_type << std::dec << std::endl;
        lsu_bit_ok = false;
    }
    
    if (lsu_bit_ok) {
        std::cerr << "[TB] LSU type bit validation PASSED" << std::endl;
    } else {
        failed++;
    }

    std::cerr << "[TB] =============================" << std::endl;
    std::cerr << "[TB] Test Summary:" << std::endl;
    std::cerr << "[TB]   Total: " << total << std::endl;
    std::cerr << "[TB]   Passed: " << passed << std::endl;
    std::cerr << "[TB]   Failed: " << failed << std::endl;

    bool test_passed = (failed == 0);
    
    if (test_passed) {
        std::cerr << "[TB] PASSED: All ct_idu_id_decd tests passed" << std::endl;
    } else {
        std::cerr << "[TB] FAILED: " << failed << " test(s) failed" << std::endl;
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
