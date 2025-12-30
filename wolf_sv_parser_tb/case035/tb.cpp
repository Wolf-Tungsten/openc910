#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_pack.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint64_t get_bits(uint64_t v, int hi, int lo) {
    const int width = hi - lo + 1;
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    return (v >> lo) & mask;
}

struct Scenario {
    std::string name;
    bool denorm_to_tiny_frac;
    bool double_mode;
    bool single_mode;
    bool dz;
    uint16_t expnt_rst;
    uint64_t frac;
    bool nv;
    bool nx;
    bool of;
    bool of_rst_lfn;
    uint8_t potnt_norm;
    bool potnt_of;
    bool potnt_uf;
    uint64_t qnan_f;
    bool qnan_sign;
    bool result_inf;
    bool result_lfn;
    bool result_nor;
    bool result_qnan;
    bool result_sign;
    bool result_zero;
    bool rslt_denorm;
    bool uf;
};

struct Expected {
    uint64_t out_result;
    uint8_t out_expt;
};

uint64_t calc_denorm_frac(uint16_t expnt, uint64_t frac, bool tiny, uint16_t min_case) {
    if (expnt == 1) return get_bits(frac, 52, 1);
    if (expnt == 0) return get_bits(frac, 53, 2);
    if (expnt >= min_case && expnt <= 0x1fff) {
        const uint16_t zeros = static_cast<uint16_t>(0x1fff - expnt);
        const int lo = 3 + zeros;
        if (lo > 54) return 0;
        return get_bits(frac, 54, lo);
    }
    if (tiny) {
        if (min_case == 0x1fcd) return 1ULL;                   // double default
        if (min_case == 0x1fea) return (1ULL << 29);           // single default
        return (1ULL << 42);                                   // half default
    }
    return 0;
}

