#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_scalar_dp.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void tick(Vct_fadd_scalar_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.ex1_pipe_clk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.ex1_pipe_clk = 1;
    dut.eval();
}

uint32_t make_func(bool is_double,
                   bool is_single,
                   bool op_add,
                   bool op_sub,
                   bool op_cmp,
                   bool op_maxnm,
                   bool op_minnm,
                   uint8_t cmp_flags) {
    uint32_t f = 0;
    if (is_double) f |= 1u << 16;
    if (is_single) f |= 1u << 15;
    if (op_add)    f |= 1u << 12;
    if (op_sub)    f |= 1u << 11;
    if (op_cmp)    f |= 1u << 10;
    if (op_maxnm)  f |= 1u << 9;
    if (op_minnm)  f |= 1u << 8;
    f |= (cmp_flags & 0x1F);
    return f;
}

struct Scenario {
    const char* name;
    bool is_double;
    bool is_single;
    bool op_add;
    bool op_sub;
    bool op_cmp;
    bool op_maxnm;
    bool op_minnm;
    uint8_t cmp_flags;
    uint8_t imm_rm;
    uint8_t ext_rm;
    bool cmp_doub;
    bool cmp_half;
    uint64_t src0;
    uint64_t src1;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_scalar_dp dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    auto tick_and_eval = [&] {
        tick(dut);
    };

    auto toggle_bitfield = [&](auto setter, unsigned bits) {
        setter(0);
        tick_and_eval();
        for (unsigned i = 0; i < bits; ++i) {
            setter(static_cast<uint64_t>(1) << i);
            tick_and_eval();
            setter(0);
            tick_and_eval();
        }
    };

    auto sweep_inputs_for_toggle_coverage = [&] {
        // Toggle cp0 enables and compare flags.
        dut.cp0_vfpu_icg_en = 0;
        dut.cp0_yy_clk_en = 0;
        dut.ex1_doub_cmp_result = 0;
        dut.ex1_doub_half_cmp_result = 0;
        tick_and_eval();
        dut.cp0_vfpu_icg_en = 1;
        dut.cp0_yy_clk_en = 1;
        dut.ex1_doub_cmp_result = 1;
        dut.ex1_doub_half_cmp_result = 1;
        tick_and_eval();

        // Toggle all bits of func and rounding imm override.
        toggle_bitfield([&](uint64_t v) { dut.dp_vfalu_ex1_pipex_func = v & 0xFFFFF; }, 20);
        toggle_bitfield([&](uint64_t v) { dut.dp_vfalu_ex1_pipex_imm0 = v & 0x7; }, 3);
        toggle_bitfield([&](uint64_t v) { dut.vfpu_yy_xx_rm = v & 0x7; }, 3);

        // Sweep source operands and result buses to toggle every bit.
        toggle_bitfield([&](uint64_t v) { dut.dp_vfalu_ex1_pipex_srcf0 = v; }, 64);
        toggle_bitfield([&](uint64_t v) { dut.dp_vfalu_ex1_pipex_srcf1 = v; }, 64);
        toggle_bitfield([&](uint64_t v) { dut.ex3_result = v; }, 64);
        toggle_bitfield([&](uint64_t v) { dut.half_result = v & 0xFFFF; }, 16);
        toggle_bitfield([&](uint64_t v) { dut.ex3_expt = v & 0x1F; }, 5);
        toggle_bitfield([&](uint64_t v) { dut.half_expt = v & 0x1F; }, 5);
    };

