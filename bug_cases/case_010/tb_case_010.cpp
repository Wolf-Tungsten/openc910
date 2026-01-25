#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "verilated.h"
#include "verilated_cov.h"
#include "Vuart_receive.h"

namespace {

constexpr int kCyclesPerBit = 16;
constexpr int kStartCycles = 16;
constexpr int kIdleCycles = 8;

struct Monitor {
    bool saw_busy = false;
    bool saw_over = false;
    bool saw_pe = false;
    bool saw_fe = false;
    uint8_t rdata_at_over = 0;
};

void tick(Vuart_receive *dut, VerilatedContext *context) {
    dut->sys_clk = 0;
    dut->eval();
    context->timeInc(1);
    dut->sys_clk = 1;
    dut->eval();
    context->timeInc(1);
}

void sample(Vuart_receive *dut, Monitor *monitor) {
    if (dut->receive_ctrl_busy) {
        monitor->saw_busy = true;
    }
    if (dut->receive_ctrl_redata_over) {
        monitor->saw_over = true;
        monitor->rdata_at_over = static_cast<uint8_t>(dut->receive_ctrl_rdata);
    }
    if (dut->receive_ctrl_pe) {
        monitor->saw_pe = true;
    }
    if (dut->receive_ctrl_fe) {
        monitor->saw_fe = true;
    }
}

void drive_level(Vuart_receive *dut,
                 VerilatedContext *context,
                 Monitor *monitor,
                 int level,
                 int cycles) {
    dut->s_in = level ? 1 : 0;
    for (int i = 0; i < cycles; ++i) {
        tick(dut, context);
        sample(dut, monitor);
    }
}

bool expect_eq(uint32_t actual, uint32_t expected, const char *label) {
    if (actual != expected) {
        std::cerr << "[TB] " << label << " expected 0x" << std::hex << expected
                  << " got 0x" << actual << std::dec << std::endl;
        return false;
    }
    return true;
}

void apply_reset(Vuart_receive *dut, VerilatedContext *context) {
    dut->receive_clk_en = 0;
    dut->rst_b = 1;
    dut->eval();
    dut->rst_b = 0;
    dut->eval();
    tick(dut, context);
    dut->rst_b = 1;
    dut->eval();
    tick(dut, context);
}

uint8_t shift_in(uint8_t shift, bool bit) {
    const uint8_t msb = static_cast<uint8_t>(bit ? 0x80 : 0x00);
    return static_cast<uint8_t>((shift >> 1) | msb);
}

uint8_t xor_bits(uint8_t data, int bits) {
    uint8_t parity = 0;
    for (int i = 0; i < bits; ++i) {
        parity ^= static_cast<uint8_t>((data >> i) & 0x1u);
    }
    return parity & 0x1u;
}

struct FrameResult {
    Monitor monitor;
    uint8_t shift_reg = 0;
};

FrameResult run_frame(Vuart_receive *dut,
                      VerilatedContext *context,
                      uint8_t data,
                      int data_bits,
                      bool parity_en,
                      bool parity_bit,
                      bool ctrl_parity_bit,
                      bool stop_high,
                      uint32_t stop_length) {
    FrameResult result;
    if (data_bits < 5 || data_bits > 8) {
        std::cerr << "[TB] invalid data_bits " << data_bits << std::endl;
        return result;
    }

    dut->ctrl_receive_data_length = static_cast<uint32_t>(data_bits - 5);
    dut->ctrl_receive_stop_length = stop_length ? 1 : 0;
    dut->ctrl_trans_parity_en = parity_en ? 1 : 0;
    dut->ctrl_receive_parity_en = parity_en ? 1 : 0;
    dut->ctrl_receive_parity_bit = ctrl_parity_bit ? 1 : 0;
    dut->receive_clk_en = 1;
    dut->eval();

    drive_level(dut, context, &result.monitor, 1, kIdleCycles);
    drive_level(dut, context, &result.monitor, 0, kStartCycles);

    uint8_t shift_reg = 0;
    for (int i = 0; i < data_bits; ++i) {
        const bool bit = ((data >> i) & 0x1u) != 0;
        drive_level(dut, context, &result.monitor, bit ? 1 : 0, kCyclesPerBit);
        shift_reg = shift_in(shift_reg, bit);
    }

    if (parity_en) {
        drive_level(dut, context, &result.monitor, parity_bit ? 1 : 0, kCyclesPerBit);
    }

    const int stop_cycles = (stop_length ? 2 : 1) * kCyclesPerBit;
    drive_level(dut, context, &result.monitor, stop_high ? 1 : 0, stop_cycles);
    drive_level(dut, context, &result.monitor, 1, kIdleCycles);

    result.shift_reg = shift_reg;
    return result;
}

bool check_frame(const FrameResult &result,
                 Vuart_receive *dut,
                 bool expect_over,
                 bool expect_pe,
                 bool expect_fe,
                 const char *label) {
    bool ok = true;
    if (!result.monitor.saw_busy) {
        std::cerr << "[TB] " << label << " busy never asserted" << std::endl;
        ok = false;
    }
    if (expect_over != result.monitor.saw_over) {
        std::cerr << "[TB] " << label << " redata_over expected "
                  << (expect_over ? "1" : "0") << " got "
                  << (result.monitor.saw_over ? "1" : "0") << std::endl;
        ok = false;
    }
    if (expect_pe != result.monitor.saw_pe) {
        std::cerr << "[TB] " << label << " parity flag expected "
                  << (expect_pe ? "1" : "0") << " got "
                  << (result.monitor.saw_pe ? "1" : "0") << std::endl;
        ok = false;
    }
    if (expect_fe != result.monitor.saw_fe) {
        std::cerr << "[TB] " << label << " frame error expected "
                  << (expect_fe ? "1" : "0") << " got "
                  << (result.monitor.saw_fe ? "1" : "0") << std::endl;
        ok = false;
    }
    if (expect_over && result.monitor.saw_over) {
        if (!expect_eq(static_cast<uint32_t>(result.monitor.rdata_at_over),
                       static_cast<uint32_t>(result.shift_reg),
                       label)) {
            ok = false;
        }
    }
    if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_busy), 0, "idle_after_frame")) {
        ok = false;
    }
    return ok;
}

} // namespace

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);

    auto dut = std::make_unique<Vuart_receive>(context.get());
    dut->sys_clk = 0;
    dut->rst_b = 0;
    dut->receive_clk_en = 0;
    dut->s_in = 1;
    dut->ctrl_receive_data_length = 0;
    dut->ctrl_receive_parity_bit = 0;
    dut->ctrl_receive_parity_en = 0;
    dut->ctrl_receive_stop_length = 0;
    dut->ctrl_trans_parity_en = 0;
    dut->eval();

    apply_reset(dut.get(), context.get());

    if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_busy), 0, "reset_busy")) {
        return EXIT_FAILURE;
    }
    if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_fe), 0, "reset_fe")) {
        return EXIT_FAILURE;
    }
    if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_pe), 0, "reset_pe")) {
        return EXIT_FAILURE;
    }
    if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_redata_over), 0, "reset_over")) {
        return EXIT_FAILURE;
    }
    if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_rdata), 0, "reset_rdata")) {
        return EXIT_FAILURE;
    }

    dut->receive_clk_en = 0;
    dut->s_in = 0;
    for (int i = 0; i < 4; ++i) {
        tick(dut.get(), context.get());
        if (!expect_eq(static_cast<uint32_t>(dut->receive_ctrl_busy), 0, "clk_en_hold_busy")) {
            return EXIT_FAILURE;
        }
    }

    dut->s_in = 1;
    tick(dut.get(), context.get());

    const uint8_t data_a = 0x15;
    FrameResult frame_a =
        run_frame(dut.get(), context.get(), data_a, 5, false, false, false, true, 0);
    if (!check_frame(frame_a, dut.get(), true, false, false, "frame_no_parity")) {
        return EXIT_FAILURE;
    }

    const uint8_t data_b = 0xA5;
    const uint8_t parity_bit_b = 1;
    const uint8_t parity_cout_b = static_cast<uint8_t>(xor_bits(data_b, 8) ^ parity_bit_b);
    const uint8_t ctrl_parity_b = parity_cout_b;
    const bool expect_pe_b = (parity_cout_b == ctrl_parity_b);
    FrameResult frame_b = run_frame(dut.get(),
                                    context.get(),
                                    data_b,
                                    8,
                                    true,
                                    parity_bit_b != 0,
                                    ctrl_parity_b != 0,
                                    true,
                                    1);
    if (!check_frame(frame_b, dut.get(), false, expect_pe_b, false, "frame_parity_match")) {
        return EXIT_FAILURE;
    }

    const uint8_t data_c = 0x0B;
    const uint8_t parity_bit_c = 0;
    const uint8_t parity_cout_c = static_cast<uint8_t>(xor_bits(data_c, 5) ^ parity_bit_c);
    const uint8_t ctrl_parity_c = parity_cout_c ^ 0x1u;
    const bool expect_pe_c = (parity_cout_c == ctrl_parity_c);
    FrameResult frame_c = run_frame(dut.get(),
                                    context.get(),
                                    data_c,
                                    5,
                                    true,
                                    parity_bit_c != 0,
                                    ctrl_parity_c != 0,
                                    true,
                                    0);
    if (!check_frame(frame_c, dut.get(), true, expect_pe_c, false, "frame_parity_mismatch")) {
        return EXIT_FAILURE;
    }

    const uint8_t data_d = 0x0C;
    FrameResult frame_d =
        run_frame(dut.get(), context.get(), data_d, 5, false, false, false, false, 0);
    if (!check_frame(frame_d, dut.get(), false, false, true, "frame_framing_error")) {
        return EXIT_FAILURE;
    }

    std::cout << "[TB] case_010 passed: uart_receive state, parity, and framing coverage" << std::endl;

#if VM_COVERAGE
    const char *covPath = std::getenv("VERILATOR_COV_FILE");
    if (covPath == nullptr || covPath[0] == '\0') {
        covPath = "coverage.dat";
    }
    VerilatedCov::write(covPath);
#endif
    return EXIT_SUCCESS;
}