Expected compute_expected(const Scenario& sc) {
    const bool is_double = sc.double_mode;
    const bool is_single = sc.single_mode;
    const uint64_t frac = sc.frac & ((1ULL << 55) - 1);

    const uint8_t frac_hi = static_cast<uint8_t>((frac >> 53) & 0x3);
    uint16_t expnt_add_op1 = 0;
    switch (frac_hi) {
        case 0b00: expnt_add_op1 = 0x0fff; break;
        case 0b01: expnt_add_op1 = 0; break;
        default: expnt_add_op1 = 1; break;
    }
    const uint16_t ex4_expnt_rst = static_cast<uint16_t>((sc.expnt_rst + expnt_add_op1) & 0x1fff);

    const uint64_t ex4_denorm_frac = calc_denorm_frac(ex4_expnt_rst, frac, sc.denorm_to_tiny_frac, 0x1fcd) & ((1ULL << 52) - 1);
    const uint64_t ex4_single_denorm_frac = calc_denorm_frac(ex4_expnt_rst, frac, sc.denorm_to_tiny_frac, 0x1fea) & ((1ULL << 52) - 1);
    const uint64_t ex4_half_denorm_frac = calc_denorm_frac(ex4_expnt_rst, frac, sc.denorm_to_tiny_frac, 0x1ff7) & ((1ULL << 52) - 1);

    const bool ex4_denorm_potnt_norm = ((sc.potnt_norm & 0x2) && ((frac >> 53) & 0x1)) || ((sc.potnt_norm & 0x1) && ((frac >> 54) & 0x1));
    const bool ex4_rslt_denorm = !sc.result_qnan && !sc.result_zero && (sc.rslt_denorm && !ex4_denorm_potnt_norm);

    uint64_t ex4_denorm_result = 0;
    if (is_double) {
        ex4_denorm_result = (static_cast<uint64_t>(sc.result_sign) << 63) | ex4_denorm_frac;
    } else if (is_single) {
        const uint64_t frac23 = get_bits(ex4_single_denorm_frac, 51, 29);
        ex4_denorm_result = (0xffffffffULL << 32) | (static_cast<uint64_t>(sc.result_sign) << 31) | frac23;
    } else {
        const uint64_t frac10 = get_bits(ex4_half_denorm_frac, 51, 42);
        ex4_denorm_result = (0xffffffffffffULL << 16) | (static_cast<uint64_t>(sc.result_sign) << 15) | frac10;
    }

    const uint64_t ex4_half_lfn = (0xffffffffffffULL << 16) | (static_cast<uint64_t>(sc.result_sign) << 15) | (0x1eULL << 10) | ((1ULL << 10) - 1);
    const uint64_t ex4_half_rst_qnan = (0xffffffffffffULL << 16) | (static_cast<uint64_t>(sc.qnan_sign) << 15) | (0x1fULL << 10) | (1ULL << 9) | get_bits(sc.qnan_f, 8, 0);
    const uint64_t ex4_half_rst_inf = (0xffffffffffffULL << 16) | (static_cast<uint64_t>(sc.result_sign) << 15) | (0x1fULL << 10);
    const uint64_t ex4_half_rst_norm = (0xffffffffffffULL << 16) | (static_cast<uint64_t>(sc.result_sign) << 15) | (get_bits(ex4_expnt_rst, 4, 0) << 10) | get_bits(frac_hi == 0 ? frac : (frac_hi == 1 ? (frac >> 1) : (frac >> 2)), 51, 42);
    const uint64_t ex4_half_rst0 = (0xffffffffffffULL << 16) | (static_cast<uint64_t>(sc.result_sign) << 15);

    uint64_t ex4_frac_52 = 0;
    switch (frac_hi) {
        case 0b00: ex4_frac_52 = get_bits(frac, 51, 0); break;
        case 0b01: ex4_frac_52 = get_bits(frac, 52, 1); break;
        default: ex4_frac_52 = get_bits(frac, 53, 2); break;
    }

    const uint64_t doub_lfn = (static_cast<uint64_t>(sc.result_sign) << 63) | (0x7feULL << 52) | ((1ULL << 52) - 1);
    const uint64_t sing_lfn = (0xffffffffULL << 32) | (static_cast<uint64_t>(sc.result_sign) << 31) | (0xfeULL << 23) | ((1ULL << 23) - 1);
    const uint64_t doub_rst0 = static_cast<uint64_t>(sc.result_sign) << 63;
    const uint64_t sing_rst0 = (0xffffffffULL << 32) | (static_cast<uint64_t>(sc.result_sign) << 31);

    const uint64_t qnan_f = sc.qnan_f & ((1ULL << 52) - 1);
    const uint64_t doub_rst_qnan = (static_cast<uint64_t>(sc.qnan_sign) << 63) | (0x7ffULL << 52) | (1ULL << 51) | get_bits(qnan_f, 50, 0);
    const uint64_t sing_rst_qnan = (0xffffffffULL << 32) | (static_cast<uint64_t>(sc.qnan_sign) << 31) | (0xffULL << 23) | (1ULL << 22) | get_bits(qnan_f, 21, 0);
    const uint64_t doub_rst_inf = (static_cast<uint64_t>(sc.result_sign) << 63) | (0x7ffULL << 52);
    const uint64_t sing_rst_inf = (0xffffffffULL << 32) | (static_cast<uint64_t>(sc.result_sign) << 31) | (0xffULL << 23);

    const uint64_t doub_rst_norm = (static_cast<uint64_t>(sc.result_sign) << 63) | (get_bits(ex4_expnt_rst, 10, 0) << 52) | ex4_frac_52;
    const uint64_t sing_rst_norm = (0xffffffffULL << 32) | (static_cast<uint64_t>(sc.result_sign) << 31) | (get_bits(ex4_expnt_rst, 7, 0) << 23) | get_bits(ex4_frac_52, 51, 29);

    const bool ex4_rst_nor = sc.result_nor;
    const bool ex4_of_plus = sc.potnt_of && ((frac >> 53) & 0x3) && ex4_rst_nor;
    const bool ex4_uf_plus = sc.potnt_uf && (((frac >> 53) & 0x3) == 0) && ex4_rst_nor;

    const bool ex4_result_lfn = (ex4_of_plus && sc.of_rst_lfn) || sc.result_lfn;
    const bool ex4_result_inf = (ex4_of_plus && !sc.of_rst_lfn) || sc.result_inf;

    const uint64_t ex4_rst_lfn = is_double ? doub_lfn : is_single ? sing_lfn : ex4_half_lfn;
    const uint64_t ex4_rst0 = is_double ? doub_rst0 : is_single ? sing_rst0 : ex4_half_rst0;
    const uint64_t ex4_rst_qnan = is_double ? doub_rst_qnan : is_single ? sing_rst_qnan : ex4_half_rst_qnan;
    const uint64_t ex4_rst_norm = is_double ? doub_rst_norm : is_single ? sing_rst_norm : ex4_half_rst_norm;
    const uint64_t ex4_rst_inf = is_double ? doub_rst_inf : is_single ? sing_rst_inf : ex4_half_rst_inf;

    const bool ex4_cor_uf = ((sc.uf && !ex4_denorm_potnt_norm) || ex4_uf_plus) && sc.nx;
    const bool ex4_cor_nx = sc.nx || sc.of || ex4_of_plus;

    const uint8_t ex4_out_expt = (static_cast<uint8_t>(sc.nv) << 4) | (static_cast<uint8_t>(sc.dz) << 3) |
                                 (static_cast<uint8_t>(sc.of || ex4_of_plus) << 2) | (static_cast<uint8_t>(ex4_cor_uf) << 1) |
                                 static_cast<uint8_t>(ex4_cor_nx);

    const bool ex4_final_rst_norm = !sc.result_qnan && !ex4_result_inf && !ex4_result_lfn && !sc.result_zero && !ex4_rslt_denorm;
    uint64_t ex4_out_result = 0;
    const uint8_t sel = (static_cast<uint8_t>(ex4_rslt_denorm) << 5) | (static_cast<uint8_t>(sc.result_qnan) << 4) |
                        (static_cast<uint8_t>(ex4_result_inf) << 3) | (static_cast<uint8_t>(ex4_result_lfn) << 2) |
                        (static_cast<uint8_t>(sc.result_zero) << 1) | static_cast<uint8_t>(ex4_final_rst_norm);
    switch (sel) {
        case 0b100000: ex4_out_result = ex4_denorm_result; break;
        case 0b010000: ex4_out_result = ex4_rst_qnan; break;
        case 0b001000: ex4_out_result = ex4_rst_inf; break;
        case 0b000100: ex4_out_result = ex4_rst_lfn; break;
        case 0b000010: ex4_out_result = ex4_rst0; break;
        case 0b000001: ex4_out_result = ex4_rst_norm; break;
        default: ex4_out_result = 0; break;
    }

    return Expected{ex4_out_result, ex4_out_expt};
}

