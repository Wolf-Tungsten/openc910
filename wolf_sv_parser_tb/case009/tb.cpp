#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_double_dp.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void tick(Vct_fadd_double_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

struct Scenario {
    const char* name;
    bool ex1_double;
    bool ex1_single;
    bool ex1_as_double;
    bool ex1_as_single;
    bool ex1_scalar;
    bool op_add;
    bool op_sub;
    bool op_cmp;
    bool op_maxnm;
    bool op_minnm;
    bool op_feq;
    bool op_fle;
    bool op_flt;
    bool op_fne;
    bool op_ford;
    bool ex2_double;
    bool ex2_single;
    bool ex2_rm_rne;
    bool ex2_rm_rtz;
    bool ex2_rm_rdn;
    bool ex2_rm_rup;
    bool ex2_rm_rmm;
    uint64_t src0;
    uint64_t src1;
    bool cmp_result;
};

uint64_t make_nan(bool quiet, bool sign) {
    uint64_t payload = 0x12345;
    uint64_t exp = 0x7FFull;
    uint64_t frac = (quiet ? (1ull << 51) : 0) | (payload & ((1ull << 51) - 1));
    return (sign ? (1ull << 63) : 0) | (exp << 52) | frac;
}

uint64_t make_double(bool sign, uint16_t exp, uint64_t frac) {
    return (sign ? (1ull << 63) : 0) | (static_cast<uint64_t>(exp & 0x7FF) << 52) | (frac & ((1ull << 52) - 1));
}

uint64_t make_lfn(bool sign) {
    return make_double(sign, 0x7FE, (1ull << 52) - 1);
}

