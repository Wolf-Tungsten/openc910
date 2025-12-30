#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_round.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

void tick(Vct_vfdsu_round& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

struct Scenario {
    std::string name;
    uint64_t total_qt_rt_58;
    bool ex2_of_rm_lfn;
    uint16_t doub_expnt;
    bool double_mode;
    bool single_mode;
    bool dz;
    uint16_t half_expnt;
    bool id_srt_skip;
    bool nv;
    bool of;
    bool potnt_of;
    bool potnt_uf;
    uint64_t qnan_f;
    bool qnan_sign;
    bool rem_sign;
    bool rem_zero;
    uint64_t result_denorm_round_add_num;
    bool result_inf;
    bool result_lfn;
    bool result_qnan;
    bool result_sign;
    bool result_zero;
    uint8_t rm;
    bool rslt_denorm;
    uint16_t sing_expnt;
    bool uf;
};

void drive(Vct_vfdsu_round& dut, const Scenario& sc) {
    dut.total_qt_rt_58 = sc.total_qt_rt_58 & ((1ULL << 58) - 1);
    dut.vfdsu_ex2_of_rm_lfn = sc.ex2_of_rm_lfn;
    dut.vfdsu_ex3_doub_expnt_rst = sc.doub_expnt & 0x1fff;
    dut.vfdsu_ex3_double = sc.double_mode;
    dut.vfdsu_ex3_dz = sc.dz;
    dut.vfdsu_ex3_half_expnt_rst = sc.half_expnt & 0x1fff;
    dut.vfdsu_ex3_id_srt_skip = sc.id_srt_skip;
    dut.vfdsu_ex3_nv = sc.nv;
    dut.vfdsu_ex3_of = sc.of;
    dut.vfdsu_ex3_potnt_of = sc.potnt_of;
    dut.vfdsu_ex3_potnt_uf = sc.potnt_uf;
    dut.vfdsu_ex3_qnan_f = sc.qnan_f & ((1ULL << 52) - 1);
    dut.vfdsu_ex3_qnan_sign = sc.qnan_sign;
    dut.vfdsu_ex3_rem_sign = sc.rem_sign;
    dut.vfdsu_ex3_rem_zero = sc.rem_zero;
    dut.vfdsu_ex3_result_denorm_round_add_num = sc.result_denorm_round_add_num & ((1ULL << 53) - 1);
    dut.vfdsu_ex3_result_inf = sc.result_inf;
    dut.vfdsu_ex3_result_lfn = sc.result_lfn;
    dut.vfdsu_ex3_result_qnan = sc.result_qnan;
    dut.vfdsu_ex3_result_sign = sc.result_sign;
    dut.vfdsu_ex3_result_zero = sc.result_zero;
    dut.vfdsu_ex3_rm = sc.rm & 0x7;
    dut.vfdsu_ex3_rslt_denorm = sc.rslt_denorm;
    dut.vfdsu_ex3_sing_expnt_rst = sc.sing_expnt & 0x1ff;
    dut.vfdsu_ex3_single = sc.single_mode;
    dut.vfdsu_ex3_uf = sc.uf;

    dut.ex3_pipedown = 1;
    tick(dut);
    dut.ex3_pipedown = 0;
    tick(dut);
}

uint64_t build_qt(uint8_t hi_pattern, uint64_t low) {
    uint64_t v = (static_cast<uint64_t>(hi_pattern & 0x3) << 56) | (low & ((1ULL << 56) - 1));
    return v & ((1ULL << 58) - 1);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_round dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    std::vector<Scenario> scenarios;

    // Directed RM coverage
    for (uint8_t rm = 0; rm <= 4; ++rm) {
        scenarios.push_back({"rm_case_" + std::to_string(rm),
                             build_qt(1, 0x123456789ULL),
                             false,
                             0x1c10,
                             true,
                             false,
                             (rm & 1),
                             0x1ff2,
                             false,
                             (rm & 2),
                             (rm & 4),
                             (rm & 1),
                             (rm & 2),
                             0x1111111111111ULL,
                             static_cast<bool>(rm & 1),
                             static_cast<bool>(rm & 2),
                             false,
                             0x15555ULL,
                             false,
                             false,
                             false,
                             static_cast<bool>(rm & 1),
                             false,
                             rm,
                             false,
                             0x180,
                             static_cast<bool>(rm & 1)});
    }

    // Sweep double denorm case table
    for (uint16_t expnt = 0x1c02; expnt >= 0x1bcf; --expnt) {
        scenarios.push_back({"d_denorm_" + std::to_string(expnt),
                             build_qt(0, 0x3ffffffffffffULL),
                             false,
                             expnt,
                             true,
                             false,
                             false,
                             0x1ff2,
                             false,
                             false,
                             false,
                             false,
                             false,
                             0,
                             false,
                             false,
                             true,
                             0,
                             false,
                             false,
                             false,
                             false,
                             false,
                             0,
                             true,
                             0x180,
                             false});
        if (expnt == 0x1bcf) break;
    }

    // Sweep single denorm table
    for (uint16_t expnt = 0x182; expnt >= 0x16b; --expnt) {
        scenarios.push_back({"s_denorm_" + std::to_string(expnt),
                             build_qt(0, 0x1ffffffffULL),
                             false,
                             0x1c10,
                             false,
                             true,
                             false,
                             0x1ff2,
                             false,
                             false,
                             false,
                             false,
                             false,
                             0,
                             false,
                             false,
                             true,
                             0,
                             false,
                             false,
                             false,
                             false,
                             false,
                             0,
                             true,
                             expnt,
                             false});
        if (expnt == 0x16b) break;
    }

    // Sweep half denorm table
    for (uint16_t expnt = 0x1ff2; expnt >= 0x1fe8; --expnt) {
        scenarios.push_back({"h_denorm_" + std::to_string(expnt),
                             build_qt(0, 0x2aaaaaaaaaaaULL),
                             false,
                             0x1c10,
                             false,
                             false,
                             false,
                             expnt,
                             false,
                             false,
                             false,
                             false,
                             false,
                             0,
                             false,
                             false,
                             true,
                             0,
                             false,
                             false,
                             false,
                             false,
                             false,
                             0,
                             true,
                             0x180,
                             false});
        if (expnt == 0x1fe8) break;
    }

    // Additional directed paths
    scenarios.push_back({"d_denorm_1bce",
                         build_qt(0, 0x123456789abcULL),
                         true,
                         0x1bce,
                         true,
                         false,
                         false,
                         0x1ff2,
                         true,
                         true,
                         true,
                         true,
                         true,
                         0x3ffffULL,
                         true,
                         false,
                         false,
                         0x7ffULL,
                         false,
                         false,
                         false,
                         true,
                         false,
                         4,
                         true,
                         0x180,
                         true});

    scenarios.push_back({"single_bit56",
                         build_qt(1, 0xdeadbeefULL),
                         false,
                         0x1c10,
                         false,
                         true,
                         true,
                         0x1ff2,
                         false,
                         false,
                         false,
                         false,
                         false,
                         0x55ULL,
                         false,
                         false,
                         false,
                         0,
                         false,
                         false,
                         false,
                         false,
                         false,
                         1,
                         true,
                         0x182,
                         false});

    scenarios.push_back({"half_bit56",
                         build_qt(1, 0xbeefULL),
                         false,
                         0x1c10,
                         false,
                         false,
                         false,
                         0x1ff2,
                         false,
                         false,
                         false,
                         false,
                         false,
                         0x1ULL,
                         false,
                         false,
                         true,
                         0,
                         false,
                         false,
                         false,
                         false,
                         false,
                         4,
                         true,
                         0x180,
                         false});

    scenarios.push_back({"double_bit56",
                         build_qt(1, 0x123456ULL),
                         false,
                         0x1c20,
                         true,
                         false,
                         false,
                         0x1ff2,
                         false,
                         false,
                         false,
                         false,
                         false,
                         0x2ULL,
                         false,
                         false,
                         false,
                         0,
                         false,
                         false,
                         false,
                         false,
                         false,
                         2,
                         false,
                         0x180,
                         false});

    scenarios.push_back({"both_modes_default",
                         build_qt(0, 0x222222ULL),
                         false,
                         0x1c10,
                         true,
                         true,
                         false,
                         0x1ff2,
                         false,
                         false,
                         false,
                         false,
                         false,
                         0x4ULL,
                         false,
                         false,
                         false,
                         0,
                         false,
                         false,
                         false,
                         false,
                         false,
                         0,
                         false,
                         0x180,
                         false});

    scenarios.push_back({"id_srt_skip",
                         build_qt(0, 0x13579ULL),
                         true,
                         0x1c05,
                         true,
                         false,
                         false,
                         0x1ff2,
                         true,
                         false,
                         true,
                         true,
                         true,
                         0xabcdefULL,
                         true,
                         true,
                         false,
                         0x12345ULL,
                         false,
                         false,
                         false,
                         true,
                         true,
                         0,
                         true,
                         0x180,
                         true});

    // Random scenarios to hit rounding add/sub logic
    uint64_t lcg = 2024;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 300; ++i) {
        Scenario sc{
            "rand_" + std::to_string(i),
            next() & ((1ULL << 58) - 1),
            (next() & 1) != 0,
            static_cast<uint16_t>(next() & 0x1fff),
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            static_cast<uint16_t>(next() & 0x1fff),
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            next() & ((1ULL << 52) - 1),
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            next() & ((1ULL << 53) - 1),
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            (next() & 1) != 0,
            static_cast<uint8_t>(next() & 0x7),
            (next() & 1) != 0,
            static_cast<uint16_t>(next() & 0x1ff),
            (next() & 1) != 0};
        if (sc.double_mode && sc.single_mode) sc.single_mode = false;
        if (!sc.double_mode && !sc.single_mode) sc.double_mode = true;
        sc.rslt_denorm = sc.rslt_denorm || ((sc.total_qt_rt_58 & (1ULL << 56)) == 0);
        drive(dut, sc);
    }

    for (const auto& sc : scenarios) {
        drive(dut, sc);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case036/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
