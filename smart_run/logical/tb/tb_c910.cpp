#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "verilated.h"
#include "verilated_cov.h"

#include "Vsim_top.h"

namespace {

constexpr uint32_t kClkPeriod = 10;
constexpr uint32_t kTclkPeriod = 40;
constexpr uint32_t kMaxRunTime = 0x3000000;
constexpr uint32_t kLastCycle = 50000;
constexpr uint32_t kMemClearLines = 0x16384;
constexpr uint32_t kInstLines = 0x4000;
constexpr uint32_t kDataLines = 0x4000;
constexpr uint32_t kWordsPerLine = 4;
constexpr uint32_t kInstWords = kInstLines * kWordsPerLine;
constexpr uint32_t kDataWords = kDataLines * kWordsPerLine;

uint32_t ByteSwap32(uint32_t value)
{
    return ((value & 0x000000ffU) << 24) |
           ((value & 0x0000ff00U) << 8) |
           ((value & 0x00ff0000U) >> 8) |
           ((value & 0xff000000U) >> 24);
}

bool ReadMemHex(const std::string &path, std::vector<uint32_t> *words)
{
    std::ifstream input(path);
    if (!input)
    {
        return false;
    }

    std::string line;
    size_t index = 0;
    while (std::getline(input, line))
    {
        size_t comment = line.find("//");
        if (comment != std::string::npos)
        {
            line.erase(comment);
        }
        comment = line.find('#');
        if (comment != std::string::npos)
        {
            line.erase(comment);
        }

        std::istringstream tokens(line);
        std::string token;
        while (tokens >> token)
        {
            if (token.empty())
            {
                continue;
            }
            if (token[0] == '@')
            {
                try
                {
                    index = std::stoul(token.substr(1), nullptr, 16);
                }
                catch (const std::exception &)
                {
                    return false;
                }
                if (index > words->size())
                {
                    words->resize(index, 0);
                }
                continue;
            }

            uint32_t value = 0;
            try
            {
                value = static_cast<uint32_t>(std::stoul(token, nullptr, 16));
            }
            catch (const std::exception &)
            {
                return false;
            }

            if (index >= words->size())
            {
                words->resize(index + 1, 0);
            }
            (*words)[index++] = value;
        }
    }

    return true;
}

void WriteReport(const char *text)
{
    std::ofstream report("run_case.report", std::ios::out | std::ios::trunc);
    if (report)
    {
        report << text;
    }
}

}  // namespace

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(false);
    context->commandArgs(argc, argv);

    auto dut = std::make_unique<Vsim_top>(context.get());

    dut->clk = 0;
    dut->jclk = 0;
    dut->rst_b = 0;
    dut->jrst_b = 1;
    dut->jtg_tdi = 0;
    dut->jtg_tms = 0;
    dut->uart0_sin = 1;
    dut->tb_init_en = 0;
    dut->tb_init_addr = 0;
    dut->tb_init_wen = 0xffff;
    dut->tb_init_wdata[0] = 0;
    dut->tb_init_wdata[1] = 0;
    dut->tb_init_wdata[2] = 0;
    dut->tb_init_wdata[3] = 0;

    auto tick_raw = [&]() {
        dut->clk = 0;
        dut->eval();
        context->timeInc(1);

        dut->clk = 1;
        dut->eval();
        context->timeInc(1);
    };

    std::vector<uint32_t> inst_words;
    std::vector<uint32_t> data_words;
    if (!ReadMemHex("inst.pat", &inst_words))
    {
        std::cerr << "[TB] Failed to read inst.pat" << std::endl;
        return EXIT_FAILURE;
    }
    if (!ReadMemHex("data.pat", &data_words))
    {
        std::cerr << "[TB] Failed to read data.pat" << std::endl;
        return EXIT_FAILURE;
    }
    inst_words.resize(kInstWords, 0);
    data_words.resize(kDataWords, 0);

    dut->tb_init_en = 1;
    dut->tb_init_wen = 0x0000;

    dut->tb_init_addr = 0;
    dut->tb_init_wdata[0] = 0;
    dut->tb_init_wdata[1] = 0;
    dut->tb_init_wdata[2] = 0;
    dut->tb_init_wdata[3] = 0;

    for (uint32_t line = 0; line < kMemClearLines; ++line)
    {
        dut->tb_init_addr = line;
        tick_raw();
    }

    for (uint32_t line = 0; line < kInstLines; ++line)
    {
        const uint32_t base = line * kWordsPerLine;
        dut->tb_init_wdata[0] = ByteSwap32(inst_words[base + 0]);
        dut->tb_init_wdata[1] = ByteSwap32(inst_words[base + 1]);
        dut->tb_init_wdata[2] = ByteSwap32(inst_words[base + 2]);
        dut->tb_init_wdata[3] = ByteSwap32(inst_words[base + 3]);
        dut->tb_init_addr = line;
        tick_raw();
    }

    for (uint32_t line = 0; line < kDataLines; ++line)
    {
        const uint32_t base = line * kWordsPerLine;
        dut->tb_init_wdata[0] = ByteSwap32(data_words[base + 0]);
        dut->tb_init_wdata[1] = ByteSwap32(data_words[base + 1]);
        dut->tb_init_wdata[2] = ByteSwap32(data_words[base + 2]);
        dut->tb_init_wdata[3] = ByteSwap32(data_words[base + 3]);
        dut->tb_init_addr = kInstLines + line;
        tick_raw();
    }

    dut->tb_init_en = 0;
    dut->tb_init_wen = 0xffff;
    dut->tb_init_wdata[0] = 0;
    dut->tb_init_wdata[1] = 0;
    dut->tb_init_wdata[2] = 0;
    dut->tb_init_wdata[3] = 0;

    int jclk_cnt = 0;
    int rst_b_cnt = 0;
    int jrst_cnt = 0;
    uint32_t clk_cnt = 0;
    uint32_t cycle_count = 0;
    uint32_t retire_inst_in_period = 0;
    uint32_t retire_print_count = 0;
    const uint32_t kRetirePrintLimit = 100;
    const uint32_t kWindowStart = 2130;
    const uint32_t kWindowEnd = 2185;
    const uint32_t kWindow2Start = 2450;
    const uint32_t kWindow2End = 2550;

    uint8_t cpu_awlen = 0;
    uint32_t cpu_awaddr = 0;
    uint64_t biu_awaddr = 0;
    uint64_t cpu_araddr = 0;
    uint64_t ibiu0_araddr = 0;
    uint64_t ibiu1_araddr = 0;
    uint16_t cpu_wstrb = 0;
    bool cpu_wvalid = false;
    bool cpu_arvalid = false;
    bool biu_awvalid = false;
    bool ibiu0_arvalid = false;
    bool ibiu1_arvalid = false;
    bool ifu_biu_rd_req = false;
    bool ifu_biu_rd_req_gate = false;
    uint64_t ifu_biu_rd_addr = 0;
    uint8_t ifu_biu_rd_len = 0;
    uint8_t ifu_biu_rd_size = 0;
    bool ifu_biu_rd_id = false;
    bool ifu_biu_r_ready = false;
    bool biu_ifu_rd_grnt = false;
    bool biu_ifu_rd_data_vld = false;
    bool biu_ifu_rd_last = false;
    uint8_t biu_ifu_rd_resp = 0;
    uint64_t biu_ifu_rd_data_lo = 0;
    uint64_t biu_ifu_rd_data_hi = 0;
    uint64_t pad_biu_rdata_lo = 0;
    uint64_t pad_biu_rdata_hi = 0;
    uint64_t rdata_s0_lo = 0;
    uint64_t rdata_s0_hi = 0;
    uint64_t rdata_s1_lo = 0;
    uint64_t rdata_s1_hi = 0;
    uint64_t rdata_s2_lo = 0;
    uint64_t rdata_s2_hi = 0;
    uint64_t rdata_s3_lo = 0;
    uint64_t rdata_s3_hi = 0;
    bool rvalid_s0 = false;
    bool rvalid_s1 = false;
    bool rvalid_s2 = false;
    bool rvalid_s3 = false;
    uint8_t rresp_s0 = 0;
    uint8_t rresp_s1 = 0;
    uint8_t rresp_s2 = 0;
    uint8_t rresp_s3 = 0;
    bool arvalid_s0 = false;
    bool arvalid_s1 = false;
    bool arvalid_s2 = false;
    bool arvalid_s3 = false;
    uint64_t fifo_pad_araddr = 0;
    bool fifo_pad_arvalid = false;
    uint32_t had_ifu_ir = 0;
    bool had_ifu_ir_vld = false;
    uint64_t had_ifu_pc = 0;
    bool ifu_had_no_inst = false;
    bool ifu_had_no_op = false;
    bool ifu_had_reset_on = false;
    bool ifu_hpcp_frontend_stall = false;
    bool ifu_hpcp_icache_miss = false;
    bool ifu_mmu_abort = false;
    bool ifu_mmu_va_vld = false;
    uint64_t ifu_mmu_va = 0;
    bool ifu_rst_b = false;
    bool cp0_biu_icg_en = false;
    uint8_t cp0_biu_lpmd_b = 0;
    bool biu_arvalid_int = false;
    bool biu_arvalid_gate = false;
    bool pad_biu_arready = false;
    bool pad_biu_rvalid = false;
    bool saw_ifu_biu_rd_req = false;
    bool saw_ifu_biu_rd_req_gate = false;
    bool saw_biu_arvalid_int = false;
    bool saw_biu_arvalid_gate = false;
    bool saw_ibiu0_arvalid = false;
    bool saw_pad_biu_arready = false;
    bool saw_pad_biu_rvalid = false;
    uint8_t biu_lpmd_b = 0;
    uint64_t pad_cpu_sys_cnt = 0;
    bool core0_cpu_no_retire = false;
    bool cpurst_b = false;
    bool core0_fifo_rst_b = false;
    uint64_t value0 = 0;
    uint64_t value1 = 0;
    uint64_t value2 = 0;
    uint64_t core0_retire0_pc = 0;
    uint64_t core0_retire1_pc = 0;
    uint64_t core0_retire2_pc = 0;

    dut->rst_b = 1;
    dut->jrst_b = 1;
    dut->jclk = 0;

    const int jclk_threshold = (kTclkPeriod / kClkPeriod / 2) - 1;
    while (!context->gotFinish())
    {
        tick_raw();

        clk_cnt++;
        if (clk_cnt > kMaxRunTime)
        {
            std::cout << "**********************************************" << std::endl;
            std::cout << "*   meeting max simulation time, stop!       *" << std::endl;
            std::cout << "**********************************************" << std::endl;
            WriteReport("TEST FAIL");
            break;
        }

        if (!dut->rst_b)
        {
            cycle_count = 1;
        }
        else
        {
            cycle_count++;
        }

        if (!dut->rst_b)
        {
            retire_inst_in_period = 0;
            retire_print_count = 0;
            saw_ifu_biu_rd_req = false;
            saw_ifu_biu_rd_req_gate = false;
            saw_biu_arvalid_int = false;
            saw_biu_arvalid_gate = false;
            saw_ibiu0_arvalid = false;
            saw_pad_biu_arready = false;
            saw_pad_biu_rvalid = false;
        }
        else if ((cycle_count % kLastCycle) == 0)
        {
            std::cout << "[c910-diff] cycle=" << cycle_count
                      << " retire_cnt=" << retire_inst_in_period
                      << " rst_b=" << static_cast<int>(dut->rst_b)
                      << " jrst_b=" << static_cast<int>(dut->jrst_b)
                      << " core_pad_rst_b="
                      << static_cast<int>(dut->core_pad_rst_b)
                      << " sys_cnt=0x" << std::hex << pad_cpu_sys_cnt
                      << " lpmd=0x" << static_cast<int>(biu_lpmd_b)
                      << " no_retire=" << std::dec
                      << static_cast<int>(core0_cpu_no_retire)
                      << " cpurst_b=" << static_cast<int>(cpurst_b)
                      << " fifo_rst_b=" << static_cast<int>(core0_fifo_rst_b)
                      << " axim_clk_en=" << static_cast<int>(dut->axim_clk_en)
                      << " retire0=" << static_cast<int>(dut->retire0)
                      << " retire1=" << static_cast<int>(dut->retire1)
                      << " retire2=" << static_cast<int>(dut->retire2)
                      << " awaddr=0x" << std::hex << cpu_awaddr
                      << " awlen=" << std::dec << static_cast<int>(cpu_awlen)
                      << " wvalid=" << static_cast<int>(cpu_wvalid)
                      << " biu_awaddr=0x" << std::hex << biu_awaddr
                      << " biu_awvalid=" << std::dec
                      << static_cast<int>(biu_awvalid)
                      << " araddr=0x" << std::hex << cpu_araddr
                      << " arvalid=" << std::dec
                      << static_cast<int>(cpu_arvalid)
                      << " ifu_req=" << static_cast<int>(ifu_biu_rd_req)
                      << " ifu_req_gate="
                      << static_cast<int>(ifu_biu_rd_req_gate)
                      << " biu_arvalid_int="
                      << static_cast<int>(biu_arvalid_int)
                      << " biu_arvalid_gate="
                      << static_cast<int>(biu_arvalid_gate)
                      << " pad_arready="
                      << static_cast<int>(pad_biu_arready)
                      << " pad_rvalid="
                      << static_cast<int>(pad_biu_rvalid)
                      << " ifu_req_any="
                      << static_cast<int>(saw_ifu_biu_rd_req)
                      << " ifu_req_gate_any="
                      << static_cast<int>(saw_ifu_biu_rd_req_gate)
                      << " biu_arvalid_any="
                      << static_cast<int>(saw_biu_arvalid_int)
                      << " biu_arvalid_gate_any="
                      << static_cast<int>(saw_biu_arvalid_gate)
                      << " ibiu0_arvalid_any="
                      << static_cast<int>(saw_ibiu0_arvalid)
                      << " pad_arready_any="
                      << static_cast<int>(saw_pad_biu_arready)
                      << " pad_rvalid_any="
                      << static_cast<int>(saw_pad_biu_rvalid)
                      << " ibiu0_araddr=0x" << std::hex << ibiu0_araddr
                      << " ibiu0_arvalid=" << std::dec
                      << static_cast<int>(ibiu0_arvalid)
                      << " ibiu1_araddr=0x" << std::hex << ibiu1_araddr
                      << " ibiu1_arvalid=" << std::dec
                      << static_cast<int>(ibiu1_arvalid) << std::endl;
            if (retire_inst_in_period == 0)
            {
                std::cout << "*************************************************************"
                          << std::endl;
                std::cout << "* Error: There is no instructions retired in the last "
                          << kLastCycle << " cycles! *" << std::endl;
                std::cout << "*              Simulation Fail and Finished!                *"
                          << std::endl;
                std::cout << "*************************************************************"
                          << std::endl;
                WriteReport("TEST FAIL");
                break;
            }
            retire_inst_in_period = 0;
            saw_ifu_biu_rd_req = false;
            saw_ifu_biu_rd_req_gate = false;
            saw_biu_arvalid_int = false;
            saw_biu_arvalid_gate = false;
            saw_ibiu0_arvalid = false;
            saw_pad_biu_arready = false;
            saw_pad_biu_rvalid = false;
        }
        else if (dut->retire0 || dut->retire1 || dut->retire2)
        {
            core0_retire0_pc = dut->core0_retire0_pc;
            core0_retire1_pc = dut->core0_retire1_pc;
            core0_retire2_pc = dut->core0_retire2_pc;
            retire_inst_in_period++;
            if (retire_print_count < kRetirePrintLimit)
            {
                if (dut->retire0 && retire_print_count < kRetirePrintLimit)
                {
                    retire_print_count++;
                    std::cout << "[c910-iret] idx=" << retire_print_count
                              << " cycle=" << cycle_count << " slot=0 pc=0x"
                              << std::hex << core0_retire0_pc << std::dec
                              << std::endl;
                }
                if (dut->retire1 && retire_print_count < kRetirePrintLimit)
                {
                    retire_print_count++;
                    std::cout << "[c910-iret] idx=" << retire_print_count
                              << " cycle=" << cycle_count << " slot=1 pc=0x"
                              << std::hex << core0_retire1_pc << std::dec
                              << std::endl;
                }
                if (dut->retire2 && retire_print_count < kRetirePrintLimit)
                {
                    retire_print_count++;
                    std::cout << "[c910-iret] idx=" << retire_print_count
                              << " cycle=" << cycle_count << " slot=2 pc=0x"
                              << std::hex << core0_retire2_pc << std::dec
                              << std::endl;
                }
            }
        }

        if (value0 == 0x444333222ULL || value1 == 0x444333222ULL ||
            value2 == 0x444333222ULL)
        {
            std::cout << "**********************************************" << std::endl;
            std::cout << "*    simulation finished successfully        *" << std::endl;
            std::cout << "**********************************************" << std::endl;
            WriteReport("TEST PASS");
            break;
        }
        if (value0 == 0x2382348720ULL || value1 == 0x2382348720ULL ||
            value2 == 0x444333222ULL)
        {
            std::cout << "**********************************************" << std::endl;
            std::cout << "*    simulation finished with error          *" << std::endl;
            std::cout << "**********************************************" << std::endl;
            WriteReport("TEST FAIL");
            break;
        }

        if ((cpu_awlen == 0) && (cpu_awaddr == 0x01ffFFF0U) && cpu_wvalid &&
            dut->axim_clk_en)
        {
            const uint32_t wdata0 = dut->biu_pad_wdata[0];
            const uint32_t wdata1 = dut->biu_pad_wdata[1];
            const uint32_t wdata2 = dut->biu_pad_wdata[2];
            const uint32_t wdata3 = dut->biu_pad_wdata[3];
            uint8_t out = 0;

            if (cpu_wstrb == 0x000f)
            {
                out = static_cast<uint8_t>(wdata0 & 0xff);
            }
            else if (cpu_wstrb == 0x00f0)
            {
                out = static_cast<uint8_t>(wdata1 & 0xff);
            }
            else if (cpu_wstrb == 0x0f00)
            {
                out = static_cast<uint8_t>(wdata2 & 0xff);
            }
            else if (cpu_wstrb == 0xf000)
            {
                out = static_cast<uint8_t>(wdata3 & 0xff);
            }

            if (cpu_wstrb == 0x000f || cpu_wstrb == 0x00f0 ||
                cpu_wstrb == 0x0f00 || cpu_wstrb == 0xf000)
            {
                std::cout << static_cast<char>(out) << std::flush;
            }
        }

        value0 = dut->value0;
        value1 = dut->value1;
        value2 = dut->value2;
        cpu_awlen = static_cast<uint8_t>(dut->axi_awlen);
        cpu_awaddr = dut->axi_mem_addr;
        biu_awaddr = dut->biu_pad_awaddr;
        cpu_araddr = dut->biu_pad_araddr;
        ibiu0_araddr = dut->ibiu0_pad_araddr;
        ibiu1_araddr = dut->ibiu1_pad_araddr;
        cpu_wvalid = (dut->biu_pad_wvalid != 0);
        cpu_arvalid = (dut->biu_pad_arvalid != 0);
        biu_awvalid = (dut->biu_pad_awvalid != 0);
        ibiu0_arvalid = (dut->ibiu0_pad_arvalid != 0);
        ibiu1_arvalid = (dut->ibiu1_pad_arvalid != 0);
        ifu_biu_rd_req = (dut->ifu_biu_rd_req != 0);
        ifu_biu_rd_req_gate = (dut->ifu_biu_rd_req_gate != 0);
        ifu_biu_rd_addr = dut->ifu_biu_rd_addr;
        ifu_biu_rd_len = static_cast<uint8_t>(dut->ifu_biu_rd_len);
        ifu_biu_rd_size = static_cast<uint8_t>(dut->ifu_biu_rd_size);
        ifu_biu_rd_id = (dut->ifu_biu_rd_id != 0);
        ifu_biu_r_ready = (dut->ifu_biu_r_ready != 0);
        biu_ifu_rd_grnt = (dut->biu_ifu_rd_grnt != 0);
        biu_ifu_rd_data_vld = (dut->biu_ifu_rd_data_vld != 0);
        biu_ifu_rd_last = (dut->biu_ifu_rd_last != 0);
        biu_ifu_rd_resp = static_cast<uint8_t>(dut->biu_ifu_rd_resp);
        biu_ifu_rd_data_lo = dut->biu_ifu_rd_data_lo;
        biu_ifu_rd_data_hi = dut->biu_ifu_rd_data_hi;
        pad_biu_rdata_lo = dut->pad_biu_rdata_lo;
        pad_biu_rdata_hi = dut->pad_biu_rdata_hi;
        rdata_s0_lo = dut->rdata_s0_lo;
        rdata_s0_hi = dut->rdata_s0_hi;
        rdata_s1_lo = dut->rdata_s1_lo;
        rdata_s1_hi = dut->rdata_s1_hi;
        rdata_s2_lo = dut->rdata_s2_lo;
        rdata_s2_hi = dut->rdata_s2_hi;
        rdata_s3_lo = dut->rdata_s3_lo;
        rdata_s3_hi = dut->rdata_s3_hi;
        rvalid_s0 = (dut->rvalid_s0 != 0);
        rvalid_s1 = (dut->rvalid_s1 != 0);
        rvalid_s2 = (dut->rvalid_s2 != 0);
        rvalid_s3 = (dut->rvalid_s3 != 0);
        rresp_s0 = static_cast<uint8_t>(dut->rresp_s0);
        rresp_s1 = static_cast<uint8_t>(dut->rresp_s1);
        rresp_s2 = static_cast<uint8_t>(dut->rresp_s2);
        rresp_s3 = static_cast<uint8_t>(dut->rresp_s3);
        arvalid_s0 = (dut->arvalid_s0 != 0);
        arvalid_s1 = (dut->arvalid_s1 != 0);
        arvalid_s2 = (dut->arvalid_s2 != 0);
        arvalid_s3 = (dut->arvalid_s3 != 0);
        fifo_pad_araddr = dut->fifo_pad_araddr;
        fifo_pad_arvalid = (dut->fifo_pad_arvalid != 0);
        had_ifu_ir = dut->had_ifu_ir;
        had_ifu_ir_vld = (dut->had_ifu_ir_vld != 0);
        had_ifu_pc = dut->had_ifu_pc;
        ifu_had_no_inst = (dut->ifu_had_no_inst != 0);
        ifu_had_no_op = (dut->ifu_had_no_op != 0);
        ifu_had_reset_on = (dut->ifu_had_reset_on != 0);
        ifu_hpcp_frontend_stall = (dut->ifu_hpcp_frontend_stall != 0);
        ifu_hpcp_icache_miss = (dut->ifu_hpcp_icache_miss != 0);
        ifu_mmu_abort = (dut->ifu_mmu_abort != 0);
        ifu_mmu_va_vld = (dut->ifu_mmu_va_vld != 0);
        ifu_mmu_va = dut->ifu_mmu_va;
        ifu_rst_b = (dut->ifu_rst_b != 0);
        cp0_biu_icg_en = (dut->cp0_biu_icg_en != 0);
        cp0_biu_lpmd_b = static_cast<uint8_t>(dut->cp0_biu_lpmd_b);
        biu_arvalid_int = (dut->biu_arvalid_int != 0);
        biu_arvalid_gate = (dut->biu_arvalid_gate != 0);
        pad_biu_arready = (dut->pad_biu_arready != 0);
        pad_biu_rvalid = (dut->pad_biu_rvalid != 0);
        saw_ifu_biu_rd_req |= ifu_biu_rd_req;
        saw_ifu_biu_rd_req_gate |= ifu_biu_rd_req_gate;
        saw_biu_arvalid_int |= biu_arvalid_int;
        saw_biu_arvalid_gate |= biu_arvalid_gate;
        saw_ibiu0_arvalid |= ibiu0_arvalid;
        saw_pad_biu_arready |= pad_biu_arready;
        saw_pad_biu_rvalid |= pad_biu_rvalid;
        biu_lpmd_b = static_cast<uint8_t>(dut->biu_pad_lpmd_b);
        cpu_wstrb = static_cast<uint16_t>(dut->biu_pad_wstrb);
        pad_cpu_sys_cnt = dut->pad_cpu_sys_cnt;
        core0_cpu_no_retire = (dut->core0_cpu_no_retire != 0);
        cpurst_b = (dut->cpurst_b != 0);
        core0_fifo_rst_b = (dut->core0_fifo_rst_b != 0);

        const bool in_window1 =
            cycle_count >= kWindowStart && cycle_count <= kWindowEnd;
        const bool in_window2 =
            cycle_count >= kWindow2Start && cycle_count <= kWindow2End;
        if (in_window1 || in_window2)
        {
            const char *window_tag = in_window2 ? "c910-window2" : "c910-window";
            std::cout << "[" << window_tag << "] cycle=" << cycle_count
                      << " rst_b=" << static_cast<int>(dut->rst_b)
                      << " cpurst_b=" << static_cast<int>(cpurst_b)
                      << " cp0_icg=" << static_cast<int>(cp0_biu_icg_en)
                      << " cp0_lpmd=0x" << std::hex
                      << static_cast<int>(cp0_biu_lpmd_b) << std::dec
                      << " ifu_rst_b=" << static_cast<int>(ifu_rst_b)
                      << " core_pad_rst_b="
                      << static_cast<int>(dut->core_pad_rst_b)
                      << " retire0=" << static_cast<int>(dut->retire0)
                      << " retire1=" << static_cast<int>(dut->retire1)
                      << " retire2=" << static_cast<int>(dut->retire2)
                      << " pc0=0x" << std::hex << core0_retire0_pc
                      << " pc1=0x" << core0_retire1_pc
                      << " pc2=0x" << core0_retire2_pc << std::dec
                      << " ifu_req=" << static_cast<int>(ifu_biu_rd_req)
                      << " ifu_req_gate="
                      << static_cast<int>(ifu_biu_rd_req_gate)
                      << " ifu_addr=0x" << std::hex << ifu_biu_rd_addr
                      << " ifu_len=" << std::dec
                      << static_cast<int>(ifu_biu_rd_len)
                      << " ifu_size=" << static_cast<int>(ifu_biu_rd_size)
                      << " ifu_id=" << static_cast<int>(ifu_biu_rd_id)
                      << " ifu_r_ready="
                      << static_cast<int>(ifu_biu_r_ready)
                      << " biu_grnt=" << static_cast<int>(biu_ifu_rd_grnt)
                      << " biu_rvld="
                      << static_cast<int>(biu_ifu_rd_data_vld)
                      << " biu_rlast="
                      << static_cast<int>(biu_ifu_rd_last)
                      << " biu_rresp=0x" << std::hex
                      << static_cast<int>(biu_ifu_rd_resp)
                      << " biu_rdata_lo=0x" << biu_ifu_rd_data_lo
                      << " biu_rdata_hi=0x" << biu_ifu_rd_data_hi
                      << " pad_rdata_lo=0x" << pad_biu_rdata_lo
                      << " pad_rdata_hi=0x" << pad_biu_rdata_hi
                      << " rdata_s0_lo=0x" << rdata_s0_lo
                      << " rdata_s0_hi=0x" << rdata_s0_hi
                      << " rvalid_s0=" << std::dec
                      << static_cast<int>(rvalid_s0)
                      << " rresp_s0=0x" << std::hex
                      << static_cast<int>(rresp_s0)
                      << " rdata_s1_lo=0x" << rdata_s1_lo
                      << " rdata_s1_hi=0x" << rdata_s1_hi
                      << " rvalid_s1=" << std::dec
                      << static_cast<int>(rvalid_s1)
                      << " rresp_s1=0x" << std::hex
                      << static_cast<int>(rresp_s1)
                      << " rdata_s2_lo=0x" << rdata_s2_lo
                      << " rdata_s2_hi=0x" << rdata_s2_hi
                      << " rvalid_s2=" << std::dec
                      << static_cast<int>(rvalid_s2)
                      << " rresp_s2=0x" << std::hex
                      << static_cast<int>(rresp_s2)
                      << " rdata_s3_lo=0x" << rdata_s3_lo
                      << " rdata_s3_hi=0x" << rdata_s3_hi
                      << " rvalid_s3=" << std::dec
                      << static_cast<int>(rvalid_s3)
                      << " rresp_s3=0x" << std::hex
                      << static_cast<int>(rresp_s3)
                      << " had_ir_vld=" << std::dec
                      << static_cast<int>(had_ifu_ir_vld)
                      << " had_ir=0x" << std::hex << had_ifu_ir
                      << " had_pc=0x" << had_ifu_pc
                      << " ifu_no_inst=" << std::dec
                      << static_cast<int>(ifu_had_no_inst)
                      << " ifu_no_op="
                      << static_cast<int>(ifu_had_no_op)
                      << " ifu_reset_on="
                      << static_cast<int>(ifu_had_reset_on)
                      << " ifu_fe_stall="
                      << static_cast<int>(ifu_hpcp_frontend_stall)
                      << " ifu_ic_miss="
                      << static_cast<int>(ifu_hpcp_icache_miss)
                      << " ifu_mmu_abort="
                      << static_cast<int>(ifu_mmu_abort)
                      << " ifu_mmu_va_vld="
                      << static_cast<int>(ifu_mmu_va_vld)
                      << " ifu_mmu_va=0x" << std::hex << ifu_mmu_va
                      << std::dec
                      << " biu_arvalid="
                      << static_cast<int>(biu_arvalid_int)
                      << " biu_arvalid_gate="
                      << static_cast<int>(biu_arvalid_gate)
                      << " pad_arready="
                      << static_cast<int>(pad_biu_arready)
                      << " pad_rvalid="
                      << static_cast<int>(pad_biu_rvalid)
                      << " araddr=0x" << std::hex << cpu_araddr
                      << " fifo_araddr=0x" << fifo_pad_araddr
                      << " fifo_arvalid=" << std::dec
                      << static_cast<int>(fifo_pad_arvalid)
                      << " arvalid_s0=" << static_cast<int>(arvalid_s0)
                      << " arvalid_s1=" << static_cast<int>(arvalid_s1)
                      << " arvalid_s2=" << static_cast<int>(arvalid_s2)
                      << " arvalid_s3=" << static_cast<int>(arvalid_s3)
                      << " ibiu0_araddr=0x" << std::hex << ibiu0_araddr
                      << " ibiu0_arvalid=" << std::dec
                      << static_cast<int>(ibiu0_arvalid) << std::endl;
        }

        if (jclk_cnt < jclk_threshold)
        {
            jclk_cnt++;
        }
        else
        {
            jclk_cnt = 0;
            dut->jclk = !dut->jclk;
        }

        rst_b_cnt++;
        if (rst_b_cnt > 10 && rst_b_cnt < 20)
        {
            dut->rst_b = 0;
        }
        else if (rst_b_cnt > 20)
        {
            dut->rst_b = 1;
        }

        jrst_cnt++;
        if (jrst_cnt > 40 && jrst_cnt < 80)
        {
            dut->jrst_b = 0;
        }
        else if (jrst_cnt > 80)
        {
            dut->jrst_b = 1;
        }

        dut->eval();
    }

    dut->final();

#if VM_COVERAGE
    const char *cov_path = std::getenv("VERILATOR_COV_FILE");
    if (cov_path == nullptr || cov_path[0] == '\0')
    {
        cov_path = "coverage.dat";
    }
    VerilatedCov::write(cov_path);
#endif

    return EXIT_SUCCESS;
}
