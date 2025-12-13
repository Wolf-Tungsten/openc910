#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_close_s1_h.h"

#include <array>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_close_s1_h dut;

    constexpr unsigned kWidth = 12;
    constexpr uint16_t kMask = (1u << kWidth) - 1;

    auto sign_extend12 = [=](uint16_t v) -> int16_t {
        v &= kMask;
        return (v & 0x800) ? static_cast<int16_t>(v | 0xF000) : static_cast<int16_t>(v);
    };
    auto encode12 = [=](int32_t v) -> uint16_t {
        return static_cast<uint16_t>(v) & kMask;
    };

    struct Ff1Result {
        uint16_t f_bits;
        uint16_t onehot;
        int pred;
    };

    auto compute_ff1 = [=](uint16_t a, uint16_t b) -> Ff1Result {
        uint16_t a12 = a & kMask;
        uint16_t b12 = b & kMask;
        uint16_t c = (~b12) & kMask;
        uint16_t t = (a12 ^ c) & kMask;
        uint16_t g = (a12 & c) & kMask;
        uint16_t z = (~a12 & ~c) & kMask;
        uint16_t f = 0;

        bool f11 = (((g >> 11) & 1u) && !((z >> 10) & 1u)) ||
                   (((z >> 11) & 1u) && !((g >> 10) & 1u));
        if (f11) {
            f |= 1u << 11;
        }

        bool t1 = (t >> 1) & 1u;
        bool g0 = g & 1u;
        bool z0 = z & 1u;
        bool f0 = (t1 && (g0 || z0)) || (!t1 && (z0 || g0));
        if (f0) {
            f |= 1u;
        }

        for (int i = 10; i >= 1; --i) {
            bool ti1 = (t >> (i + 1)) & 1u;
            bool gi = (g >> i) & 1u;
            bool zi = (z >> i) & 1u;
            bool gim1 = (g >> (i - 1)) & 1u;
            bool zim1 = (z >> (i - 1)) & 1u;
            bool fi = (ti1 && ((gi && !zim1) || (zi && !gim1))) ||
                      (!ti1 && ((gi && !gim1) || (zi && !zim1)));
            if (fi) {
                f |= 1u << i;
            }
        }

        int pred = -1;
        uint16_t onehot = 0;
        for (int i = 11; i >= 0; --i) {
            if (f & (1u << i)) {
                pred = 11 - i;
                onehot = 1u << i;
                break;
            }
        }

        return Ff1Result{f, onehot, pred};
    };

    std::array<bool, 2> op_chg_seen{};
    std::array<bool, kWidth> pred_seen{};
    std::array<bool, kWidth> onehot_seen{};
    bool zero_f_seen = false;

    dut.close_adder0 = 0;
    dut.close_adder1 = 0;
    dut.eval();
    for (int bit = 0; bit < static_cast<int>(kWidth); ++bit) {
        uint16_t val = 1u << bit;
        dut.close_adder0 = val;
        dut.eval();
        dut.close_adder0 = 0;
        dut.eval();

        dut.close_adder1 = val;
        dut.eval();
        dut.close_adder1 = 0;
        dut.eval();
    }

    for (unsigned a = 0; a <= kMask; ++a) {
        for (unsigned b = 0; b <= kMask; ++b) {
            dut.close_adder0 = a;
            dut.close_adder1 = b;
            dut.eval();

            int16_t a_signed = sign_extend12(static_cast<uint16_t>(a));
            int16_t b_signed = sign_extend12(static_cast<uint16_t>(b));
            uint16_t expected_sum = encode12(a_signed - b_signed);
            uint16_t expected_sum_m1 = encode12(a_signed - b_signed + 2);
            bool expected_op_chg = (expected_sum >> 11) & 1u;

            if (dut.close_sum != expected_sum ||
                dut.close_sum_m1 != expected_sum_m1 ||
                dut.close_op_chg != expected_op_chg) {
                std::cerr << "Sum mismatch a=0x" << std::hex << a
                          << " b=0x" << b << std::dec
                          << " dut_sum=0x" << std::hex << dut.close_sum
                          << " exp_sum=0x" << expected_sum
                          << " dut_sum_m1=0x" << dut.close_sum_m1
                          << " exp_sum_m1=0x" << expected_sum_m1 << std::dec
                          << " dut_op_chg=" << int(dut.close_op_chg)
                          << " exp_op_chg=" << expected_op_chg << "\n";
                return 1;
            }

            Ff1Result ff1 = compute_ff1(static_cast<uint16_t>(a), static_cast<uint16_t>(b));
            if (ff1.pred >= 0) {
                if (dut.ff1_pred != static_cast<uint32_t>(ff1.pred) ||
                    (dut.ff1_pred_onehot & kMask) != ff1.onehot) {
                    std::cerr << "FF1 mismatch a=0x" << std::hex << a
                              << " b=0x" << b
                              << " dut_ff1_pred=" << std::dec << dut.ff1_pred
                              << " exp_pred=" << ff1.pred
                              << " dut_onehot=0x" << std::hex << (dut.ff1_pred_onehot & kMask)
                              << " exp_onehot=0x" << ff1.onehot << std::dec
                              << " f_bits=0x" << std::hex << ff1.f_bits << std::dec << "\n";
                    return 1;
                }
            } else {
                zero_f_seen = true;
            }

            op_chg_seen[dut.close_op_chg] = true;
            if (dut.ff1_pred < pred_seen.size()) {
                pred_seen[dut.ff1_pred] = true;
            }
            uint16_t oh = dut.ff1_pred_onehot & kMask;
            for (unsigned i = 0; i < kWidth; ++i) {
                if (oh & (1u << i)) {
                    onehot_seen[i] = true;
                }
            }
        }
    }

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case004/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    auto count_hits = [](const auto& container) {
        return std::count(container.begin(), container.end(), true);
    };

    std::cout << "close_op_chg states hit: " << count_hits(op_chg_seen)
              << "/2, ff1_pred values hit: " << count_hits(pred_seen)
              << "/" << kWidth
              << ", ff1_pred_onehot positions hit: " << count_hits(onehot_seen)
              << "/" << kWidth
              << ", zero f_bits observed: " << (zero_f_seen ? "yes" : "no")
              << "\n";
    std::cout << "Coverage written to " << cov_path << "\n";
    return 0;
}
