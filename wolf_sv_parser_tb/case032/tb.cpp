#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfalu_top_pipe7.h"
#include "Vct_vfalu_top_pipe7___024root.h"
#include "Vct_vfalu_top_pipe7__Syms.h"

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

enum class RoundingMode { RNE, RTZ, RDN, RUP, RMM };

struct MaybeExpected {
    bool has_value{false};
    uint64_t value{0};
};

struct CheckLogger {
    size_t pass{0};
    size_t fail{0};
    struct Info {
        size_t count{0};
        size_t fail{0};
    };
    std::unordered_map<std::string, Info> records;

    void record(const std::string& name, bool ok, const std::string& detail = {}) {
        auto& entry = records[name];
        entry.count++;
        if (!ok) entry.fail++;
        if (ok) {
            ++pass;
        } else {
            ++fail;
        }

        const bool should_log = !ok || entry.count == 1;
        if (should_log) {
            std::cout << (ok ? "[PASS] " : "[FAIL] ") << name << " (#" << entry.count << ")";
            if (!detail.empty()) {
                std::cout << " " << detail;
            }
            std::cout << "\n";
        }
    }

    void summary() const {
        const size_t total = pass + fail;
        std::cout << "[SUMMARY] checks=" << total << " pass=" << pass << " fail=" << fail << "\n";
        if (fail > 0) {
            std::cout << "[SUMMARY] failing scenarios:\n";
            for (const auto& kv : records) {
                if (kv.second.fail > 0) {
                    std::cout << "  " << kv.first << " fail=" << kv.second.fail << "/" << kv.second.count << "\n";
                }
            }
        }
    }
};

int to_fe_round(RoundingMode mode) {
    switch (mode) {
        case RoundingMode::RNE:
            return FE_TONEAREST;
        case RoundingMode::RTZ:
            return FE_TOWARDZERO;
        case RoundingMode::RDN:
            return FE_DOWNWARD;
        case RoundingMode::RUP:
            return FE_UPWARD;
        case RoundingMode::RMM:
        default:
            return FE_TONEAREST;
    }
}

struct RoundingGuard {
    explicit RoundingGuard(RoundingMode mode) : original(fegetround()) { fesetround(to_fe_round(mode)); }
    ~RoundingGuard() { fesetround(original); }
    int original;
};

uint8_t effective_rm(uint8_t imm, uint8_t rm) { return (imm & 0x7u) == 0x7u ? (rm & 0x7u) : (imm & 0x7u); }

double bits_to_double(uint64_t bits) {
    double v;
    std::memcpy(&v, &bits, sizeof(v));
    return v;
}

uint64_t double_to_bits(double v) {
    uint64_t bits;
    std::memcpy(&bits, &v, sizeof(bits));
    return bits;
}

float bits_to_float(uint32_t bits) {
    float v;
    std::memcpy(&v, &bits, sizeof(v));
    return v;
}

uint32_t float_to_bits(float v) {
    uint32_t bits;
    std::memcpy(&bits, &v, sizeof(bits));
    return bits;
}

float half_to_float(uint16_t h) {
    const uint16_t sign = h >> 15;
    const uint16_t exp = (h >> 10) & 0x1F;
    const uint16_t frac = h & 0x3FF;
    uint32_t result = static_cast<uint32_t>(sign) << 31;
    if (exp == 0x1F) {
        uint32_t payload = frac ? static_cast<uint32_t>(frac) << 13 : 0;
        if (payload == 0) payload = 0x400000;  // quiet NaN
        result |= 0x7F800000u | payload;
    } else if (exp == 0) {
        if (frac != 0) {
            const float mant = static_cast<float>(frac) * std::ldexp(1.0f, -10);
            float val = std::ldexp(mant, -14);
            val = sign ? -val : val;
            return val;
        }
    } else {
        const float mant = 1.0f + static_cast<float>(frac) * std::ldexp(1.0f, -10);
        float val = std::ldexp(mant, static_cast<int>(exp) - 15);
        val = sign ? -val : val;
        return val;
    }
    return bits_to_float(result);
}

uint16_t float_to_half_rne(float f) {
    const uint32_t bits = float_to_bits(f);
    const uint32_t sign = bits >> 31;
    const uint32_t exp = (bits >> 23) & 0xFF;
    const uint32_t frac = bits & 0x7FFFFF;

    uint16_t hsign = static_cast<uint16_t>(sign) << 15;
    if (exp == 0xFF) {
        uint16_t payload = frac ? static_cast<uint16_t>((frac >> 13) | 0x200u) : 0;
        return static_cast<uint16_t>(hsign | 0x7C00u | payload);
    }

    int32_t half_exp = static_cast<int32_t>(exp) - 127 + 15;
    if (half_exp >= 0x1F) {
        return static_cast<uint16_t>(hsign | 0x7C00u);
    }
    if (half_exp <= 0) {
        if (half_exp < -10) {
            return hsign;
        }
        uint32_t mant = (frac | 0x800000u) >> (1 - half_exp);
        uint32_t round_bit = mant & 0x00001000u;
        uint16_t half = static_cast<uint16_t>(hsign | (mant >> 13));
        if (round_bit && ((mant & 0x00001FFFu) != 0 || (half & 1))) {
            ++half;
        }
        return half;
    }

    uint32_t mant = frac + 0x00001000u;  // rounding bias
    if (mant & 0x00800000u) {
        mant = 0;
        ++half_exp;
    }
    if (half_exp >= 0x1F) {
        return static_cast<uint16_t>(hsign | 0x7C00u);
    }
    return static_cast<uint16_t>(hsign | (static_cast<uint16_t>(half_exp) << 10) | static_cast<uint16_t>(mant >> 13));
}

uint64_t pack_single_bits(uint32_t bits) { return 0xFFFFFFFF00000000ULL | static_cast<uint64_t>(bits); }
uint64_t pack_half_bits(uint16_t bits) { return 0xFFFFFFFFFFFF0000ULL | static_cast<uint64_t>(bits); }

uint64_t pack_single_from_double(double value, RoundingMode mode) {
    RoundingGuard guard(mode);
    const float narrowed = static_cast<float>(value);
    return pack_single_bits(float_to_bits(narrowed));
}

uint64_t pack_single_from_half(uint16_t h) {
    const float widened = half_to_float(h);
    return pack_single_bits(float_to_bits(widened));
}

uint64_t pack_half_from_double(double value, RoundingMode /*mode*/) {
    const float narrowed = static_cast<float>(value);
    const uint16_t h = float_to_half_rne(narrowed);
    return pack_half_bits(h);
}

uint64_t pack_half_from_single(uint32_t bits, RoundingMode /*mode*/) {
    const float narrowed = bits_to_float(bits);
    const uint16_t h = float_to_half_rne(narrowed);
    return pack_half_bits(h);
}

int64_t rounded_from_double(double value, RoundingMode mode, bool is_signed) {
    RoundingGuard guard(mode);
    if (std::isnan(value)) return 0;
    if (is_signed) {
        return static_cast<int64_t>(std::llrint(value));
    }
    long double r = std::llrint(value);
    if (r < 0) return 0;
    return static_cast<int64_t>(static_cast<uint64_t>(r));
}

struct FcnvtConfig {
    bool src_l64;
    bool src_l32;
    bool src_l16;
    bool src_float;
    bool src_si;
    bool dest_float;
    bool dest_si;
    bool dest_ui;
    bool dest_l64;
    bool dest_l32;
    bool dest_l16;
    bool dest_double;
    bool dest_single;
    bool dest_half;
};