bool drive_and_check(Vct_vfdsu_pack& dut, const Scenario& sc) {
    const Expected exp = compute_expected(sc);

    dut.vfdsu_ex4_denorm_to_tiny_frac = sc.denorm_to_tiny_frac;
    dut.vfdsu_ex4_double = sc.double_mode;
    dut.vfdsu_ex4_dz = sc.dz;
    dut.vfdsu_ex4_expnt_rst = sc.expnt_rst;
    dut.vfdsu_ex4_frac = sc.frac;
    dut.vfdsu_ex4_nv = sc.nv;
    dut.vfdsu_ex4_nx = sc.nx;
    dut.vfdsu_ex4_of = sc.of;
    dut.vfdsu_ex4_of_rst_lfn = sc.of_rst_lfn;
    dut.vfdsu_ex4_potnt_norm = sc.potnt_norm & 0x3;
    dut.vfdsu_ex4_potnt_of = sc.potnt_of;
    dut.vfdsu_ex4_potnt_uf = sc.potnt_uf;
    dut.vfdsu_ex4_qnan_f = sc.qnan_f;
    dut.vfdsu_ex4_qnan_sign = sc.qnan_sign;
    dut.vfdsu_ex4_result_inf = sc.result_inf;
    dut.vfdsu_ex4_result_lfn = sc.result_lfn;
    dut.vfdsu_ex4_result_nor = sc.result_nor;
    dut.vfdsu_ex4_result_qnan = sc.result_qnan;
    dut.vfdsu_ex4_result_sign = sc.result_sign;
    dut.vfdsu_ex4_result_zero = sc.result_zero;
    dut.vfdsu_ex4_rslt_denorm = sc.rslt_denorm;
    dut.vfdsu_ex4_single = sc.single_mode;
    dut.vfdsu_ex4_uf = sc.uf;

    dut.eval();

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) std::cerr << msg << "\n";
        return cond;
    };

    bool ok = true;
    ok &= expect(dut.ex4_out_result == exp.out_result, sc.name + " out_result mismatch");
    ok &= expect(dut.ex4_out_expt == exp.out_expt, sc.name + " out_expt mismatch");
    return ok;
}

