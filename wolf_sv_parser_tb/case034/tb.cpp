#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_prepare.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

void tick(Vct_vfdsu_prepare& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

struct FF1Res {
    uint64_t shift;
    uint16_t bin;
};

FF1Res ff1(uint64_t frac52) {
    frac52 &= ((1ULL << 52) - 1);
    if (frac52 == 0) return {0, 0x1fcc};
    for (int i = 51; i >= 0; --i) {
        if (frac52 & (1ULL << i)) {
            if (i == 51) {
                return {frac52, 0};
            }
            const int shift = 51 - i;
            const uint16_t bin = static_cast<uint16_t>(0x1fff - (50 - i));
            return {(frac52 << shift) & ((1ULL << 52) - 1), bin};
        }
    }
    return {0, 0};
}

struct Scenario {
    std::string name;
    bool ex1_div;
    bool ex1_sqrt;
    bool ex1_double;
    bool ex1_single;
    bool ex1_scalar;
    uint64_t src0;
    uint64_t src1;
    uint8_t static_rm;
    uint8_t vfpu_rm;
    bool dqnan;
};

struct Expected {
    uint64_t divisor;
    uint64_t remainder;
    bool result_zero;
    bool result_qnan;
    bool result_inf;
    bool result_sign;
    bool op0_norm;
    bool op1_norm;
    uint16_t expnt_add0;
    uint16_t expnt_add1;
    bool nv;
    bool dz;
    bool srt_skip;
    bool of_rm_lfn;
    bool qnan_sign;
    uint64_t qnan_f;
    uint8_t rm;
    bool div;
    bool sqrt;
    bool is_double;
    bool is_single;
};

template <typename T>
std::string fmt(const T& v) {
    std::ostringstream oss;
    if constexpr (std::is_same_v<T, bool>) {
        oss << (v ? 1 : 0);
    } else {
        oss << "0x" << std::hex << static_cast<unsigned long long>(v);
    }
    return oss.str();
}

uint64_t get_bits(uint64_t v, int hi, int lo) {
    const int width = hi - lo + 1;
    uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    return (v >> lo) & mask;
}

Expected compute_expected(const Scenario& sc) {
    const bool is_double = sc.ex1_double;
    const bool is_single = sc.ex1_single;
    const bool is_half = !is_double && !is_single;

    const uint64_t exp0 = is_double ? get_bits(sc.src0, 62, 52) : is_single ? get_bits(sc.src0, 30, 23) : get_bits(sc.src0, 14, 10);
    const uint64_t exp1 = is_double ? get_bits(sc.src1, 62, 52) : is_single ? get_bits(sc.src1, 30, 23) : get_bits(sc.src1, 14, 10);
    const uint64_t frac0 = is_double ? get_bits(sc.src0, 51, 0) : is_single ? get_bits(sc.src0, 22, 0) : get_bits(sc.src0, 9, 0);
    const uint64_t frac1 = is_double ? get_bits(sc.src1, 51, 0) : is_single ? get_bits(sc.src1, 22, 0) : get_bits(sc.src1, 9, 0);
    const bool op0_sign = is_double ? (sc.src0 >> 63) & 1ULL : is_single ? (sc.src0 >> 31) & 1ULL : (sc.src0 >> 15) & 1ULL;
    const bool op1_sign = is_double ? (sc.src1 >> 63) & 1ULL : is_single ? (sc.src1 >> 31) & 1ULL : (sc.src1 >> 15) & 1ULL;
    const bool oper0_high_all1 = is_single ? (get_bits(sc.src0, 63, 32) == 0xFFFFFFFFULL) : (get_bits(sc.src0, 63, 16) == 0xFFFFFFFFFFFFULL);
    const bool oper1_high_all1 = is_single ? (get_bits(sc.src1, 63, 32) == 0xFFFFFFFFULL) : (get_bits(sc.src1, 63, 16) == 0xFFFFFFFFFFFFULL);

    const bool exp0_max = is_double ? exp0 == 0x7FF : is_single ? exp0 == 0xFF : exp0 == 0x1F;
    const bool exp1_max = is_double ? exp1 == 0x7FF : is_single ? exp1 == 0xFF : exp1 == 0x1F;
    const bool exp0_zero = exp0 == 0;
    const bool exp1_zero = exp1 == 0;
    const bool frac0_all0 = frac0 == 0;
    const bool frac1_all0 = frac1 == 0;
    const bool frac0_msb = is_double ? (frac0 >> 51) & 1ULL : is_single ? (frac0 >> 22) & 1ULL : (frac0 >> 9) & 1ULL;
    const bool frac1_msb = is_double ? (frac1 >> 51) & 1ULL : is_single ? (frac1 >> 22) & 1ULL : (frac1 >> 9) & 1ULL;

    const bool op0_cnan = sc.ex1_scalar && !is_double && !oper0_high_all1;
    const bool op1_cnan = sc.ex1_scalar && !is_double && !oper1_high_all1;

    const bool op0_inf = exp0_max && frac0_all0 && !op0_cnan;
    const bool op1_inf = exp1_max && frac1_all0 && !op1_cnan;
    const bool op0_zero = exp0_zero && frac0_all0 && !op0_cnan;
    const bool op1_zero = exp1_zero && frac1_all0 && !op1_cnan;
    const bool op0_id = exp0_zero && !frac0_all0 && !op0_cnan;
    const bool op1_id = exp1_zero && !frac1_all0 && !op1_cnan;

    const bool op0_snan = exp0_max && !frac0_all0 && !frac0_msb && !op0_cnan;
    const bool op1_snan = exp1_max && !frac1_all0 && !frac1_msb && !op1_cnan;
    const bool op0_qnan = (exp0_max && frac0_msb) || op0_cnan;
    const bool op1_qnan = (exp1_max && frac1_msb) || op1_cnan;

    const uint64_t op0_f_bits = op0_cnan ? 0 : (sc.src0 & ((1ULL << 52) - 1));
    const uint64_t op1_f_bits = op1_cnan ? 0 : (sc.src1 & ((1ULL << 52) - 1));

    const FF1Res ff1_op0 = ff1(is_double ? frac0 : is_single ? (frac0 << 29) : (frac0 << 42));
    const FF1Res ff1_op1 = ff1(is_double ? frac1 : is_single ? (frac1 << 29) : (frac1 << 42));

    const uint64_t oper0_frac = is_double ? frac0 : is_single ? (frac0 << 29) : (frac0 << 42);
    const uint64_t oper1_frac = is_double ? frac1 : is_single ? (frac1 << 29) : (frac1 << 42);

    const uint16_t div_op0_expnt = is_double ? static_cast<uint16_t>(exp0) : is_single ? static_cast<uint16_t>(exp0) : static_cast<uint16_t>(exp0);
    const uint16_t div_op1_expnt = is_double ? static_cast<uint16_t>(exp1) : is_single ? static_cast<uint16_t>(exp1) : static_cast<uint16_t>(exp1);

    const uint16_t op0_id_expnt = static_cast<uint16_t>(ff1_op0.bin);
    const uint16_t op1_id_expnt = static_cast<uint16_t>(ff1_op1.bin);
    const uint16_t expnt_adder_op0 = op0_id ? op0_id_expnt : div_op0_expnt;
    uint16_t expnt_adder_op1 = 0;
    if (sc.ex1_div) {
        expnt_adder_op1 = op1_id ? op1_id_expnt : div_op1_expnt;
    } else if (sc.ex1_sqrt) {
        expnt_adder_op1 = is_double ? 0x3FF : is_single ? 0x7F : 0x0F;
    }

    const bool op0_norm = (!exp0_zero && !exp0_max && !op0_cnan) || op0_id;
    const bool op1_norm = (!exp1_zero && !exp1_max && !op1_cnan) || op1_id;

    const bool op0_id_nor = op0_id;
    const bool op1_id_nor = op1_id;

    const uint64_t div_noid_op0 = is_double ? ((1ULL << 52) | frac0) : is_single ? ((1ULL << 52) | (frac0 << 29)) : ((1ULL << 52) | (frac0 << 42));
    const uint64_t div_noid_op1 = is_double ? ((1ULL << 52) | frac1) : is_single ? ((1ULL << 52) | (frac1 << 29)) : ((1ULL << 52) | (frac1 << 42));
    const uint64_t div_srt_op0 = op0_id_nor ? ((ff1_op0.shift << 1) & ((1ULL << 53) - 1)) : div_noid_op0;
    const uint64_t div_srt_op1 = op1_id_nor ? ((ff1_op1.shift << 1) & ((1ULL << 53) - 1)) : div_noid_op1;

    const bool div_nv = op0_snan || op1_snan || (op0_zero && op1_zero) || (op0_inf && op1_inf);
    const bool sqrt_nv = op0_snan || (op0_sign && (op0_norm || op0_inf));
    const bool ex1_nv = (sc.ex1_div && div_nv) || (sc.ex1_sqrt && sqrt_nv);

    const bool op1_tt_zero = op1_zero;
    const bool op0_tt_zero = op0_zero;
    const bool ex1_dz = sc.ex1_div && op1_tt_zero && op0_norm;

    const bool div_rst_qnan = op0_qnan || op1_qnan;
    const bool sqrt_rst_qnan = op0_qnan;
    const bool ex1_result_qnan = (sc.ex1_div && div_rst_qnan) || (sc.ex1_sqrt && sqrt_rst_qnan) || ex1_nv;

    const bool div_rst_inf = op0_inf && !exp1_max && !op1_cnan;
    const bool sqrt_rst_inf = op0_inf && !op0_sign;
    const bool ex1_result_inf = (sc.ex1_div && div_rst_inf) || (sc.ex1_sqrt && sqrt_rst_inf) || ex1_dz;

    const bool div_rst_zero = (op0_tt_zero && op1_norm) || (!exp0_max && !op0_cnan && op1_inf);
    const bool sqrt_rst_zero = op0_tt_zero;
    const bool ex1_result_zero = (sc.ex1_div && div_rst_zero) || (sc.ex1_sqrt && sqrt_rst_zero);

    const bool ex1_rst_default_qnan = (sc.ex1_div && op0_zero && op1_zero) || (sc.ex1_div && op0_inf && op1_inf) || (sc.ex1_sqrt && op0_sign && (op0_norm || op0_inf));

    const bool ex1_srt_skip = ex1_result_zero || ex1_result_qnan || ex1_result_inf;
    const bool ex1_result_sign = sc.ex1_div ? (op0_sign ^ op1_sign) : op0_sign;

    const uint8_t ex1_rm = ((sc.static_rm & 0x7) == 0x7 || !sc.ex1_scalar) ? (sc.vfpu_rm & 0x7) : (sc.static_rm & 0x7);

    bool ex1_of_result_lfn = false;
    switch (ex1_rm) {
        case 0b001: ex1_of_result_lfn = true; break;
        case 0b010: ex1_of_result_lfn = !ex1_result_sign; break;
        case 0b011: ex1_of_result_lfn = ex1_result_sign; break;
        default: ex1_of_result_lfn = false; break;
    }

    uint64_t qnan_f = 0;
    if (ex1_rst_default_qnan) {
        qnan_f = 1ULL << 51;
    } else if (op0_snan && sc.dqnan) {
        qnan_f = op0_f_bits;
    } else if (op1_snan && sc.dqnan && sc.ex1_div) {
        qnan_f = op1_f_bits;
    } else if (op0_qnan && sc.dqnan) {
        qnan_f = op0_f_bits;
    } else if (op1_qnan && sc.dqnan && sc.ex1_div) {
        qnan_f = op1_f_bits;
    } else {
        qnan_f = 1ULL << 51;
    }

    bool qnan_sign = false;
    if (ex1_rst_default_qnan) {
        qnan_sign = false;
    } else if (op0_snan && sc.dqnan) {
        qnan_sign = op0_sign;
    } else if (op1_snan && sc.dqnan && sc.ex1_div) {
        qnan_sign = op1_sign;
    } else if (op0_qnan && sc.dqnan) {
        qnan_sign = op0_sign && !op0_cnan;
    } else if (op1_qnan && sc.dqnan && sc.ex1_div) {
        qnan_sign = op1_sign && !op1_cnan;
    } else {
        qnan_sign = false;
    }

    const uint64_t srt_op0_53 = div_srt_op0 & ((1ULL << 53) - 1);
    uint64_t remainder = 0;
    if (sc.ex1_div) {
        remainder = srt_op0_53 << 2;
    } else if (sc.ex1_sqrt) {
        const bool expnt_odd = ((expnt_adder_op0 ^ expnt_adder_op1) & 1) != 0;
        remainder = srt_op0_53 << (expnt_odd ? 2 : 1);
    }

    return Expected{
        div_srt_op1 & ((1ULL << 53) - 1),
        remainder,
        ex1_result_zero,
        ex1_result_qnan,
        ex1_result_inf,
        ex1_result_sign,
        op0_norm,
        op1_norm,
        expnt_adder_op0,
        expnt_adder_op1,
        ex1_nv,
        ex1_dz,
        ex1_srt_skip,
        ex1_of_result_lfn,
        qnan_sign,
        qnan_f,
        ex1_rm,
        sc.ex1_div,
        sc.ex1_sqrt,
        is_double,
        is_single};
}

bool drive_and_check(Vct_vfdsu_prepare& dut, const Scenario& sc, int& total, int& failed) {
    ++total;
    const Expected exp = compute_expected(sc);

    dut.ex1_div = sc.ex1_div;
    dut.ex1_sqrt = sc.ex1_sqrt;
    dut.ex1_double = sc.ex1_double;
    dut.ex1_single = sc.ex1_single;
    dut.ex1_scalar = sc.ex1_scalar;
    dut.ex1_src0 = sc.src0;
    dut.ex1_src1 = sc.src1;
    dut.ex1_pipedown = 1;
    dut.ex1_static_rm = sc.static_rm & 0x7;
    dut.vfpu_yy_xx_rm = sc.vfpu_rm & 0x7;
    dut.vfpu_yy_xx_dqnan = sc.dqnan ? 1 : 0;

    tick(dut);
    dut.ex1_pipedown = 0;
    tick(dut);

    auto expect = [&](auto got, auto expected, const std::string& msg) {
        if (got != expected) {
            std::cerr << "[FAIL] " << sc.name << " " << msg << " got=" << fmt(got) << " expected=" << fmt(expected) << "\n";
            return false;
        }
        return true;
    };

    bool ok = true;
    ok &= expect(dut.ex1_divisor, exp.divisor, "divisor mismatch");
    ok &= expect(dut.ex1_remainder, exp.remainder, "remainder mismatch");
    ok &= expect(dut.vfdsu_ex2_result_zero, exp.result_zero, "result_zero mismatch");
    ok &= expect(dut.vfdsu_ex2_result_qnan, exp.result_qnan, "result_qnan mismatch");
    ok &= expect(dut.vfdsu_ex2_result_inf, exp.result_inf, "result_inf mismatch");
    ok &= expect(dut.vfdsu_ex2_result_sign, exp.result_sign, "result_sign mismatch");
    ok &= expect(dut.vfdsu_ex2_op0_norm, exp.op0_norm, "op0_norm mismatch");
    ok &= expect(dut.vfdsu_ex2_op1_norm, exp.op1_norm, "op1_norm mismatch");
    ok &= expect(dut.vfdsu_ex2_expnt_add0, exp.expnt_add0, "expnt_add0 mismatch");
    ok &= expect(dut.vfdsu_ex2_expnt_add1, exp.expnt_add1, "expnt_add1 mismatch");
    ok &= expect(dut.vfdsu_ex2_nv, exp.nv, "nv mismatch");
    ok &= expect(dut.vfdsu_ex2_dz, exp.dz, "dz mismatch");
    ok &= expect(dut.vfdsu_ex2_srt_skip, exp.srt_skip, "srt_skip mismatch");
    ok &= expect(dut.vfdsu_ex2_of_rm_lfn, exp.of_rm_lfn, "of_rm_lfn mismatch");
    ok &= expect(dut.vfdsu_ex2_qnan_sign, exp.qnan_sign, "qnan_sign mismatch");
    ok &= expect(dut.vfdsu_ex2_qnan_f, exp.qnan_f, "qnan_f mismatch");
    ok &= expect(dut.vfdsu_ex2_rm, exp.rm, "rm mismatch");
    ok &= expect(dut.vfdsu_ex2_div, exp.div, "div mismatch");
    ok &= expect(dut.vfdsu_ex2_sqrt, exp.sqrt, "sqrt mismatch");
    ok &= expect(dut.vfdsu_ex2_double, exp.is_double, "double mismatch");
    ok &= expect(dut.vfdsu_ex2_single, exp.is_single, "single mismatch");
    if (ok) {
        std::cout << "[PASS] " << sc.name << "\n";
    } else {
        ++failed;
        std::cerr << "[RESULT] " << sc.name << " FAILED\n";
    }
    return ok;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_prepare dut;
    int total = 0;
    int failed = 0;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    std::vector<Scenario> scenarios = {
        {"div_double_norm", true, false, true, false, true, 0x4008000000000000ULL, 0x4000000000000000ULL, 0, 0, false},
        {"div_zero_zero", true, false, true, false, true, 0x0ULL, 0x0ULL, 1, 2, true},
        {"div_by_zero", true, false, true, false, true, 0x3FF0000000000000ULL, 0x0ULL, 0, 3, false},
        {"sqrt_negative", false, true, true, false, true, 0xBFF0000000000000ULL, 0, 0, 4, true},
        {"sqrt_pos_inf", false, true, true, false, true, 0x7FF0000000000000ULL, 0, 0, 5, false},
        {"single_cnan_div", true, false, false, true, true, 0x000000007F800000ULL, 0xFFFFFFFF3F800000ULL, 0, 6, true},
        {"half_denorm_div", true, false, false, false, true, 0xFFFFFFFFFFFF0001ULL, 0xFFFFFFFFFFFF3C00ULL, 0, 0, false},
        {"div_zero_norm", true, false, true, false, true, 0x0ULL, 0x3FF0000000000000ULL, 0, 0, false},
        {"div_op1_inf", true, false, true, false, true, 0x3FF0000000000000ULL, 0x7FF0000000000000ULL, 0, 0, false},
        {"div_op1_cnan_single", true, false, false, true, true, 0x3F800000ULL, 0x0000000000000001ULL, 0, 1, false},
        {"div_op1_snan_dqnan", true, false, false, true, true, 0x3F800000ULL, 0xFFFFFFFF7FA00001ULL, 0, 2, true},
        {"div_op1_qnan_dqnan", true, false, false, true, true, 0x3F800000ULL, 0xFFFFFFFF7FC00001ULL, 0, 3, true},
        {"rm_rdn_negative", true, false, true, false, true, 0xBFF8000000000000ULL, 0x3FF0000000000000ULL, 3, 4, false},
        {"div_op1_snan_double_dqnan", true, false, true, false, true, 0x3FF0000000000000ULL, 0xFFF4000000000000ULL, 0, 0, true},
        {"div_op1_qnan_double_dqnan", true, false, true, false, true, 0x3FF0000000000000ULL, 0xFFF8000000000001ULL, 0, 0, true},
    };

    for (int bit = 0; bit < 52; ++bit) {
        const uint64_t frac = 1ULL << bit;
        scenarios.push_back({"ff1_op0_bit" + std::to_string(bit), true, false, true, false, true, frac, 0x3FF0000000000000ULL, 0, 0, false});
        scenarios.push_back({"ff1_op1_bit" + std::to_string(bit), true, false, true, false, true, 0x3FF0000000000000ULL, frac, 0, 0, false});
    }

    for (const auto& sc : scenarios) {
        drive_and_check(dut, sc, total, failed);
    }

    uint64_t lcg = 23;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 500; ++i) {
        Scenario sc{
            "rand_" + std::to_string(i),
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            next(),
            next(),
            static_cast<uint8_t>(next() & 0x7),
            static_cast<uint8_t>(next() & 0x7),
            (next() & 1) != 0};
        if (sc.ex1_div == sc.ex1_sqrt) {
            sc.ex1_div = true;
            sc.ex1_sqrt = false;
        }
        if (sc.ex1_double && sc.ex1_single) sc.ex1_single = false;
        drive_and_check(dut, sc, total, failed);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case034/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    const int passed = total - failed;
    if (failed == 0) {
        std::cout << "[RESULT] PASS (" << passed << "/" << total << ")\n";
        return 0;
    }
    std::cerr << "[RESULT] FAIL (" << passed << "/" << total << ")\n";
    return 1;
}