FcnvtConfig decode_fcnvt(uint32_t func) {
    const bool widden = (func & (1u << 14)) && !(func & (1u << 13));
    const bool narrow = !(func & (1u << 14)) && (func & (1u << 13));
    const bool equal = !(func & (1u << 13)) && !(func & (1u << 14));
    const bool sover = (func & (1u << 14)) && (func & (1u << 13));
    const bool src_l64 = (func & (1u << 16)) || ((func & (1u << 15)) && narrow);
    const bool src_l32 = ((func & (1u << 15)) && !narrow) || (!(func & (1u << 16)) && !(func & (1u << 15)) && narrow);
    const bool src_l16 = !(func & (1u << 16)) && !(func & (1u << 15)) && !narrow;
    const bool dest_l64 = (src_l64 && equal) || (src_l32 && widden) || (src_l16 && sover);
    const bool dest_l32 = (src_l32 && equal) || (src_l16 && widden) || (src_l64 && narrow);
    const bool dest_l16 = (src_l32 && narrow) || (src_l16 && equal) || (src_l64 && sover);
    const bool dest_float = func & (1u << 2);
    const bool dest_si = func & (1u << 3);
    const bool dest_ui = func & (1u << 4);
    const bool src_float = func & (1u << 1);
    const bool src_si = func & 1u;

    const bool dest_double = dest_float && dest_l64;
    const bool dest_single = dest_float && dest_l32;
    const bool dest_half = dest_float && dest_l16;

    return {src_l64,
            src_l32,
            src_l16,
            src_float,
            src_si,
            dest_float,
            dest_si,
            dest_ui,
            dest_l64,
            dest_l32,
            dest_l16,
            dest_double,
            dest_single,
            dest_half};
}

MaybeExpected compute_fcnvt_expected(const std::string& /*name*/, uint32_t func, uint64_t src, uint8_t imm, uint8_t rm, bool /*dqnan*/) {
    const auto cfg = decode_fcnvt(func);
    const bool raw_src_l64 = func & (1u << 16);
    const bool raw_src_l32 = func & (1u << 15);
    const RoundingMode mode = static_cast<RoundingMode>(effective_rm(imm, rm));
    const int dest_float_count = (cfg.dest_double ? 1 : 0) + (cfg.dest_single ? 1 : 0) + (cfg.dest_half ? 1 : 0);
    if (cfg.dest_float && dest_float_count != 1) {
        return {};
    }
    if (cfg.src_float && raw_src_l32 && cfg.src_l64) {
        return {};
    }
    if (!cfg.src_float && !raw_src_l64 && !raw_src_l32) {
        return {};
    }
    double source_value = 0.0;
    if (cfg.src_float) {
        if (raw_src_l64) {
            source_value = bits_to_double(src);
        } else if (raw_src_l32) {
            source_value = static_cast<double>(bits_to_float(static_cast<uint32_t>(src)));
        } else {
            source_value = static_cast<double>(half_to_float(static_cast<uint16_t>(src)));
        }
    } else {
        if (raw_src_l64) {
            source_value = cfg.src_si ? static_cast<double>(static_cast<int64_t>(src)) : static_cast<double>(src);
        } else if (raw_src_l32) {
            const uint32_t raw = static_cast<uint32_t>(src);
            source_value = cfg.src_si ? static_cast<double>(static_cast<int32_t>(raw)) : static_cast<double>(raw);
        } else {
            const uint16_t raw = static_cast<uint16_t>(src);
            source_value = cfg.src_si ? static_cast<double>(static_cast<int16_t>(raw)) : static_cast<double>(raw);
        }
    }

    if (cfg.dest_float) {
        if (cfg.dest_double) {
            return {true, double_to_bits(source_value)};
        }
        if (cfg.dest_single) {
            return {true, pack_single_from_double(source_value, mode)};
        }
        if (cfg.dest_half) {
            return {true, pack_half_from_double(source_value, mode)};
        }
    } else if (cfg.dest_si || cfg.dest_ui) {
        if (std::isnan(source_value) || std::isinf(source_value)) {
            return {};
        }
        const int64_t rounded = rounded_from_double(source_value, mode, cfg.dest_si);
        uint64_t widened = static_cast<uint64_t>(rounded);
        if (cfg.dest_l16) {
            widened = cfg.dest_si ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(rounded)))
                                  : static_cast<uint64_t>(static_cast<uint16_t>(rounded));
        } else if (cfg.dest_l32) {
            widened = cfg.dest_si ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(rounded)))
                                  : static_cast<uint64_t>(static_cast<uint32_t>(rounded));
        }
        return {true, widened};
    }

    return {};
}

void tick(Vct_vfalu_top_pipe7& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

namespace fadd {

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
    if (op_add) f |= 1u << 12;
    if (op_sub) f |= 1u << 11;
    if (op_cmp) f |= 1u << 10;
    if (op_maxnm) f |= 1u << 9;
    if (op_minnm) f |= 1u << 8;
    f |= (cmp_flags & 0x1Fu);
    return f;
}

uint64_t pack_single(uint32_t low) { return 0xFFFFFFFF00000000ULL | low; }
uint64_t pack_half(uint16_t low) { return 0xFFFFFFFFFFFF0000ULL | low; }
uint64_t pack_single_clean(uint32_t low) { return static_cast<uint64_t>(low); }
uint64_t pack_half_clean(uint16_t low) { return static_cast<uint64_t>(low); }
uint64_t make_double_bits(int exp, uint64_t frac, bool sign = false) {
    return (sign ? (1ULL << 63) : 0) | ((static_cast<uint64_t>(exp) & 0x7FFULL) << 52) | (frac & ((1ULL << 52) - 1));
}
uint64_t make_single_bits(int exp, uint32_t frac, bool sign = false) {
    const uint32_t bits = (sign ? 0x80000000u : 0) | ((static_cast<uint32_t>(exp) & 0xFFu) << 23) | (frac & 0x7FFFFFu);
    return pack_single(bits);
}
uint64_t make_half_bits(int exp, uint16_t frac, bool sign = false) {
    const uint16_t bits = static_cast<uint16_t>((sign ? 0x8000u : 0) | ((exp & 0x1Fu) << 10) | (frac & 0x3FFu));
    return pack_half(bits);
}

struct Scenario {
    const char* name;
    uint32_t func;
    uint64_t src0;
    uint64_t src1;
    uint64_t expected;
    uint8_t imm0;
};

}  // namespace fadd

