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

    uint8_t cpu_awlen = 0;
    uint32_t cpu_awaddr = 0;
    uint16_t cpu_wstrb = 0;
    bool cpu_wvalid = false;
    uint64_t value0 = 0;
    uint64_t value1 = 0;
    uint64_t value2 = 0;

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
        }
        else if ((cycle_count % kLastCycle) == 0)
        {
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
        }
        else if (dut->retire0 || dut->retire1 || dut->retire2)
        {
            retire_inst_in_period++;
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
        cpu_wvalid = (dut->biu_pad_wvalid != 0);
        cpu_wstrb = static_cast<uint16_t>(dut->biu_pad_wstrb);

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