    auto drive_and_capture = [&](const Scenario& sc) -> bool {
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_func =
            make_func(sc.is_double, sc.is_single, sc.op_add, sc.op_sub,
                      sc.op_cmp, sc.op_maxnm, sc.op_minnm, sc.cmp_flags);
        dut.dp_vfalu_ex1_pipex_imm0 = sc.imm_rm & 0x7;
        dut.vfpu_yy_xx_rm = sc.ext_rm & 0x7;
        dut.ex1_doub_cmp_result = sc.cmp_doub;
        dut.ex1_doub_half_cmp_result = sc.cmp_half;

        // EX1 -> EX2 stage
        dut.ex1_pipedown = 1;
        tick(dut);
        dut.ex1_pipedown = 0;

        if (!expect(dut.ex2_double == sc.is_double, std::string(sc.name) + " ex2_double mismatch")) return false;
        if (!expect(dut.ex2_single == sc.is_single, std::string(sc.name) + " ex2_single mismatch")) return false;
        if (!expect(dut.ex2_op_add == sc.op_add, std::string(sc.name) + " ex2_op_add mismatch")) return false;
        if (!expect(dut.ex2_op_sub == sc.op_sub, std::string(sc.name) + " ex2_op_sub mismatch")) return false;
        if (!expect(dut.ex2_op_cmp == sc.op_cmp, std::string(sc.name) + " ex2_op_cmp mismatch")) return false;
        if (!expect(dut.ex2_op_maxnm == sc.op_maxnm, std::string(sc.name) + " ex2_op_maxnm mismatch")) return false;
        if (!expect(dut.ex2_op_minnm == sc.op_minnm, std::string(sc.name) + " ex2_op_minnm mismatch")) return false;
        if (!expect(dut.ex2_op_flt == static_cast<bool>(sc.cmp_flags & (1 << 1)),
                    std::string(sc.name) + " ex2_op_flt mismatch")) return false;
        if (!expect(dut.ex2_op_fle == static_cast<bool>(sc.cmp_flags & (1 << 2)),
                    std::string(sc.name) + " ex2_op_fle mismatch")) return false;
        if (!expect(dut.ex1_op_cmp == sc.op_cmp, std::string(sc.name) + " ex1_op_cmp mismatch")) return false;
        if (!expect(dut.ex1_op_add == sc.op_add, std::string(sc.name) + " ex1_op_add mismatch")) return false;
        if (!expect(dut.ex1_op_sub == sc.op_sub, std::string(sc.name) + " ex1_op_sub mismatch")) return false;
        if (!expect(dut.ex1_double == sc.is_double, std::string(sc.name) + " ex1_double mismatch")) return false;
        if (!expect(dut.ex1_single == sc.is_single, std::string(sc.name) + " ex1_single mismatch")) return false;
        if (!expect(dut.ex1_op_feq == static_cast<bool>(sc.cmp_flags & 0x1),
                    std::string(sc.name) + " ex1_op_feq mismatch")) return false;
        if (!expect(dut.ex1_op_fne == static_cast<bool>(sc.cmp_flags & 0x10),
                    std::string(sc.name) + " ex1_op_fne mismatch")) return false;
        if (!expect(dut.ex1_op_ford == static_cast<bool>(sc.cmp_flags & 0x8),
                    std::string(sc.name) + " ex1_op_ford mismatch")) return false;

        // Rounding mode propagation
        const uint8_t rm_effective = (sc.imm_rm == 0b111) ? (sc.ext_rm & 0x7) : (sc.imm_rm & 0x7);
        if (!expect(dut.ex2_rm_rne == (rm_effective == 0b000), std::string(sc.name) + " ex2_rm_rne mismatch")) return false;
        if (!expect(dut.ex2_rm_rtz == (rm_effective == 0b001), std::string(sc.name) + " ex2_rm_rtz mismatch")) return false;
        if (!expect(dut.ex2_rm_rdn == (rm_effective == 0b010), std::string(sc.name) + " ex2_rm_rdn mismatch")) return false;
        if (!expect(dut.ex2_rm_rup == (rm_effective == 0b011), std::string(sc.name) + " ex2_rm_rup mismatch")) return false;
        if (!expect(dut.ex2_rm_rmm == (rm_effective == 0b100), std::string(sc.name) + " ex2_rm_rmm mismatch")) return false;

        // EX2 -> EX3 stage
        dut.ex2_pipedown = 1;
        tick(dut);
        dut.ex2_pipedown = 0;

        // Drive EX3 inputs and latch outputs.
        dut.half_result = 0x1234;
        dut.half_expt = 0x1E;
        dut.ex3_result = 0xDEADBEEFCAFEBABEULL;
        dut.ex3_expt = 0x1F;
        dut.ex3_pipedown = 1;
        tick(dut);
        dut.ex3_pipedown = 0;

        const bool half_path = !sc.is_double && !sc.is_single;
        uint64_t expected_forward = half_path
                                        ? ((static_cast<uint64_t>(sc.op_cmp ? 0 : ~0ULL) & 0xFFFFFFFFFFFF0000ULL) |
                                           (dut.half_result & 0xFFFFULL))
                                        : dut.ex3_result;
        uint8_t expected_expt = half_path ? dut.half_expt : dut.ex3_expt;

        if (!expect(dut.fadd_forward_result == expected_forward,
                    std::string(sc.name) + " forward result mismatch")) return false;
        if (!expect(dut.fadd_ereg_ex3_result == expected_expt,
                    std::string(sc.name) + " forward exponent mismatch")) return false;
        if (!expect(dut.fadd_forward_r_vld == 1 && dut.fadd_ereg_ex3_forward_r_vld == 1,
                    std::string(sc.name) + " forward valid mismatch")) return false;

        // Verify cmp result muxing
        bool expected_cmp = (sc.is_double || sc.is_single) ? sc.cmp_doub : sc.cmp_half;
        if (!expect((dut.fadd_mfvr_cmp_result & 0x1ULL) == static_cast<uint64_t>(expected_cmp),
                    std::string(sc.name) + " cmp result mismatch")) return false;

        return true;
    };