uint64_t make_denorm(bool sign, uint64_t frac) {
    return make_double(sign, 0x000, frac & ((1ull << 52) - 1));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_double_dp dut;

    auto expect = [&](bool cond, const std::string& msg) -> bool {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    auto drive_pipe = [&](const Scenario& sc) -> bool {
        dut.ex1_double = sc.ex1_double;
        dut.ex1_single = sc.ex1_single;
        dut.ex1_as_double = sc.ex1_as_double;
        dut.ex1_as_single = sc.ex1_as_single;
        dut.ex1_scalar = sc.ex1_scalar;
        dut.ex1_op_add = sc.op_add;
        dut.ex1_op_sub = sc.op_sub;
        dut.ex1_op_cmp = sc.op_cmp;
        dut.ex1_op_maxnm = sc.op_maxnm;
        dut.ex1_op_minnm = sc.op_minnm;
        dut.ex1_op_feq = sc.op_feq;
        dut.ex1_op_fle = sc.op_fle;
        dut.ex1_op_flt = sc.op_flt;
        dut.ex1_op_fne = sc.op_fne;
        dut.ex1_op_ford = sc.op_ford;
        dut.fadd_ctrl_src0 = sc.src0;
        dut.fadd_ctrl_src1 = sc.src1;
        dut.ex1_cmp_result = sc.cmp_result;

        dut.ex1_pipedown = 1;
        tick(dut);
        dut.ex1_pipedown = 0;

        dut.ex2_double = sc.ex2_double;
        dut.ex2_single = sc.ex2_single;
        dut.ex2_op_add = sc.op_add;
        dut.ex2_op_sub = sc.op_sub;
        dut.ex2_op_cmp = sc.op_cmp;
        dut.ex2_op_maxnm = sc.op_maxnm;
        dut.ex2_op_minnm = sc.op_minnm;
        dut.ex2_op_fle = sc.op_fle;
        dut.ex2_op_flt = sc.op_flt;
        dut.ex2_rm_rne = sc.ex2_rm_rne;
        dut.ex2_rm_rtz = sc.ex2_rm_rtz;
        dut.ex2_rm_rdn = sc.ex2_rm_rdn;
        dut.ex2_rm_rup = sc.ex2_rm_rup;
        dut.ex2_rm_rmm = sc.ex2_rm_rmm;

        dut.ex2_pipedown = 1;
        tick(dut);
        dut.ex2_pipedown = 0;

        tick(dut);

        if (!expect(dut.ex3_expt <= 0x1F, std::string(sc.name) + " ex3_expt out of range")) return false;
        return true;
    };

    auto run_add_sweep = [&](bool single_path) {
        std::array<uint64_t, 6> pats{
            0x0ULL,
            0xFFFFFFFFFFFFFFFFULL,
            0x8000000000000000ULL,
            0x7FF0000000000000ULL,
            0x000FFFFFFFFFFFFFULL,
            0x3FF0000000000000ULL};
        std::array<std::array<bool, 5>, 5> rms{{
            {true, false, false, false, false},
            {false, true, false, false, false},
            {false, false, true, false, false},
            {false, false, false, true, false},
            {false, false, false, false, true},
        }};
        for (uint64_t a : pats) {
            uint64_t b = ~a;
            for (const auto& rm : rms) {
                Scenario sc{
                    "sweep_add",
                    !single_path, single_path,
                    !single_path, single_path,
                    true,
                    true, false, false, false, false,
                    false, false, false, false, false,
                    !single_path, single_path,
                    rm[0], rm[1], rm[2], rm[3], rm[4],
                    a, b, false};
                if (!drive_pipe(sc)) return false;
            }
        }
        return true;
    };

    auto run_special_matrix = [&]() {
        std::array<uint64_t, 10> specials{
            0x0ULL,
            0x8000000000000000ULL,
            make_denorm(false, 1),
            make_denorm(true, 3),
            make_double(false, 0x3FF, 0x123456789ABCULL),
            make_double(true, 0x3FF, 0x555555555555ULL),
            make_nan(true, false),
            make_nan(false, true),
            make_lfn(false),
            make_lfn(true)};
        std::array<std::array<bool, 5>, 5> rms{{
            {true, false, false, false, false},
            {false, true, false, false, false},
            {false, false, true, false, false},
            {false, false, false, true, false},
            {false, false, false, false, true},
        }};
        enum class Op { Add, Sub, Max, Min, Cmp };
        std::array<Op, 5> ops{Op::Add, Op::Sub, Op::Max, Op::Min, Op::Cmp};

        for (uint64_t a : specials) {
            for (uint64_t b : specials) {
                for (const auto& rm : rms) {
                    for (Op op : ops) {
                        bool add = op == Op::Add;
                        bool sub = op == Op::Sub;
                        bool maxnm = op == Op::Max;
                        bool minnm = op == Op::Min;
                        bool cmp = op == Op::Cmp;
                        Scenario sc{
                            "matrix",
                            true, false, true, false, true,
                            add, sub, cmp, maxnm, minnm,
                            cmp, cmp, cmp, cmp, cmp,
                            true, false,
                            rm[0], rm[1], rm[2], rm[3], rm[4],
                            a, b, (a == b)};
                        if (!drive_pipe(sc)) return false;
                    }
                }
            }
        }
        return true;
    };

    auto run_fuzz = [&]() {
        uint64_t lcg = 0x12345678ABCDEF00ULL;
        auto next = [&]() {
            lcg = lcg * 6364136223846793005ULL + 1;
            return lcg;
        };

        std::array<uint64_t, 16> specials{
            0x0ULL,
            0x8000000000000000ULL,
            0x7FF0000000000000ULL,
            0xFFF0000000000000ULL,
            0x3FF8000000000000ULL,
            0xBFF0000000000000ULL,
            0x000FFFFFFFFFFFFFULL,
            make_nan(true, false),
            make_nan(false, true),
            make_double(false, 0x001, 0x1),
            make_double(true, 0x001, 0x2),
            make_double(false, 0x7FE, 0x0),
            make_double(true, 0x7FE, 0x123456789ABCULL),
            make_double(false, 0x3FF, 0x123456789ABCULL),
            make_double(true, 0x200, 0xABCDEFULL),
            make_double(false, 0x100, 0x5555ULL)};

        for (int i = 0; i < 15000; ++i) {
            uint64_t a = specials[next() % specials.size()] ^ (next() & 0xFF);
            uint64_t b = specials[next() % specials.size()] ^ (next() << 8);
            unsigned op_sel = next() % 7;
            bool as_double = (next() & 1);
            bool as_single = (next() & 2);
            if (!as_double && !as_single) as_double = true;
            bool scalar = (next() & 4);
            bool rm_rne = (next() & 1);
            bool rm_rtz = (next() & 2);
            bool rm_rdn = (next() & 4);
            bool rm_rup = (next() & 8);
            bool rm_rmm = !(rm_rne || rm_rtz || rm_rdn || rm_rup);
            bool cmp = (op_sel == 2);
            bool add = (op_sel == 0);
            bool sub = (op_sel == 1);
            bool maxnm = (op_sel == 3);
            bool minnm = (op_sel == 4);
            bool cmp_ford_case = (op_sel == 5);
            bool cmp_fne_case = (op_sel == 6);
            bool feq = cmp && (next() & 1);
            bool fle = cmp && (next() & 2);
            bool flt = cmp && (next() & 4);
            bool fne = cmp ? (next() & 8) : cmp_fne_case;
            bool ford = cmp ? (next() & 16) : cmp_ford_case;
            bool cmp_res = (next() & 1);

            Scenario sc{
                "fuzz", as_double, as_single, as_double, as_single, scalar,
                add, sub, cmp, maxnm, minnm, feq, fle, flt, fne, ford,
                as_double, as_single,
                rm_rne, rm_rtz, rm_rdn, rm_rup, rm_rmm,
                a, b, cmp_res};
            if (!drive_pipe(sc)) return false;
        }
        return true;
    };

    auto run_targeted = [&]() {
        std::vector<Scenario> vec{
            {"lfn_vs_norm_add_rne", true, false, true, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, true, false, false, false, false,
             make_lfn(false), make_double(false, 0x400, 0x123456789ABCULL), false},
            {"lfn_vs_lfn_sub_rtz", true, false, true, false, true,
             false, true, false, false, false, false, false, false, false, false,
             true, false, false, true, false, false, false,
             make_lfn(true), make_lfn(false), false},
            {"lfn_vs_norm_rup_far_sel", true, false, true, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, false, false, false, true, false,
             make_lfn(false), make_double(true, 0x200, 0x1), false},
            {"snm_vs_denorm_rdn", true, false, true, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, false, false, true, false, false,
             make_denorm(false, 1), make_denorm(true, 2), false},
            {"snm_cmp", true, false, true, false, true,
             false, false, true, false, false, false, false, false, false, true,
             true, false, true, false, false, false, false,
             make_denorm(false, 3), make_denorm(false, 3), true},
            {"inf_vs_neg_inf", true, false, true, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, true, false, false, false, false,
             0x7FF0000000000000ULL, 0xFFF0000000000000ULL, false},
            {"cmp_ford_nan", true, false, true, false, true,
             false, false, true, false, false, false, false, false, false, true,
             true, false, true, false, false, false, false,
             make_nan(false, true), make_nan(true, false), false},
            {"cmp_fne_nan", true, false, true, false, true,
             false, false, true, false, false, false, false, false, true, false,
             true, false, true, false, false, false, false,
             make_nan(true, false), make_double(false, 0x3FF, 0x0), true},
            {"bypass_far_overflow_rtz", true, false, true, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, false, true, false, false, false,
             0x7FDFFFFFFFFFFFFFULL, 0x7FDFFFFFFFFFFFFFULL, false},
            {"bypass_close_path_rup", true, false, true, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, false, false, false, true, false,
             make_denorm(false, 5), make_denorm(false, 3), false},
            {"scalar_cnan_path", false, false, false, false, true,
             true, false, false, false, false, false, false, false, false, false,
             true, false, true, false, false, false, false,
             0x00000000FFFFFFFFULL, 0x00000000FFFFFFFFULL, false},
        };
        for (const auto& sc : vec) {
            if (!drive_pipe(sc)) return false;
        }
        return true;
    };

    // Reset and enable clocks.
    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.vfpu_yy_xx_dqnan = 0;
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex3_result = 0;
    dut.ex3_expt = 0;
    dut.eval();
    tick(dut);
    dut.cpurst_b = 1;
    dut.eval();
    dut.vfpu_yy_xx_dqnan = 1;
    tick(dut);
    dut.vfpu_yy_xx_dqnan = 0;
    tick(dut);

    if (!run_add_sweep(false) || !run_add_sweep(true)) {
        return 1;
    }
    if (!run_special_matrix()) {
        return 1;
    }
    if (!run_targeted()) {
        return 1;
    }
    if (!run_fuzz()) {
        return 1;
    }

    std::array<Scenario, 10> scenarios{{
        {"add_normals", true, false, true, false, true,
         true, false, false, false, false, false, false, false, false, false,
         true, false, true, false, false, false, false,
         0x3FF0000000000000ULL, 0x4000000000000000ULL, false},
        {"sub_normals", true, false, true, false, true,
         false, true, false, false, false, false, false, false, false, false,
         true, false, false, true, false, false, false,
         0x4010000000000000ULL, 0x4000000000000000ULL, false},
        {"cmp_feq", true, false, true, false, true,
         false, false, true, false, false, true, false, false, false, false,
         true, false, true, false, false, false, false,
         0x3FF0000000000000ULL, 0x3FF0000000000000ULL, true},
        {"cmp_flt", true, false, true, false, true,
         false, false, true, false, false, false, false, true, false, false,
         true, false, false, true, false, false, false,
         0x3FE0000000000000ULL, 0x3FF0000000000000ULL, true},
        {"maxnm_with_nan", true, false, true, false, true,
         false, false, false, true, false, false, false, false, false, false,
         true, false, true, false, false, false, false,
         make_nan(true, false), 0x7FF0000000000000ULL, false},
        {"minnm_with_nan", true, false, true, false, true,
         false, false, false, false, true, false, false, false, false, false,
         true, false, false, true, false, false, false,
         make_nan(false, true), make_nan(true, false), false},
        {"cmp_ford_fne_nan", true, false, true, false, false,
         false, false, true, false, false, false, false, false, true, true,
         true, false, true, false, false, false, false,
         make_nan(true, false), 0x3FF0000000000000ULL, false},
        {"single_path_add", false, true, false, true, true,
         true, false, false, false, false, false, false, false, false, false,
         false, true, false, true, false, false, false,
         0x3F8000003F800000ULL, 0x4000000040000000ULL, false},
        {"neg_inf_add", true, false, true, false, true,
         true, false, false, false, false, false, false, false, false, false,
         true, false, true, false, false, false, false,
         0xFFF0000000000000ULL, 0x3FF0000000000000ULL, false},
        {"mixed_sign_add", true, false, true, false, true,
         true, false, false, false, false, false, false, false, false, false,
         true, false, true, false, false, false, false,
         0xBFF8000000000000ULL, 0x4008000000000000ULL, false},
    }};

    for (const auto& sc : scenarios) {
        if (!drive_pipe(sc)) {
            return 1;
        }
    }

    dut.pad_yy_icg_scan_en = 1;
    dut.eval();
    tick(dut);

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case009/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    std::cout << "ct_fadd_double_dp scenarios completed; coverage written to "
              << cov_path << "\n";
    return 0;
}
