#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_scalar_dp.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void tick(Vct_vfdsu_scalar_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.ex1_data_clk = 0;
    dut.ex2_data_clk = 0;
    dut.ex3_data_clk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.ex1_data_clk = 1;
    dut.ex2_data_clk = 1;
    dut.ex3_data_clk = 1;
    dut.eval();
}

struct Scenario {
    std::string name;
    bool gateclk_sel;
    uint32_t func;
    uint8_t imm0;
    uint64_t src0;
    uint64_t src1;
    uint8_t dst_ereg;
    uint8_t dst_vreg;
    uint8_t iid;
    uint8_t ex4_expt;
    uint64_t ex4_result;
    bool ex1_pd;
    bool ex2_pd;
    bool ex3_pd;
};

struct State {
    bool ex1_div{};
    bool ex1_sqrt{};
    bool ex1_double{};
    bool ex1_single{};
    uint8_t ex2_dst_ereg{};
    uint8_t ex2_dst_vreg{};
    uint8_t ex2_iid{};
    bool ex2_double{};
    bool ex2_single{};
    uint8_t ex3_dst_ereg{};
    uint8_t ex3_dst_vreg{};
    uint8_t ex3_iid{};
    uint8_t ex4_dst_ereg{};
    uint8_t ex4_dst_vreg{};
    uint8_t ex4_iid{};
};

void apply_step(const Scenario& sc, bool ex1_pd, bool ex2_pd, bool ex3_pd, State& st) {
    if (sc.gateclk_sel) {
        st.ex1_div = (sc.func & 0x1) != 0;
        st.ex1_sqrt = (sc.func & 0x2) != 0;
        st.ex1_double = (sc.func & (1u << 16)) != 0;
        st.ex1_single = (sc.func & (1u << 15)) != 0;
    }
    if (ex1_pd) {
        st.ex2_dst_ereg = sc.dst_ereg & 0x1f;
        st.ex2_dst_vreg = sc.dst_vreg & 0x7f;
        st.ex2_iid = sc.iid & 0x7f;
        st.ex2_double = st.ex1_double;
        st.ex2_single = st.ex1_single;
    }
    if (ex2_pd) {
        st.ex3_dst_ereg = st.ex2_dst_ereg;
        st.ex3_dst_vreg = st.ex2_dst_vreg;
        st.ex3_iid = st.ex2_iid;
    }
    if (ex3_pd) {
        st.ex4_dst_ereg = st.ex3_dst_ereg;
        st.ex4_dst_vreg = st.ex3_dst_vreg;
        st.ex4_iid = st.ex3_iid;
    }
}