    // Reset and enable clocks.
    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex3_pipedown = 0;
    dut.vfpu_yy_xx_rm = 0;
    dut.ex3_expt = 0;
    dut.ex3_result = 0;
    dut.half_expt = 0;
    dut.half_result = 0;
    dut.eval();
    tick(dut);
    dut.cpurst_b = 1;
    dut.eval();

    // Broad toggle sweep to cover data/ctrl bitfields.
    sweep_inputs_for_toggle_coverage();

    std::array<Scenario, 7> scenarios{{
        {"add_double_rne", true, false, true, false, false, false, false, 0, 0b000, 0b000, 0, 0, 0x3FF0000000000000ULL, 0x4000000000000000ULL},
        {"sub_single_rtz", false, true, false, true, false, false, false, 0, 0b001, 0b000, 1, 0, 0x4008000000000000ULL, 0x3FF8000000000000ULL},
        {"cmp_flt_single_rdn", false, true, false, false, true, false, false, 0b10, 0b010, 0b000, 0, 1, 0, 0},
        {"cmp_fle_double_rup", true, false, false, false, true, false, false, 0b100, 0b011, 0b000, 1, 0, 0, 0},
        {"cmp_feq_ford_fne", false, true, false, false, true, false, false, static_cast<uint8_t>(0b1 | 0b1000 | 0b10000), 0b000, 0b000, 1, 0, 0, 0},
        {"maxnm_double_rmm", true, false, false, false, false, true, false, 0, 0b100, 0b000, 0, 0, 0, 0},
        {"minnm_half_rm_override", false, false, false, false, false, false, true, 0, 0b111, 0b101, 1, 1, 0, 0},
    }};

    for (const auto& sc : scenarios) {
        if (!drive_and_capture(sc)) {
            return 1;
        }
    }

    // Toggle scan enable once for icg coverage.
    dut.pad_yy_icg_scan_en = 1;
    dut.eval();
    tick(dut);

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case008/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    std::cout << "ct_fadd_scalar_dp scenarios completed; coverage written to " << cov_path << "\n";
    return 0;
}