uint64_t make_frac(uint8_t hi_bits, uint64_t payload) {
    const uint64_t hi = static_cast<uint64_t>(hi_bits & 0x3) << 53;
    return (payload & ((1ULL << 53) - 1)) | hi;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_pack dut;

    std::vector<Scenario> scenarios = {
        {"norm_double", false, true, false, false, 0x100, make_frac(0b10, 0x123456789ABFULL), false, true, false, false, 0, false, false, 0x1111111111111ULL, false, false, false, true, false, false, false, false, false},
        {"qnan_double", false, true, false, false, 0x10, make_frac(0b01, 0x2222222222222ULL), false, false, false, false, 0, false, false, 0xABCDE00000000ULL, true, false, false, false, true, false, false, false, false},
        {"inf_flag", false, true, false, false, 0x20, make_frac(0b01, 0x3333333333333ULL), false, false, false, false, 0, false, false, 0x0, false, true, false, false, false, false, false, false, false},
        {"lfn_flag", false, true, false, false, 0x30, make_frac(0b01, 0x4444444444444ULL), false, false, false, false, 0, false, false, 0, false, false, true, false, false, false, false, false, false},
        {"zero_flag", false, true, false, false, 0x40, make_frac(0b01, 0x5555555555555ULL), false, false, false, false, 0, false, false, 0, false, false, false, false, false, true, false, false, false},
        {"denorm_path", false, true, false, false, 0x1fce, make_frac(0b01, 0x1555555555555ULL), false, true, false, false, 0, false, false, 0, false, false, false, false, true, false, true, false, false},
        {"denorm_tiny_default", true, true, false, false, 0x123, make_frac(0b01, 0x1666666666666ULL), false, true, false, false, 0, false, false, 0, false, false, false, false, true, false, true, false, false},
        {"of_plus_inf", false, true, false, false, 0x50, make_frac(0b10, 0x7777777777777ULL), false, true, false, false, 0, true, false, 0, false, false, false, false, false, false, false, true, false},
        {"of_plus_lfn", false, true, false, false, 0x60, make_frac(0b11, 0x1234500000000ULL), false, true, true, true, 0, true, false, 0, false, false, false, false, false, false, false, true, false},
        {"uf_plus", false, true, false, false, 0x70, make_frac(0b00, 0x1), false, true, false, false, 0, false, true, 0, false, false, false, false, false, false, true, false, true},
        {"default_path", false, true, false, false, 0x80, make_frac(0b01, 0x8888888888888ULL), false, false, false, false, 0, false, false, 0, false, false, false, false, false, false, false, false, false},
        {"single_norm", false, false, true, false, 0x12, make_frac(0b01, 0xABCDEFFFFULL), false, true, false, false, 0, false, false, 0x1234, false, false, true, false, false, false, false, false, false},
        {"half_norm", false, false, false, false, 0x5, make_frac(0b01, 0x54321ULL), false, true, false, false, 0, false, false, 0x55, false, false, true, false, false, false, false, false, false},
        {"potnt_norm_block", false, true, false, false, 0x90, make_frac(0b01, 0x1FFFFFFFFFFFFULL), false, true, false, false, 0x2, false, false, 0, false, false, false, false, true, false, true, false, false},
    };

    // Cover all denorm case branches
    for (uint16_t expnt = 0x1fcd; expnt <= 0x1fff; ++expnt) {
        scenarios.push_back({"denorm_case_" + std::to_string(expnt), false, true, false, false, static_cast<uint16_t>(expnt), make_frac(0b01, 0x1555555555555ULL), false, true, false, false, 0, false, false, 0, false, false, false, false, true, false, true, false, false});
    }
    scenarios.push_back({"denorm_case_1", false, true, false, false, 1, make_frac(0b01, 0x1333333333333ULL), false, true, false, false, 0, false, false, 0, false, false, false, false, true, false, true, false, false});
    scenarios.push_back({"denorm_case_0", false, true, false, false, 0, make_frac(0b01, 0x1222222222222ULL), false, true, false, false, 0, false, false, 0, false, false, false, false, true, false, true, false, false});

    // Random sanity
    uint64_t lcg = 12345;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 200; ++i) {
        Scenario sc{
            "rand_" + std::to_string(i),
            (next() & 1) != 0,                      // denorm_to_tiny_frac
            (next() & 1) != 0,                      // double_mode
            (next() & 1) != 0,                      // single_mode
            (next() & 1) != 0,                      // dz
            static_cast<uint16_t>(next() & 0x1fff), // expnt_rst
            next() & ((1ULL << 55) - 1),            // frac
            (next() & 1) != 0,                      // nv
            (next() & 1) != 0,                      // nx
            (next() & 1) != 0,                      // of
            (next() & 1) != 0,                      // of_rst_lfn
            static_cast<uint8_t>(next() & 0x3),     // potnt_norm
            (next() & 1) != 0,                      // potnt_of
            (next() & 1) != 0,                      // potnt_uf
            next() & ((1ULL << 52) - 1),            // qnan_f
            (next() & 1) != 0,                      // qnan_sign
            (next() & 1) != 0,                      // result_inf
            (next() & 1) != 0,                      // result_lfn
            (next() & 1) != 0,                      // result_nor
            (next() & 1) != 0,                      // result_qnan
            (next() & 1) != 0,                      // result_sign
            (next() & 1) != 0,                      // result_zero
            (next() & 1) != 0,                      // rslt_denorm
            (next() & 1) != 0};                     // uf
        if (sc.double_mode && sc.single_mode) sc.single_mode = false;
        if (!sc.double_mode && !sc.single_mode) sc.double_mode = true;
        scenarios.push_back(sc);
    }

    for (const auto& sc : scenarios) {
        if (!drive_and_check(dut, sc)) return 1;
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case035/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