namespace fspu {

uint64_t make_fclass_double(uint64_t v) {
    const bool sign = (v >> 63) & 1;
    const uint16_t exp = static_cast<uint16_t>((v >> 52) & 0x7FF);
    const uint64_t frac = v & ((1ULL << 52) - 1);
    const bool exp_max = exp == 0x7FF;
    const bool exp_zero = exp == 0;
    const bool frac_zero = frac == 0;
    const bool frac_msb = (frac >> 51) & 1;

    const bool neg_inf = sign && exp_max && frac_zero;
    const bool neg_nm = sign && !exp_max && !exp_zero;
    const bool neg_dn = sign && exp_zero && !frac_zero;
    const bool neg_zero = sign && exp_zero && frac_zero;
    const bool pos_zero = !sign && exp_zero && frac_zero;
    const bool pos_dn = !sign && exp_zero && !frac_zero;
    const bool pos_nm = !sign && !exp_max && !exp_zero;
    const bool pos_inf = !sign && exp_max && frac_zero;
    const bool snan = exp_max && !frac_zero && !frac_msb;
    const bool qnan = exp_max && frac_msb;

    uint64_t bits = 0;
    bits |= static_cast<uint64_t>(neg_inf) << 0;
    bits |= static_cast<uint64_t>(neg_nm) << 1;
    bits |= static_cast<uint64_t>(neg_dn) << 2;
    bits |= static_cast<uint64_t>(neg_zero) << 3;
    bits |= static_cast<uint64_t>(pos_zero) << 4;
    bits |= static_cast<uint64_t>(pos_dn) << 5;
    bits |= static_cast<uint64_t>(pos_nm) << 6;
    bits |= static_cast<uint64_t>(pos_inf) << 7;
    bits |= static_cast<uint64_t>(snan) << 8;
    bits |= static_cast<uint64_t>(qnan) << 9;
    return bits;
}

uint32_t fclass_single_bits(uint32_t v) {
    const bool sign = (v >> 31) & 1u;
    const uint32_t exp = (v >> 23) & 0xFFu;
    const uint32_t frac = v & 0x7FFFFFu;
    const bool exp_max = exp == 0xFFu;
    const bool exp_zero = exp == 0;
    const bool frac_zero = frac == 0;
    const bool frac_msb = (frac >> 22) & 1u;

    const bool neg_inf = sign && exp_max && frac_zero;
    const bool neg_nm = sign && !exp_max && !exp_zero;
    const bool neg_dn = sign && exp_zero && !frac_zero;
    const bool neg_zero = sign && exp_zero && frac_zero;
    const bool pos_zero = !sign && exp_zero && frac_zero;
    const bool pos_dn = !sign && exp_zero && !frac_zero;
    const bool pos_nm = !sign && !exp_max && !exp_zero;
    const bool pos_inf = !sign && exp_max && frac_zero;
    const bool snan = exp_max && !frac_zero && !frac_msb;
    const bool qnan = exp_max && frac_msb;

    uint32_t res = 0;
    res |= static_cast<uint32_t>(neg_inf) << 0;
    res |= static_cast<uint32_t>(neg_nm) << 1;
    res |= static_cast<uint32_t>(neg_dn) << 2;
    res |= static_cast<uint32_t>(neg_zero) << 3;
    res |= static_cast<uint32_t>(pos_zero) << 4;
    res |= static_cast<uint32_t>(pos_dn) << 5;
    res |= static_cast<uint32_t>(pos_nm) << 6;
    res |= static_cast<uint32_t>(pos_inf) << 7;
    res |= static_cast<uint32_t>(snan) << 8;
    res |= static_cast<uint32_t>(qnan) << 9;
    return res;
}

uint16_t fclass_half_bits(uint16_t v) {
    const bool sign = (v >> 15) & 1;
    const uint16_t exp = (v >> 10) & 0x1F;
    const uint16_t frac = v & 0x3FF;
    const bool exp_max = exp == 0x1F;
    const bool exp_zero = exp == 0;
    const bool frac_zero = frac == 0;
    const bool frac_msb = (frac >> 9) & 1;

    const bool neg_inf = sign && exp_max && frac_zero;
    const bool neg_nm = sign && !exp_max && !exp_zero;
    const bool neg_dn = sign && exp_zero && !frac_zero;
    const bool neg_zero = sign && exp_zero && frac_zero;
    const bool pos_zero = !sign && exp_zero && frac_zero;
    const bool pos_dn = !sign && exp_zero && !frac_zero;
    const bool pos_nm = !sign && !exp_max && !exp_zero;
    const bool pos_inf = !sign && exp_max && frac_zero;
    const bool snan = exp_max && !frac_zero && !frac_msb;
    const bool qnan = exp_max && frac_msb;

    uint16_t res = 0;
    res |= static_cast<uint16_t>(neg_inf) << 0;
    res |= static_cast<uint16_t>(neg_nm) << 1;
    res |= static_cast<uint16_t>(neg_dn) << 2;
    res |= static_cast<uint16_t>(neg_zero) << 3;
    res |= static_cast<uint16_t>(pos_zero) << 4;
    res |= static_cast<uint16_t>(pos_dn) << 5;
    res |= static_cast<uint16_t>(pos_nm) << 6;
    res |= static_cast<uint16_t>(pos_inf) << 7;
    res |= static_cast<uint16_t>(snan) << 8;
    res |= static_cast<uint16_t>(qnan) << 9;
    return res;
}

uint64_t apply_fsgnj(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign1 = (op1 >> sign_lsb) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    return (sign1 << sign_lsb) | payload;
}

uint64_t apply_fsgnjn(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign1 = (~op1 >> sign_lsb) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    return (sign1 << sign_lsb) | payload;
}

uint64_t apply_fsgnjx(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign = ((op0 >> sign_lsb) ^ (op1 >> sign_lsb)) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    return (sign << sign_lsb) | payload;
}

uint64_t pack_single_val(uint32_t v) { return (0xFFFFFFFFULL << 32) | v; }
uint64_t pack_half_val(uint16_t v) { return (0xFFFFFFFFFFFFULL << 16) | v; }

uint32_t build_func(bool is_double, bool is_single, bool op_class, bool fmvfx, bool fmvxf, bool fsgnj, bool fsgnjn, bool fsgnjx) {
    uint32_t f = 0;
    if (is_double) f |= 1u << 16;
    if (is_single) f |= 1u << 15;
    if (op_class) f |= 1u << 18;
    if (fmvfx) {
        f |= 1u << 5;
        f |= 1u << 0;
    }
    if (fmvxf) {
        f |= 1u << 5;
        f |= 1u << 2;
    }
    if (fsgnj) {
        f |= 1u << 6;
        f |= 1u << 0;
    }
    if (fsgnjn) {
        f |= 1u << 6;
        f |= 1u << 1;
    }
    if (fsgnjx) {
        f |= 1u << 6;
        f |= 1u << 2;
    }
    return f;
}

struct Scenario {
    const char* name;
    uint32_t func;
    uint64_t src0;
    uint64_t src1;
    uint64_t mtvr;
};

struct Expected {
    uint64_t result;
    uint64_t mfvr;
};

Expected compute_expected(const Scenario& sc) {
    const bool is_double = sc.func & (1u << 16);
    const bool is_single = sc.func & (1u << 15);
    const bool op_class = sc.func & (1u << 18);
    const bool op_fmvfx = (sc.func & (1u << 5)) && (sc.func & 1u);
    const bool op_fmvxf = (sc.func & (1u << 5)) && (sc.func & (1u << 2));
    const bool op_fsgnj = (sc.func & (1u << 6)) && (sc.func & 1u);
    const bool op_fsgnjn = (sc.func & (1u << 6)) && (sc.func & (1u << 1));
    const bool op_fsgnjx = (sc.func & (1u << 6)) && (sc.func & (1u << 2));
    const uint64_t chosen_mtvr = op_fmvfx ? sc.mtvr : sc.src1;

    Expected e{0, 0};
    if (is_double) {
        if (op_fmvfx) {
            e.result = chosen_mtvr;
        } else if (op_fsgnj) {
            e.result = apply_fsgnj(sc.src0, sc.src1, 64, 63);
        } else if (op_fsgnjn) {
            e.result = apply_fsgnjn(sc.src0, sc.src1, 64, 63);
        } else if (op_fsgnjx) {
            e.result = apply_fsgnjx(sc.src0, sc.src1, 64, 63);
        }
        e.mfvr = op_class ? make_fclass_double(sc.src0) : (op_fmvxf ? sc.src0 : 0);
    } else if (is_single) {
        const bool cnan0 = (sc.src0 >> 32) != 0xFFFFFFFFULL;
        const bool cnan1 = (sc.src1 >> 32) != 0xFFFFFFFFULL;
        const uint32_t op0 = cnan0 ? 0x7fc00000u : static_cast<uint32_t>(sc.src0);
        const uint32_t op1 = cnan1 ? 0x7fc00000u : static_cast<uint32_t>(sc.src1);
        if (op_fmvfx) {
            e.result = (0xFFFFFFFFULL << 32) | static_cast<uint32_t>(chosen_mtvr);
        } else if (op_fsgnj) {
            e.result = (0xFFFFFFFFULL << 32) | apply_fsgnj(op0, op1, 32, 31);
        } else if (op_fsgnjn) {
            e.result = (0xFFFFFFFFULL << 32) | apply_fsgnjn(op0, op1, 32, 31);
        } else if (op_fsgnjx) {
            e.result = (0xFFFFFFFFULL << 32) | apply_fsgnjx(op0, op1, 32, 31);
        }
        e.mfvr = op_class ? static_cast<uint64_t>(fclass_single_bits(op0))
                          : (op_fmvxf ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(sc.src0 & 0xFFFFFFFFu))) : 0);
    } else {
        const bool cnan0 = (sc.src0 >> 16) != 0xFFFFFFFFFFFFULL;
        const bool cnan1 = (sc.src1 >> 16) != 0xFFFFFFFFFFFFULL;
        const uint16_t op0 = cnan0 ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(sc.src0 & 0xFFFF);
        const uint16_t op1 = cnan1 ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(sc.src1 & 0xFFFF);
        if (op_fmvfx) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(chosen_mtvr & 0xFFFF);
        } else if (op_fsgnj) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(apply_fsgnj(op0, op1, 16, 15));
        } else if (op_fsgnjn) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(apply_fsgnjn(op0, op1, 16, 15));
        } else if (op_fsgnjx) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(apply_fsgnjx(op0, op1, 16, 15));
        }
        e.mfvr = op_class ? static_cast<uint64_t>(fclass_half_bits(op0))
                          : (op_fmvxf ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(sc.src0 & 0xFFFF))) : 0);
    }
    return e;
}

}  // namespace fspu