bool drive_and_check(Vct_vfdsu_scalar_dp& dut, const Scenario& sc, State& st) {
    // Inputs
    dut.idu_vfpu_rf_pipex_gateclk_sel = sc.gateclk_sel;
    dut.idu_vfpu_rf_pipex_func = sc.func;
    dut.dp_vfdsu_ex1_pipex_imm0 = sc.imm0 & 0x7;
    dut.dp_vfdsu_ex1_pipex_srcf0 = sc.src0;
    dut.dp_vfdsu_ex1_pipex_srcf1 = sc.src1;
    dut.dp_vfdsu_ex1_pipex_dst_ereg = sc.dst_ereg & 0x1f;
    dut.dp_vfdsu_ex1_pipex_dst_vreg = sc.dst_vreg & 0x7f;
    dut.dp_vfdsu_ex1_pipex_iid = sc.iid & 0x7f;
    dut.ex4_out_expt = sc.ex4_expt & 0x1f;
    dut.ex4_out_result = sc.ex4_result;

    // Step 1: update ex1 stage
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex3_pipedown = 0;
    tick(dut);
    State new_st = st;
    apply_step(sc, false, false, false, new_st);

    // Step 2: ex1 -> ex2
    dut.ex1_pipedown = sc.ex1_pd;
    dut.ex2_pipedown = 0;
    dut.ex3_pipedown = 0;
    tick(dut);
    apply_step(sc, sc.ex1_pd, false, false, new_st);

    // Step 3: ex2 -> ex3
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = sc.ex2_pd;
    dut.ex3_pipedown = 0;
    tick(dut);
    apply_step(sc, false, sc.ex2_pd, false, new_st);

    // Step 4: ex3 -> ex4
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex3_pipedown = sc.ex3_pd;
    tick(dut);
    apply_step(sc, false, false, sc.ex3_pd, new_st);

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) std::cerr << msg << "\n";
        return cond;
    };

    bool ok = true;
    ok &= expect(dut.ex1_div == new_st.ex1_div, sc.name + " ex1_div mismatch");
    ok &= expect(dut.ex1_sqrt == new_st.ex1_sqrt, sc.name + " ex1_sqrt mismatch");
    ok &= expect(dut.ex1_double == new_st.ex1_double, sc.name + " ex1_double mismatch");
    ok &= expect(dut.ex1_single == new_st.ex1_single, sc.name + " ex1_single mismatch");
    ok &= expect(dut.ex1_scalar == 1, sc.name + " ex1_scalar mismatch");
    ok &= expect(dut.ex1_static_rm == (sc.imm0 & 0x7), sc.name + " ex1_static_rm mismatch");
    ok &= expect(dut.ex1_src0 == sc.src0, sc.name + " ex1_src0 mismatch");
    ok &= expect(dut.ex1_src1 == sc.src1, sc.name + " ex1_src1 mismatch");

    ok &= expect(dut.vfdsu_ex2_double == new_st.ex2_double, sc.name + " ex2_double mismatch");
    ok &= expect(dut.vfdsu_ex2_single == new_st.ex2_single, sc.name + " ex2_single mismatch");

    ok &= expect(dut.pipex_dp_vfdsu_ereg == new_st.ex4_dst_ereg, sc.name + " pipex_ereg mismatch");
    ok &= expect(dut.pipex_dp_vfdsu_vreg == new_st.ex4_dst_vreg, sc.name + " pipex_vreg mismatch");
    ok &= expect(dut.pipex_dp_vfdsu_ereg_data == (sc.ex4_expt & 0x1f), sc.name + " pipex_ereg_data mismatch");
    ok &= expect(dut.pipex_dp_vfdsu_freg_data == sc.ex4_result, sc.name + " pipex_freg_data mismatch");

    if (ok) st = new_st;
    return ok;
}

uint32_t make_func(bool div, bool sqrt, bool dbl, bool sgl) {
    uint32_t f = 0;
    if (div) f |= 1u;
    if (sqrt) f |= 1u << 1;
    if (sgl) f |= 1u << 15;
    if (dbl) f |= 1u << 16;
    return f;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_scalar_dp dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;
    tick(dut);

    State st{};
    std::vector<Scenario> scenarios = {
        {"basic_double_div", true, make_func(true, false, true, false), 0b101, 0x1122334455667788ULL, 0x8877665544332211ULL, 3, 5, 7, 0x1A, 0xABCDEF1234567890ULL, true, true, true},
        {"single_sqrt", true, make_func(false, true, false, true), 0b010, 0xDEADBEEFF00DBAADULL, 0x0BADF00DDEADCAFEULL, 4, 6, 8, 0x0F, 0x12345678ULL, true, true, true},
        {"hold_no_pipedown", true, make_func(false, false, true, false), 0b111, 0xCAFEBABECAFEBABEULL, 0x0, 9, 10, 11, 0x05, 0xFEDCBA9876543210ULL, false, false, false},
        {"gate_off_hold", false, make_func(true, true, true, true), 0b001, 0x1111111111111111ULL, 0x2222222222222222ULL, 12, 13, 14, 0x10, 0xAAAAAAAAAAAAAAAAULL, false, true, true},
        {"update_after_hold", true, make_func(true, false, false, true), 0b011, 0x3333333333333333ULL, 0x4444444444444444ULL, 15, 16, 17, 0x1E, 0x5555555555555555ULL, true, false, true},
    };

    for (const auto& sc : scenarios) {
        if (!drive_and_check(dut, sc, st)) return 1;
    }

    // Randomized scenarios to exercise hold/update paths
    uint64_t lcg = 2025;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 200; ++i) {
        Scenario sc{
            "rand_" + std::to_string(i),
            (next() & 1) != 0,
            static_cast<uint32_t>(next()),
            static_cast<uint8_t>(next() & 0x7),
            next(),
            next(),
            static_cast<uint8_t>(next() & 0x1f),
            static_cast<uint8_t>(next() & 0x7f),
            static_cast<uint8_t>(next() & 0x7f),
            static_cast<uint8_t>(next() & 0x1f),
            next(),
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0};
        if (!drive_and_check(dut, sc, st)) return 1;
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case037/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
