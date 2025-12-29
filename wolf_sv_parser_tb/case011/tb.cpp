#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_double_dp.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace {

void tick(Vct_fcnvt_double_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

struct Scenario {
    const char* name;
    uint64_t src;
    bool ex1_src_double;
    bool ex1_src_single;
    bool ex1_src_float;
    bool ex1_src_si;
    bool ex1_src_l64;
    bool ex1_src_l32;
    bool ex1_src_l16;
    bool ex1_widden;
    bool ex1_narrow;
    bool ex1_sover;
    bool ex1_dest_double;
    bool ex1_dest_single;
    bool ex1_dest_float;
    bool ex2_dest_double;
    bool ex2_dest_single;
    bool ex2_dest_float;
    bool ex2_dest_half;
    bool ex2_dest_si;
    bool ex2_dest_l64;
    bool ex2_dest_l32;
    bool ex2_dest_l16;
    bool ex2_dest_l8;
    bool ex2_src_float;
    bool dqnan;
    std::array<bool, 5> rm;  // {rne, rtz, rup, rdn, rmm}
};

bool drive(Vct_fcnvt_double_dp& dut,
           const Scenario& sc,
           const std::function<bool(bool, const std::string&)>& expect) {
    dut.dp_ex1_src = sc.src;
    dut.vfpu_yy_xx_dqnan = sc.dqnan;
    dut.ex1_src_double = sc.ex1_src_double;
    dut.ex1_src_single = sc.ex1_src_single;
    dut.ex1_src_float = sc.ex1_src_float;
    dut.ex1_src_si = sc.ex1_src_si;
    dut.ex1_src_l64 = sc.ex1_src_l64;
    dut.ex1_src_l32 = sc.ex1_src_l32;
    dut.ex1_src_l16 = sc.ex1_src_l16;
    dut.ex1_widden = sc.ex1_widden;
    dut.ex1_narrow = sc.ex1_narrow;
    dut.ex1_sover = sc.ex1_sover;
    dut.ex1_dest_double = sc.ex1_dest_double;
    dut.ex1_dest_single = sc.ex1_dest_single;
    dut.ex1_dest_float = sc.ex1_dest_float;
    dut.ex1_pipedown = 1;
    dut.ex1_scalar = 1;
    dut.ex1_rm = (sc.rm[0] ? 1 : 0) | (sc.rm[1] ? 2 : 0) | (sc.rm[2] ? 4 : 0) |
                 (sc.rm[3] ? 8 : 0) | (sc.rm[4] ? 16 : 0);

    tick(dut);
    dut.ex1_pipedown = 0;

    dut.ex2_dest_double = sc.ex2_dest_double;
    dut.ex2_dest_single = sc.ex2_dest_single;
    dut.ex2_dest_float = sc.ex2_dest_float;
    dut.ex2_dest_half = sc.ex2_dest_half;
    dut.ex2_dest_si = sc.ex2_dest_si;
    dut.ex2_dest_l64 = sc.ex2_dest_l64;
    dut.ex2_dest_l32 = sc.ex2_dest_l32;
    dut.ex2_dest_l16 = sc.ex2_dest_l16;
    dut.ex2_dest_l8 = sc.ex2_dest_l8;
    dut.ex2_src_float = sc.ex2_src_float;
    dut.ex2_pipedown = 1;
    tick(dut);
    dut.ex2_pipedown = 0;

    tick(dut);

    if (!expect(dut.fcnvt_ex3_expt <= 0x1F, std::string(sc.name) + ": ex3_expt out of range")) {
        return false;
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_double_dp dut;

    auto expect = [&](bool cond, const std::string& msg) -> bool {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.vfpu_yy_xx_dqnan = 0;
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex1_scalar = 0;
    dut.forever_cpuclk = 0;
    dut.eval();
    tick(dut);
    dut.cpurst_b = 1;
    dut.eval();
    tick(dut);
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    auto base = [](const char* name, uint64_t src) {
        Scenario sc{};
        sc.name = name;
        sc.src = src;
        sc.rm = {true, false, false, false, false};
        sc.dqnan = false;
        return sc;
    };

    std::vector<Scenario> scenarios;

    {
        auto sc = base("double_to_single", 0x4008000000000000ULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_narrow = true;
        sc.ex1_dest_single = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_single = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.rm = {false, false, true, false, false};  // rup
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_int64", 0xC008000000000000ULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_src_l64 = true;
        sc.ex2_dest_si = true;
        sc.ex2_dest_l64 = true;
        sc.ex2_src_float = true;
        sc.rm = {false, true, false, false, false};  // rtz
        scenarios.push_back(sc);
    }

    {
        auto sc = base("int_to_double", 0x0000000000000005ULL);
        sc.ex1_src_si = true;
        sc.ex1_src_l64 = true;
        sc.ex1_widden = true;
        sc.ex1_dest_double = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = false;
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_half", 0x3FF0000000000000ULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_narrow = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_half = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.rm = {false, false, true, false, false};  // rup
        scenarios.push_back(sc);
    }

    {
        auto sc = base("single_to_double", 0x3F400000ULL);  // 0.75f
        sc.ex1_src_single = true;
        sc.ex1_src_float = true;
        sc.ex1_widden = true;
        sc.ex1_dest_double = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("single_to_int32_rdn", 0xBF800000ULL);  // -1.0f
        sc.ex1_src_single = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_src_l32 = true;
        sc.ex1_dest_float = false;
        sc.ex2_dest_l32 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {false, false, false, true, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("int16_to_single", 0x00000000000000FFULL);
        sc.ex1_src_si = true;
        sc.ex1_src_l16 = true;
        sc.ex1_widden = true;
        sc.ex1_dest_single = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_single = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = false;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_int8", 0x4022000000000000ULL);  // 9.0
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_dest_float = false;
        sc.ex2_dest_l8 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {false, false, false, false, true};  // rmm
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_tie_pos_int32", 0x3FF8000000000000ULL);  // 1.5
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_src_l32 = true;
        sc.ex2_dest_l32 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_tie_neg_int32", 0xBFF8000000000000ULL);  // -1.5
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_src_l32 = true;
        sc.ex2_dest_l32 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_int16_crit", 0x40DFFE0000000000ULL);  // 32767.5
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex2_dest_l16 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_uint16_crit", 0x40EFFFC000000000ULL);  // 65535.5
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = false;
        sc.ex2_dest_l16 = true;
        sc.ex2_dest_si = false;
        sc.ex2_src_float = true;
        sc.rm = {false, false, false, true, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("single_to_half_tie", 0x3F000000ULL);  // 0.5f
        sc.ex1_src_single = true;
        sc.ex1_src_float = true;
        sc.ex1_narrow = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_half = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_half_tie", 0x3FE0000000000000ULL);  // 0.5
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_narrow = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_half = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("snan_single_dqnan", 0x7FC00001ULL);
        sc.ex1_src_single = true;
        sc.ex1_src_float = true;
        sc.ex1_widden = true;
        sc.ex1_dest_double = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.dqnan = true;
        scenarios.push_back(sc);
    }

    {
        auto sc = base("sover_nan_dq", 0x7FF8000000000001ULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_sover = true;
        sc.ex1_narrow = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_half = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.dqnan = true;
        sc.rm = {false, true, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("widden_int32_to_double", 0xFFFFFFFF00000010ULL);  // negative
        sc.ex1_src_si = true;
        sc.ex1_src_l32 = true;
        sc.ex1_widden = true;
        sc.ex1_dest_double = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = false;
        sc.rm = {false, false, false, true, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("half_dest_overflow_rtz", 0x7FEFFFFFFFFFFFFFULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_narrow = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_half = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.rm = {false, true, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_inf_to_int32", 0x7FF0000000000000ULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_src_l32 = true;
        sc.ex2_dest_l32 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_snan_no_dq", 0x7FF0000000000001ULL);
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.dqnan = false;
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_neg_large_int16", 0xC4D2FA0000000000ULL);  // ~-1e9
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex2_dest_l16 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {false, false, false, true, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_pos_large_uint32", 0x43E158E460913D00ULL);  // ~1e20
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = false;
        sc.ex2_dest_l32 = true;
        sc.ex2_dest_si = false;
        sc.ex2_src_float = true;
        sc.rm = {false, false, true, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_neg_uint64_underflow", 0xC000000000000000ULL);  // -2.0
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = false;
        sc.ex2_dest_l64 = true;
        sc.ex2_dest_si = false;
        sc.ex2_src_float = true;
        sc.rm = {false, true, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_int16", 0x4049000000000000ULL);  // 50.0
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex2_dest_l16 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {true, false, false, false, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("double_to_uint16_overflow", 0x412E848000000000ULL);  // ~1e6
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = false;
        sc.ex2_dest_l16 = true;
        sc.ex2_dest_si = false;
        sc.ex2_src_float = true;
        sc.rm = {false, true, false, false, false};
        scenarios.push_back(sc);
    }

    auto add_int_case = [&](const char* name, uint64_t src, bool signed_int, int width_bits, const std::array<bool, 5>& rm, bool src_float = true) {
        Scenario sc = base(name, src);
        sc.ex1_src_double = true;
        sc.ex1_src_float = src_float;
        sc.ex1_src_si = signed_int;
        sc.ex1_src_l64 = width_bits == 64;
        sc.ex1_src_l32 = width_bits == 32;
        sc.ex1_src_l16 = width_bits == 16;
        sc.ex1_src_single = false;
        sc.ex1_dest_float = false;
        sc.ex2_dest_si = signed_int;
        sc.ex2_dest_l64 = width_bits == 64;
        sc.ex2_dest_l32 = width_bits == 32;
        sc.ex2_dest_l16 = width_bits == 16;
        sc.ex2_dest_l8 = width_bits == 8;
        sc.ex2_src_float = src_float;
        sc.rm = rm;
        scenarios.push_back(sc);
    };

    add_int_case("double_int32_pos_crit", 0x41E0000000000000ULL, true, 32, {true, false, false, false, false});
    add_int_case("double_int32_neg_crit", 0xC1E0000000000000ULL, true, 32, {true, false, false, false, false});
    add_int_case("double_int16_neg_uf", 0xC0E0000000000000ULL, true, 16, {false, false, false, true, false});
    add_int_case("double_int8_pos_crit", 0x4060000000000000ULL, true, 8, {true, false, false, false, false});
    add_int_case("double_uint32_pos_crit", 0x41F0000000000000ULL, false, 32, {false, false, false, true, false});
    add_int_case("double_uint64_large", 0x43F0000000000000ULL, false, 64, {false, true, false, false, false});
    add_int_case("double_uint64_neg", 0xBFF0000000000000ULL, false, 64, {false, false, false, false, true});

    {
        auto sc = base("snan_single_to_double", 0x7F800001ULL);
        sc.ex1_src_single = true;
        sc.ex1_src_float = true;
        sc.ex1_widden = true;
        sc.ex1_dest_double = true;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = true;
        sc.ex2_dest_float = true;
        sc.ex2_src_float = true;
        sc.dqnan = true;
        scenarios.push_back(sc);
    }

    {
        auto sc = base("neg_double_to_int32_min", 0xC24F800000000000ULL);  // -5e6
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = true;
        sc.ex1_src_l32 = true;
        sc.ex2_dest_l32 = true;
        sc.ex2_dest_si = true;
        sc.ex2_src_float = true;
        sc.rm = {false, false, false, true, false};
        scenarios.push_back(sc);
    }

    {
        auto sc = base("neg_double_to_uint8_uf", 0xC020000000000000ULL);  // -8.0
        sc.ex1_src_double = true;
        sc.ex1_src_float = true;
        sc.ex1_src_si = false;
        sc.ex2_dest_l8 = true;
        sc.ex2_dest_si = false;
        sc.ex2_src_float = true;
        sc.rm = {false, false, false, false, true};
        scenarios.push_back(sc);
    }

    uint32_t lcg = 123u;
    auto next_rand = [&]() {
        lcg = lcg * 1103515245u + 12345u;
        return lcg;
    };
    std::array<std::array<bool, 5>, 5> rm_sweep{{
        {true, false, false, false, false},
        {false, true, false, false, false},
        {false, false, true, false, false},
        {false, false, false, true, false},
        {false, false, false, false, true},
    }};
    for (int i = 0; i < 80; ++i) {
        auto sc = base("fuzz_mix", static_cast<uint64_t>(next_rand()) << 32 | next_rand());
        bool src_float = (i % 2) == 0;
        sc.ex1_src_double = src_float;
        sc.ex1_src_single = !src_float;
        sc.ex1_src_float = true;
        sc.ex1_src_si = (i % 3) == 0;
        sc.ex1_src_l16 = (i % 5) == 0;
        sc.ex1_src_l32 = (i % 7) == 0;
        sc.ex1_widden = sc.ex1_src_si;
        sc.ex1_narrow = src_float && ((i % 4) == 0);
        sc.ex1_sover = (i % 6) == 0;
        sc.ex1_dest_double = (i % 4) == 0;
        sc.ex1_dest_single = (i % 4) == 1;
        sc.ex1_dest_float = true;
        sc.ex2_dest_double = sc.ex1_dest_double;
        sc.ex2_dest_single = sc.ex1_dest_single;
        sc.ex2_dest_half = (i % 4) == 2;
        sc.ex2_dest_float = true;
        sc.ex2_dest_si = (i % 3) == 0;
        sc.ex2_dest_l64 = sc.ex2_dest_si && ((i % 2) == 0);
        sc.ex2_dest_l32 = sc.ex2_dest_si && ((i % 3) == 1);
        sc.ex2_dest_l16 = sc.ex2_dest_si && ((i % 3) == 2);
        sc.ex2_dest_l8 = sc.ex2_dest_si && ((i % 5) == 0);
        sc.ex2_src_float = src_float;
        sc.rm = rm_sweep[i % rm_sweep.size()];
        sc.dqnan = (i % 10) == 0;
        scenarios.push_back(sc);
    }

    for (const auto& sc : scenarios) {
        if (!drive(dut, sc, expect)) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case011/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