namespace fcnvt {

uint32_t func_bits(bool src_l64,
                   bool src_l32,
                   bool widden,
                   bool narrow,
                   bool src_si,
                   bool src_float,
                   bool dest_float,
                   bool dest_si,
                   bool dest_ui,
                   bool dest_h,
                   bool dest_s) {
    uint32_t f = 0;
    if (src_l64) f |= 1u << 16;
    if (src_l32) f |= 1u << 15;
    if (widden) f |= 1u << 14;
    if (narrow) f |= 1u << 13;
    if (src_si) f |= 1u << 0;
    if (src_float) f |= 1u << 1;
    if (dest_float) f |= 1u << 2;
    if (dest_si) f |= 1u << 3;
    if (dest_ui) f |= 1u << 4;
    if (dest_h) f |= 1u << 10;
    if (dest_s) f |= 1u << 11;
    return f;
}

uint64_t pack_single(uint32_t low) { return 0xFFFFFFFF00000000ULL | low; }
uint64_t pack_half(uint16_t low) { return 0xFFFFFFFFFFFF0000ULL | low; }
uint64_t make_double(int sign, uint16_t exp, uint64_t frac) { return (static_cast<uint64_t>(sign & 1) << 63) | (static_cast<uint64_t>(exp & 0x7FF) << 52) | (frac & ((1ULL << 52) - 1)); }

}  // namespace fcnvt

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_vfalu_top_pipe7 dut;
    CheckLogger logger;

    auto expect = [&](bool cond, const std::string& msg) {
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
    dut.dp_vfalu_ex1_pipex_sel = 0;
    dut.dp_vfalu_ex1_pipex_func = 0;
    dut.dp_vfalu_ex1_pipex_imm0 = 0;
    dut.dp_vfalu_ex1_pipex_srcf0 = 0;
    dut.dp_vfalu_ex1_pipex_srcf1 = 0;
    dut.dp_vfalu_ex1_pipex_mtvr_src0 = 0;
    dut.vfpu_yy_xx_dqnan = 0;
    dut.vfpu_yy_xx_rm = 0;
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    auto run_fadd = [&](const fadd::Scenario& sc, bool check_expected = true) -> bool {
        bool ok = true;
        std::string detail;
        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_imm0 = sc.imm0;
        dut.dp_vfalu_ex1_pipex_sel = 0b010;
        tick(dut);  // EX1 -> EX2
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);  // EX2 -> EX3

        if (check_expected &&
            !expect(dut.pipex_dp_ex3_vfalu_freg_data == sc.expected,
                    std::string(sc.name) + " result mismatch got 0x" + [&]() {
                        std::ostringstream oss;
                        oss << std::hex << dut.pipex_dp_ex3_vfalu_freg_data << " expected 0x" << sc.expected;
                        return oss.str();
                    }())) {
            ok = false;
            std::ostringstream oss;
            oss << "got 0x" << std::hex << dut.pipex_dp_ex3_vfalu_freg_data << " expected 0x" << sc.expected;
            detail = oss.str();
        }
        tick(dut);
        if (check_expected) {
            logger.record(sc.name, ok, detail);
        }
        return ok;
    };

    auto run_cmp = [&](const char* name, uint32_t func, uint64_t s0, uint64_t s1, uint64_t expected_cmp) -> bool {
        bool ok = true;
        std::string detail;
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = s0;
        dut.dp_vfalu_ex1_pipex_srcf1 = s1;
        dut.dp_vfalu_ex1_pipex_imm0 = 0;
        dut.dp_vfalu_ex1_pipex_sel = 0b010;
        tick(dut);
        const uint64_t mfvr = dut.pipex_dp_ex1_vfalu_mfvr_data;
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);

        if (!expect(mfvr == expected_cmp,
                    std::string(name) + " cmp mfvr mismatch got 0x" + [&]() {
                        std::ostringstream oss;
                        oss << std::hex << mfvr << " expected 0x" << expected_cmp;
                        return oss.str();
                    }())) {
            ok = false;
            std::ostringstream oss;
            oss << "got 0x" << std::hex << mfvr << " expected 0x" << expected_cmp;
            detail = oss.str();
        }
        tick(dut);
        logger.record(name, ok, detail);
        return ok;
    };

    auto drive_fadd_no_check = [&](uint32_t func, uint64_t s0, uint64_t s1, uint8_t imm = 0, uint8_t rm = 0) {
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = s0;
        dut.dp_vfalu_ex1_pipex_srcf1 = s1;
        dut.dp_vfalu_ex1_pipex_imm0 = imm;
        dut.vfpu_yy_xx_rm = rm;
        dut.dp_vfalu_ex1_pipex_sel = 0b010;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        tick(dut);
    };

    auto run_fspu = [&](const fspu::Scenario& sc) -> bool {
        bool ok = true;
        std::string detail;
        const fspu::Expected exp = fspu::compute_expected(sc);
        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = sc.mtvr;
        dut.dp_vfalu_ex1_pipex_sel = 0b001;
        tick(dut);
        const uint64_t mfvr = dut.pipex_dp_ex1_vfalu_mfvr_data;
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        if (!expect(dut.pipex_dp_ex3_vfalu_freg_data == exp.result,
                    std::string(sc.name) + " result mismatch got 0x" + [&]() {
                        std::ostringstream oss;
                        oss << std::hex << dut.pipex_dp_ex3_vfalu_freg_data << " expected 0x" << exp.result;
                        return oss.str();
                    }())) {
            ok = false;
            std::ostringstream oss;
            oss << "result got 0x" << std::hex << dut.pipex_dp_ex3_vfalu_freg_data << " expected 0x" << exp.result;
            detail = oss.str();
        }
        if (!expect(mfvr == exp.mfvr,
                    std::string(sc.name) + " mfvr mismatch got 0x" + [&]() {
                        std::ostringstream oss;
                        oss << std::hex << mfvr << " expected 0x" << exp.mfvr;
                        return oss.str();
                    }())) {
            ok = false;
            if (detail.empty()) {
                std::ostringstream oss;
                oss << "mfvr got 0x" << std::hex << mfvr << " expected 0x" << exp.mfvr;
                detail = oss.str();
            }
        }
        tick(dut);
        logger.record(sc.name, ok, detail);
        return ok;
    };

    auto drive_fspu_no_check = [&](uint32_t func, uint64_t s0, uint64_t s1, uint64_t mtvr) {
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = s0;
        dut.dp_vfalu_ex1_pipex_srcf1 = s1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = mtvr;
        dut.dp_vfalu_ex1_pipex_sel = 0b001;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        tick(dut);
    };

    auto run_fcnvt = [&](const char* name, uint32_t func, uint64_t src, uint8_t imm, uint8_t rm, bool dqnan, bool expect_nonzero) -> bool {
        bool ok = true;
        std::string detail;
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = src;
        dut.dp_vfalu_ex1_pipex_imm0 = imm & 0x7;
        dut.vfpu_yy_xx_rm = rm & 0x7;
        dut.vfpu_yy_xx_dqnan = dqnan ? 1 : 0;
        dut.dp_vfalu_ex1_pipex_sel = 0b100;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        const MaybeExpected expected = compute_fcnvt_expected(name, func, src, imm, rm, dqnan);
        const uint64_t got = dut.pipex_dp_ex3_vfalu_freg_data;
        if (expected.has_value) {
            if (!expect(got == expected.value,
                        std::string(name) + " result mismatch got 0x" + [&]() {
                            std::ostringstream oss;
                            oss << std::hex << got << " expected 0x" << expected.value;
                            return oss.str();
                        }())) {
                ok = false;
                std::ostringstream oss;
                oss << "got 0x" << std::hex << got << " expected 0x" << expected.value;
                detail = oss.str();
            }
        } else if (!expect(got != 0 || !expect_nonzero, std::string(name) + " result stuck zero")) {
            ok = false;
            detail = "result stuck zero";
        }
        tick(dut);
        logger.record(name, ok, detail);
        return ok;
    };

    std::vector<fadd::Scenario> fadd_basic = {
        {"double_add", fadd::make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x4000000000000000ULL, 0x4008000000000000ULL, 0},
        {"single_add", fadd::make_func(false, true, true, false, false, false, false, 0), fadd::pack_single(0x3FC00000), fadd::pack_single(0xBF000000), fadd::pack_single(0x3F800000), 0},
        {"half_add", fadd::make_func(false, false, true, false, false, false, false, 0), fadd::pack_half(0x3C00), fadd::pack_half(0x3800), fadd::pack_half(0x3E00), 0},
        {"double_sub", fadd::make_func(true, false, false, true, false, false, false, 0), 0x4008000000000000ULL, 0x3FF0000000000000ULL, 0x4000000000000000ULL, 0},
        {"single_sub", fadd::make_func(false, true, false, true, false, false, false, 0), fadd::pack_single(0x40200000), fadd::pack_single(0x3F800000), fadd::pack_single(0x3FC00000), 0},
        {"half_sub", fadd::make_func(false, false, false, true, false, false, false, 0), fadd::pack_half(0x3C00), fadd::pack_half(0x3800), fadd::pack_half(0x3800), 0},
    };
    for (const auto& sc : fadd_basic) {
        if (!run_fadd(sc)) return 1;
    }

    if (!run_fadd({"double_maxnm", fadd::make_func(true, false, false, false, false, true, false, 0), 0xBFF0000000000000ULL, 0x4008000000000000ULL, 0x4008000000000000ULL, 0})) return 1;
    if (!run_fadd({"single_minnm", fadd::make_func(false, true, false, false, false, false, true, 0), fadd::pack_single(0x7F800000), fadd::pack_single(0x3F000000), fadd::pack_single(0x3F000000), 0})) return 1;
    if (!run_fadd({"half_maxnm", fadd::make_func(false, false, false, false, false, true, false, 0), fadd::pack_half(0xFC00), fadd::pack_half(0x3C00), fadd::pack_half(0x3C00), 0})) return 1;

    if (!run_cmp("cmp_feq_single", fadd::make_func(false, true, false, false, true, false, false, 0b00001), fadd::pack_single(0x3F800000), fadd::pack_single(0x3F800000), 1)) return 1;
    if (!run_cmp("cmp_flt_double", fadd::make_func(true, false, false, false, true, false, false, 0b00010), 0x3FF0000000000000ULL, 0x4008000000000000ULL, 1)) return 1;
    if (!run_cmp("cmp_fle_half", fadd::make_func(false, false, false, false, true, false, false, 0b00100), fadd::pack_half(0x3800), fadd::pack_half(0x3C00), 1)) return 1;
    if (!run_cmp("cmp_unord_double", fadd::make_func(true, false, false, false, true, false, false, 0b00001), 0x7FF8000000000001ULL, 0x3FF0000000000000ULL, 0)) return 1;
    if (!run_cmp("cmp_unord_single", fadd::make_func(false, true, false, false, true, false, false, 0b00010), fadd::pack_single(0x7FC00001), fadd::pack_single(0x3F800000), 0)) return 1;
    if (!run_cmp("cmp_unord_half", fadd::make_func(false, false, false, false, true, false, false, 0b00100), fadd::pack_half(0x7E00), fadd::pack_half(0x3C00), 0)) return 1;

    std::vector<fadd::Scenario> rounding = {
        {"double_rtz_add", fadd::make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x3FF0000000000000ULL, 0x4000000000000000ULL, 0b001},
        {"single_rdn_add", fadd::make_func(false, true, true, false, false, false, false, 0), fadd::pack_single(0x3E99999A), fadd::pack_single(0x3E4CCCCD), fadd::pack_single(0x3F000000), 0b010},
        {"single_rup_add", fadd::make_func(false, true, true, false, false, false, false, 0), fadd::pack_single(0x3E99999A), fadd::pack_single(0x3E99999A), fadd::pack_single(0x3F19999A), 0b111},
        {"double_rmm_add", fadd::make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x3FF0000000000000ULL, 0x4000000000000000ULL, 0},
    };
    dut.vfpu_yy_xx_rm = 0;
    if (!run_fadd(rounding[0])) return 1;
    if (!run_fadd(rounding[1])) return 1;
    dut.vfpu_yy_xx_rm = 0b011;
    if (!run_fadd(rounding[2])) return 1;
    dut.vfpu_yy_xx_rm = 0b100;
    if (!run_fadd(rounding[3])) return 1;
    dut.vfpu_yy_xx_rm = 0;

    dut.vfpu_yy_xx_dqnan = 1;
    if (!run_fadd({"single_nan_add", fadd::make_func(false, true, true, false, false, false, false, 0), fadd::pack_single(0x7FC00001), fadd::pack_single(0x3F800000), fadd::pack_single(0x7FC00001), 0})) return 1;
    dut.vfpu_yy_xx_dqnan = 0;

    auto close_sweep = [&](bool is_single, bool is_half) {
        for (uint16_t frac = 0x0001; frac < 0x0008; ++frac) {
            uint64_t a = is_single ? fadd::pack_single(0x3F800000 + (static_cast<uint32_t>(frac) << 7)) : fadd::pack_half(static_cast<uint16_t>(0x3C00 + frac));
            uint64_t b = is_single ? fadd::pack_single(0x3F800000 - (static_cast<uint32_t>(frac) << 6)) : fadd::pack_half(static_cast<uint16_t>(0x3C00 - frac));
            uint32_t func = fadd::make_func(false, is_single, false, true, false, false, false, 0);
            drive_fadd_no_check(func, a, b);
        }
    };
    close_sweep(true, false);
    close_sweep(false, true);
    for (uint16_t frac = 0x0001; frac < 0x0010; ++frac) {
        uint64_t a = 0x3FF0000000000000ULL + (static_cast<uint64_t>(frac) << 40);
        uint64_t b = 0x3FF0000000000000ULL - (static_cast<uint64_t>(frac) << 39);
        uint32_t func = fadd::make_func(true, false, false, true, false, false, false, 0);
        drive_fadd_no_check(func, a, b);
    }

    const uint32_t func_double_sub = fadd::make_func(true, false, false, true, false, false, false, 0);
    const uint32_t func_double_add = fadd::make_func(true, false, true, false, false, false, false, 0);
    for (int bit = 0; bit < 52; ++bit) {
        uint64_t base = 0x3FF0000000000000ULL;
        uint64_t delta = 1ULL << bit;
        uint64_t a = base;
        uint64_t b = base | delta;
        drive_fadd_no_check(func_double_sub, a, b);
        drive_fadd_no_check(func_double_sub, b, a);
        drive_fadd_no_check(func_double_add, a, b | (1ULL << 63));
    }
    const uint32_t func_half_sub = fadd::make_func(false, false, false, true, false, false, false, 0);
    const uint32_t func_half_add = fadd::make_func(false, false, true, false, false, false, false, 0);
    for (int bit = 0; bit < 10; ++bit) {
        uint16_t delta = static_cast<uint16_t>(1u << bit);
        const uint16_t base_half = 0x3C00u;
        const uint16_t varied = static_cast<uint16_t>(base_half + delta);
        uint64_t a = fadd::pack_half(base_half);
        uint64_t b = fadd::pack_half(varied);
        uint64_t b_neg = fadd::pack_half(static_cast<uint16_t>(varied | 0x8000u));
        drive_fadd_no_check(func_half_sub, a, b);
        drive_fadd_no_check(func_half_sub, b, a);
        drive_fadd_no_check(func_half_add, a, b_neg);
    }

    drive_fadd_no_check(func_double_add, 0x3FF0000000000000ULL, 0x0000000000000000ULL);
    drive_fadd_no_check(func_double_add, 0x3FF0000000000000ULL, 0x7FF0000000000000ULL);
    drive_fadd_no_check(func_double_add, 0x3FF0000000000000ULL, 0x7FEFFFFFFFFFFFFFULL);
    drive_fadd_no_check(func_double_add, 0x3FF0000000000000ULL, 0x7FF0000000000001ULL);
    drive_fadd_no_check(func_half_add, fadd::pack_half(0x3C00), fadd::pack_half(0x0000));
    drive_fadd_no_check(func_half_add, fadd::pack_half(0x3C00), fadd::pack_half(0x7C00));
    drive_fadd_no_check(func_half_add, fadd::pack_half(0x3C00), fadd::pack_half(0x7BFF));
    drive_fadd_no_check(func_half_add, fadd::pack_half(0x3C00), fadd::pack_half(0x7C01));

    for (int bit = 0; bit < 20; ++bit) {
        uint64_t sub = static_cast<uint64_t>(1) << bit;
        drive_fadd_no_check(func_double_add, sub, 0);
        drive_fadd_no_check(func_double_sub, sub << 1, sub);
    }
    for (int bit = 0; bit < 10; ++bit) {
        uint16_t sub = static_cast<uint16_t>(1u << bit);
        uint64_t sub_a = fadd::pack_half(sub);
        drive_fadd_no_check(func_half_add, sub_a, fadd::pack_half(0));
        drive_fadd_no_check(func_half_sub, sub_a, sub_a);
    }

    uint64_t lcg = 1;
    auto next_rand = [&]() {
        lcg = lcg * 1103515245ULL + 12345ULL;
        return lcg;
    };
    for (uint8_t rm_mode = 0; rm_mode < 5; ++rm_mode) {
        for (int i = 0; i < 800; ++i) {
            uint16_t exp = static_cast<uint16_t>((next_rand() % 0x7FE) + 1);
            uint16_t exp1 = (i & 1) ? exp : static_cast<uint16_t>(exp > 0 ? exp - 1 : exp);
            uint64_t frac0 = next_rand() & ((1ULL << 52) - 1);
            uint64_t frac1 = (frac0 + ((next_rand() & 0x7F) + 1)) & ((1ULL << 52) - 1);
            uint64_t src0 = (static_cast<uint64_t>(exp) << 52) | frac0;
            uint64_t src1 = (static_cast<uint64_t>(exp1) << 52) | frac1;
            drive_fadd_no_check(func_double_sub, src0, src1, 0b111, rm_mode);
        }
    }
    for (uint8_t rm_mode = 0; rm_mode < 5; ++rm_mode) {
        for (int i = 0; i < 400; ++i) {
            uint16_t exp = static_cast<uint16_t>((next_rand() % 0x1E) + 1);
            uint16_t exp1 = (i & 1) ? exp : static_cast<uint16_t>(exp ? exp - 1 : 0);
            uint16_t frac0 = static_cast<uint16_t>(next_rand() & 0x3FF);
            uint16_t frac1 = static_cast<uint16_t>((frac0 + static_cast<uint16_t>((next_rand() & 0x1F) + 1)) & 0x3FF);
            uint64_t src0 = fadd::pack_half(static_cast<uint16_t>((exp << 10) | frac0));
            uint64_t src1 = fadd::pack_half(static_cast<uint16_t>((exp1 << 10) | frac1));
            drive_fadd_no_check(func_half_sub, src0, src1, 0b111, rm_mode);
        }
    }
    for (int i = 0; i < 20000; ++i) {
        uint16_t exp = static_cast<uint16_t>((next_rand() % 0x7FE) + 1);
        uint16_t exp1 = (i & 1) ? exp : static_cast<uint16_t>(exp > 1 ? exp - 1 : exp);
        uint64_t frac0 = next_rand() & ((1ULL << 52) - 1);
        uint64_t tweak = (next_rand() & 0xFFF) + 1;
        uint64_t src0 = (static_cast<uint64_t>(exp) << 52) | frac0;
        uint64_t src1 = (static_cast<uint64_t>(exp1) << 52) | ((frac0 + tweak) & ((1ULL << 52) - 1));
        drive_fadd_no_check(func_double_sub, src0, src1);
    }
    for (int i = 0; i < 5000; ++i) {
        uint16_t exp = static_cast<uint16_t>((next_rand() % 0x1E) + 1);
        uint16_t exp1 = (i & 1) ? exp : static_cast<uint16_t>(exp ? exp - 1 : 0);
        uint16_t frac0 = static_cast<uint16_t>(next_rand() & 0x3FF);
        uint16_t tweak = static_cast<uint16_t>((next_rand() & 0x3F) + 1);
        uint16_t frac1 = static_cast<uint16_t>((frac0 + tweak) & 0x3FF);
        uint64_t src0 = fadd::pack_half(static_cast<uint16_t>((exp << 10) | frac0));
        uint64_t src1 = fadd::pack_half(static_cast<uint16_t>((exp1 << 10) | frac1));
        drive_fadd_no_check(func_half_sub, src0, src1);
    }

    std::vector<fspu::Scenario> fspu_scenarios = {
        {"double_fsgnj", fspu::build_func(true, false, false, false, false, true, false, false), 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL, 0},
        {"single_class", fspu::build_func(false, true, true, false, false, false, false, false), fspu::pack_single_val(0x7FC00000u), 0, 0},
        {"half_fmvfx", fspu::build_func(false, false, false, true, false, false, false, false), fspu::pack_half_val(0x1234u), 0, 0x1122334455667788ULL},
        {"double_fmvfx", fspu::build_func(true, false, false, true, false, false, false, false), 0, 0, 0xDEADBEEFCAFEBABEULL},
        {"single_fmvfx", fspu::build_func(false, true, false, true, false, false, false, false), 0, 0, 0x89ABCDEF11223344ULL},
        {"double_fmvxf", fspu::build_func(true, false, false, false, true, false, false, false), 0x7FF8000000000001ULL, 0, 0},
        {"single_fmvxf", fspu::build_func(false, true, false, false, true, false, false, false), fspu::pack_single_val(0xFF800001u), 0, 0},
        {"half_fmvxf", fspu::build_func(false, false, false, false, true, false, false, false), fspu::pack_half_val(0x7DFFu), 0, 0},
        {"double_class_zero", fspu::build_func(true, false, true, false, false, false, false, false), 0x8000000000000000ULL, 0, 0},
        {"double_class_posinf", fspu::build_func(true, false, true, false, false, false, false, false), 0x7FF0000000000000ULL, 0, 0},
        {"double_class_dn", fspu::build_func(true, false, true, false, false, false, false, false), 0x0000000000000001ULL, 0, 0},
        {"single_class_neginf", fspu::build_func(false, true, true, false, false, false, false, false), fspu::pack_single_val(0xFF800000u), 0, 0},
        {"half_class_qnan", fspu::build_func(false, false, true, false, false, false, false, false), fspu::pack_half_val(0x7E01u), 0, 0},
        {"half_class_dn", fspu::build_func(false, false, true, false, false, false, false, false), fspu::pack_half_val(0x0001u), 0, 0},
        {"single_class_negdn", fspu::build_func(false, true, true, false, false, false, false, false), fspu::pack_single_val(0x80000001u), 0, 0},
        {"single_class_posnm", fspu::build_func(false, true, true, false, false, false, false, false), fspu::pack_single_val(0x3F800000u), 0, 0},
        {"double_class_snan", fspu::build_func(true, false, true, false, false, false, false, false), 0x7FF0000000000001ULL, 0, 0},
        {"single_fsgnjn", fspu::build_func(false, true, false, false, false, false, true, false), fspu::pack_single_val(0x3F800000u), fspu::pack_single_val(0xBF800000u), 0},
        {"single_fsgnjx", fspu::build_func(false, true, false, false, false, false, false, true), fspu::pack_single_val(0xBF800000u), fspu::pack_single_val(0xBF800000u), 0},
        {"single_fsgnj", fspu::build_func(false, true, false, false, false, true, false, false), fspu::pack_single_val(0xBF800000u), fspu::pack_single_val(0x3F800000u), 0},
        {"half_fsgnj", fspu::build_func(false, false, false, false, false, true, false, false), fspu::pack_half_val(0x3C00u), fspu::pack_half_val(0xBC00u), 0},
        {"half_fsgnjn", fspu::build_func(false, false, false, false, false, false, true, false), fspu::pack_half_val(0x3C00u), fspu::pack_half_val(0xBC00u), 0},
        {"half_fsgnjx", fspu::build_func(false, false, false, false, false, false, false, true), fspu::pack_half_val(0x3C00u), fspu::pack_half_val(0xBC00u), 0},
    };
    for (const auto& sc : fspu_scenarios) {
        if (!run_fspu(sc)) return 1;
    }

    auto sign_pairs = std::vector<std::pair<uint64_t, uint64_t>>{{0x3FF0000000000000ULL, 0xBFF0000000000000ULL}, {0xBFF0000000000000ULL, 0x3FF0000000000000ULL}};
    for (auto [a, b] : sign_pairs) {
        drive_fspu_no_check(fspu::build_func(true, false, false, false, false, true, false, false), a, b, 0);
        drive_fspu_no_check(fspu::build_func(true, false, false, false, false, false, true, false), a, b, 0);
        drive_fspu_no_check(fspu::build_func(true, false, false, false, false, false, false, true), a, b, 0);
    }
    auto sign_pairs_single = std::vector<std::pair<uint32_t, uint32_t>>{{0x3F800000u, 0xBF800000u}, {0xBF800000u, 0x3F800000u}};
    for (auto [a, b] : sign_pairs_single) {
        drive_fspu_no_check(fspu::build_func(false, true, false, false, false, true, false, false), fspu::pack_single_val(a), fspu::pack_single_val(b), 0);
        drive_fspu_no_check(fspu::build_func(false, true, false, false, false, false, true, false), fspu::pack_single_val(a), fspu::pack_single_val(b), 0);
        drive_fspu_no_check(fspu::build_func(false, true, false, false, false, false, false, true), fspu::pack_single_val(a), fspu::pack_single_val(b), 0);
    }
    auto sign_pairs_half = std::vector<std::pair<uint16_t, uint16_t>>{{0x3C00u, 0xBC00u}, {0xBC00u, 0x3C00u}};
    for (auto [a, b] : sign_pairs_half) {
        uint64_t pa = fspu::pack_half_val(a);
        uint64_t pb = fspu::pack_half_val(b);
        drive_fspu_no_check(fspu::build_func(false, false, false, false, false, true, false, false), pa, pb, 0);
        drive_fspu_no_check(fspu::build_func(false, false, false, false, false, false, true, false), pa, pb, 0);
        drive_fspu_no_check(fspu::build_func(false, false, false, false, false, false, false, true), pa, pb, 0);
    }

    for (uint64_t mv : {0x0ULL, 0xFFFFFFFFFFFFFFFFULL, 0x12345678ABCDEF01ULL}) {
        drive_fspu_no_check(fspu::build_func(true, false, false, true, false, false, false, false), 0, 0, mv);
        drive_fspu_no_check(fspu::build_func(false, true, false, true, false, false, false, false), 0, 0, mv);
        drive_fspu_no_check(fspu::build_func(false, false, false, true, false, false, false, false), 0, 0, mv);
    }
    for (uint64_t src : {0x3FF8000000000000ULL, 0xBFF0000000000000ULL, 0x7FF8000000000001ULL}) {
        drive_fspu_no_check(fspu::build_func(true, false, false, false, true, false, false, false), src, 0, 0);
    }
    for (uint64_t src : {0x3F800000ULL, 0xBF800000ULL, 0x7F800001ULL}) {
        drive_fspu_no_check(fspu::build_func(false, true, false, false, true, false, false, false), fspu::pack_single_val(static_cast<uint32_t>(src)), 0, 0);
    }
    for (uint64_t src : {fspu::pack_half_val(0x3C00u), fspu::pack_half_val(0xBC00u), fspu::pack_half_val(0x7DFFu)}) {
        drive_fspu_no_check(fspu::build_func(false, false, false, false, true, false, false, false), src, 0, 0);
    }

    std::vector<std::tuple<const char*, uint32_t, uint64_t, uint8_t, uint8_t, bool, bool>> fcnvt_cases = {
        {"double_passthrough", fcnvt::func_bits(true, false, false, false, false, true, true, false, false, false, false), 0x4008000000000000ULL, 0, 0, false, true},
        {"double_to_int", fcnvt::func_bits(true, false, false, false, false, true, false, true, false, false, false), 0x3FF0000000000000ULL, 0, 1, false, true},
        {"double_to_uint", fcnvt::func_bits(true, false, false, false, false, true, false, false, true, false, false), 0x3FF8000000000000ULL, 1, 2, false, true},
        {"double_to_half_narrow", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, true, false), 0x3FF8000000000000ULL, 2, 3, false, true},
        {"double_to_single_narrow", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), 0x4000000000000000ULL, 3, 4, true, true},
        {"single_to_double_widen", fcnvt::func_bits(false, true, true, false, false, true, true, false, false, false, false), fcnvt::pack_single(0x3F800000), 4, 5, false, true},
        {"single_to_half", fcnvt::func_bits(false, true, false, true, false, true, true, false, false, true, false), fcnvt::pack_single(0x40000000), 5, 6, false, true},
        {"half_to_single", fcnvt::func_bits(false, false, true, false, false, false, true, false, false, false, true), fcnvt::pack_half(0x3E00), 6, 7, true, true},
        {"half_to_int", fcnvt::func_bits(false, false, false, false, false, false, false, true, false, false, false), fcnvt::pack_half(0x3800), 7, 0, false, false},
        {"half_to_uint", fcnvt::func_bits(false, false, false, false, false, false, false, false, true, false, false), fcnvt::pack_half(0xBC00), 0, 0, false, false},
        {"double_nan_dqnan", fcnvt::func_bits(true, false, false, false, false, true, false, false, false, false, false), 0x7FF8000000000000ULL, 0, 0, true, false},
        {"double_to_half_denorm", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, true, false), 0x0008000000000000ULL, 1, 2, false, false},
        {"double_to_half_overflow", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, true, false), 0x7FEFFFFFFFFFFFFFULL, 2, 3, false, true},
        {"double_to_single_denorm", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), 0x0010000000000000ULL, 3, 4, false, false},
        {"double_to_single_overflow", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), 0x7FEFFFFFFFFFFFFFULL, 4, 5, false, true},
        {"sover_toggle", fcnvt::func_bits(true, false, true, true, false, true, true, false, false, false, true), 0x3FF0000000000000ULL, 5, 6, true, true},
        {"double_snan_to_int", fcnvt::func_bits(true, false, false, false, false, true, false, true, false, false, false), 0x7FF0000000000001ULL, 0, 0, false, true},
        {"double_inf_to_uint", fcnvt::func_bits(true, false, false, false, false, true, false, false, true, false, false), 0x7FF0000000000000ULL, 0, 0, false, true},
        {"double_snan_to_single", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), 0x7FF0000000000001ULL, 0, 0, false, true},
    };
    for (const auto& entry : fcnvt_cases) {
        if (!run_fcnvt(std::get<0>(entry), std::get<1>(entry), std::get<2>(entry), std::get<3>(entry), std::get<4>(entry), std::get<5>(entry), std::get<6>(entry))) return 1;
    }

    for (int sign : {0, 1}) {
        for (int exp : {0, 1, 2, 5, 10, 20, 50, 100, 200, 400, 800, 1000, 1022, 1023}) {
            for (uint64_t frac : {0ULL, 1ULL << 51, 0x000FFFFFFFFFFFFULL, 0x001000000000000ULL, 0x000800000000000ULL}) {
                uint64_t val = fcnvt::make_double(sign, static_cast<uint16_t>(exp), frac);
                uint8_t rm = static_cast<uint8_t>((exp + sign) & 0x7);
                (void)run_fcnvt("dtoh_sweep", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, true, false), val, 0, rm, false, false);
                (void)run_fcnvt("dtos_sweep", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), val, 1, rm, false, false);
            }
        }
    }

    for (int exp = 850; exp <= 1023; ++exp) {
        uint64_t frac = (exp & 1) ? (1ULL << 51) : 0x000123456789ABULL;
        uint64_t val_pos = fcnvt::make_double(0, static_cast<uint16_t>(exp), frac);
        uint64_t val_neg = fcnvt::make_double(1, static_cast<uint16_t>(exp), frac >> 1);
        (void)run_fcnvt("dtoh_exp_walk_pos", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, true, false), val_pos, 0, 0, false, false);
        (void)run_fcnvt("dtoh_exp_walk_neg", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, true, false), val_neg, 0, 1, false, false);
        (void)run_fcnvt("dtos_exp_walk_pos", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), val_pos, 1, 2, false, false);
        (void)run_fcnvt("dtos_exp_walk_neg", fcnvt::func_bits(true, false, false, true, false, true, true, false, false, false, true), val_neg, 1, 3, false, false);
    }

    for (int bit = 0; bit <= 22; ++bit) {
        uint32_t frac = 1u << bit;
        uint64_t single_denorm = fcnvt::pack_single(frac);
        (void)run_fcnvt("stod_denorm_walk", fcnvt::func_bits(false, true, true, false, false, true, true, false, false, false, false), single_denorm, 0, 0, false, false);
    }

    for (int bit = 0; bit < 64; ++bit) {
        uint64_t pos = 1ULL << bit;
        uint64_t neg = pos | (1ULL << 63);
        (void)run_fcnvt("itod_pos", fcnvt::func_bits(true, false, false, false, true, false, true, false, false, false, false), pos, 0, 0, false, true);
        (void)run_fcnvt("itod_neg", fcnvt::func_bits(true, false, false, false, true, false, true, false, false, false, false), neg, 0, 0, false, true);
        (void)run_fcnvt("itos_pos", fcnvt::func_bits(true, false, false, false, true, false, true, false, false, false, true), pos, 0, 1, false, true);
        (void)run_fcnvt("itos_neg", fcnvt::func_bits(true, false, false, false, true, false, true, false, false, false, true), neg, 0, 1, false, true);
        (void)run_fcnvt("itoh_pos", fcnvt::func_bits(true, false, false, false, true, false, true, false, false, true, false), pos, 0, 2, false, true);
        (void)run_fcnvt("itoh_neg", fcnvt::func_bits(true, false, false, false, true, false, true, false, false, true, false), neg, 0, 2, false, true);
    }

    uint64_t lcg2 = 11;
    auto next_rand2 = [&]() {
        lcg2 = lcg2 * 6364136223846793005ULL + 1;
        return lcg2;
    };
    for (int i = 0; i < 4096; ++i) {
        const bool src_l64 = (i & 1) == 0;
        const bool src_l32 = (i & 1) != 0;
        const bool widden = (i & 2) != 0;
        const bool narrow = (i & 4) != 0;
        const bool src_si = (i & 8) != 0;
        const bool src_float = (i & 16) != 0;
        const bool dest_float = (i & 32) != 0;
        const bool dest_si = (i & 64) != 0;
        const bool dest_ui = (i & 128) != 0;
        const bool dest_h = (i & 256) != 0;
        const bool dest_s = (i & 512) != 0;
        uint32_t func = fcnvt::func_bits(src_l64, src_l32, widden, narrow, src_si, src_float, dest_float, dest_si, dest_ui, dest_h, dest_s);
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_imm0 = next_rand2() & 0x7;
        dut.vfpu_yy_xx_rm = next_rand2() & 0x7;
        dut.vfpu_yy_xx_dqnan = (next_rand2() >> 3) & 1;
        dut.dp_vfalu_ex1_pipex_srcf0 = next_rand2();
        dut.dp_vfalu_ex1_pipex_sel = 0b100;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
    }

    const std::vector<std::pair<uint64_t, uint64_t>> fadd_overflow_cases = {
        {0x7FEFFFFFFFFFFFFFULL, 0x7FEFFFFFFFFFFFFFULL},
        {0x7FF0000000000000ULL, 0x3FF0000000000000ULL},
        {0x7FF0000000000001ULL, 0x3FF0000000000000ULL},
    };
    const std::vector<std::pair<uint64_t, uint64_t>> fadd_half_overflow_cases = {
        {fadd::pack_half(0x7BFF), fadd::pack_half(0x7BFF)},
        {fadd::pack_half(0x7C00), fadd::pack_half(0x3C00)},
        {fadd::pack_half(0x7C01), fadd::pack_half(0x3C00)},
    };
    for (int rep = 0; rep < 3; ++rep) {
        for (auto [a, b] : fadd_overflow_cases) {
            drive_fadd_no_check(func_double_add, a, b);
            drive_fadd_no_check(func_double_sub, a, b);
        }
        for (auto [a, b] : fadd_half_overflow_cases) {
            drive_fadd_no_check(func_half_add, a, b);
            drive_fadd_no_check(func_half_sub, a, b);
        }
    }

    for (int repeat = 0; repeat < 2; ++repeat) {
        for (const auto& sc : fadd_basic) {
            if (!run_fadd(sc)) return 1;
        }
        if (!run_fadd({"double_maxnm_repeat", fadd::make_func(true, false, false, false, false, true, false, 0), 0xBFF0000000000000ULL, 0x4008000000000000ULL, 0x4008000000000000ULL, 0})) return 1;
        if (!run_fadd({"single_minnm_repeat", fadd::make_func(false, true, false, false, false, false, true, 0), fadd::pack_single(0x7F800000), fadd::pack_single(0x3F000000), fadd::pack_single(0x3F000000), 0})) return 1;
        if (!run_fadd({"half_maxnm_repeat", fadd::make_func(false, false, false, false, false, true, false, 0), fadd::pack_half(0xFC00), fadd::pack_half(0x3C00), fadd::pack_half(0x3C00), 0})) return 1;

        if (!run_cmp("cmp_feq_single_repeat", fadd::make_func(false, true, false, false, true, false, false, 0b00001), fadd::pack_single(0x3F800000), fadd::pack_single(0x3F800000), 1)) return 1;
        if (!run_cmp("cmp_flt_double_repeat", fadd::make_func(true, false, false, false, true, false, false, 0b00010), 0x3FF0000000000000ULL, 0x4008000000000000ULL, 1)) return 1;
        if (!run_cmp("cmp_fle_half_repeat", fadd::make_func(false, false, false, false, true, false, false, 0b00100), fadd::pack_half(0x3800), fadd::pack_half(0x3C00), 1)) return 1;

        dut.vfpu_yy_xx_rm = 0;
        if (!run_fadd(rounding[0])) return 1;
        if (!run_fadd(rounding[1])) return 1;
        dut.vfpu_yy_xx_rm = 0b011;
        if (!run_fadd(rounding[2])) return 1;
        dut.vfpu_yy_xx_rm = 0b100;
        if (!run_fadd(rounding[3])) return 1;
        dut.vfpu_yy_xx_rm = 0;

        dut.vfpu_yy_xx_dqnan = 1;
        if (!run_fadd({"single_nan_add_repeat", fadd::make_func(false, true, true, false, false, false, false, 0), fadd::pack_single(0x7FC00001), fadd::pack_single(0x3F800000), fadd::pack_single(0x7FC00001), 0})) return 1;
        dut.vfpu_yy_xx_dqnan = 0;

        for (const auto& sc : fspu_scenarios) {
            if (!run_fspu(sc)) return 1;
        }
        for (const auto& entry : fcnvt_cases) {
            if (!run_fcnvt(std::get<0>(entry), std::get<1>(entry), std::get<2>(entry), std::get<3>(entry), std::get<4>(entry), std::get<5>(entry), std::get<6>(entry))) return 1;
        }
    }

    dut.cp0_yy_clk_en = 0;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    tick(dut);
    dut.cp0_vfpu_icg_en = 0;
    tick(dut);
    dut.cp0_vfpu_icg_en = 1;

    // Near-cancel sweeps to exercise close/ff1 paths through public interfaces
    for (int bit = 0; bit < 52; ++bit) {
        const uint64_t frac = 1ULL << bit;
        const uint64_t a = 0x3FF0000000000000ULL | frac;   // +1.frac
        const uint64_t b = 0xBFF0000000000000ULL;          // -1.0
        drive_fadd_no_check(func_double_add, a, b);
    }
    for (int bit = 0; bit < 10; ++bit) {
        const uint16_t frac = static_cast<uint16_t>(1u << bit);
        const uint64_t a = fadd::pack_half(static_cast<uint16_t>(0x3C00u | frac));  // +1.frac
        const uint64_t b = fadd::pack_half(static_cast<uint16_t>(0xBC00u));         // -1.0
        drive_fadd_no_check(func_half_add, a, b);
    }

    logger.summary();

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case032/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
