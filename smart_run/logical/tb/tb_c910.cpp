#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include "verilated.h"
#include "verilated_cov.h"

#if VM_TRACE_FST
#include "verilated_fst_c.h"
#endif

#include "Vsim_top.h"

namespace {

constexpr uint32_t kClkPeriod = 10;
constexpr uint32_t kTclkPeriod = 40;
constexpr uint32_t kMaxRunTime = 0x3000000;
constexpr uint32_t kLastCycle = 50000;

// Configuration from environment variables
// Returns 0 for unlimited simulation (run to completion)
uint32_t GetMaxSimCycles()
{
    const char* env = std::getenv("C910_SIM_MAX_CYCLE");
    if (env != nullptr && env[0] != '\0')
    {
        try
        {
            return static_cast<uint32_t>(std::stoul(env));
        }
        catch (const std::exception&)
        {
            // Use default
        }
    }
    return 5000;  // Default 5000 cycles
}

bool IsWaveformEnabled()
{
    const char* env = std::getenv("C910_WAVEFORM");
    return env != nullptr && std::strcmp(env, "1") == 0;
}

const char* GetWaveformPath()
{
    const char* env = std::getenv("C910_WAVEFORM_PATH");
    if (env != nullptr && env[0] != '\0')
    {
        return env;
    }
    return "waveform.fst";
}

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
    // Parse configuration from environment
    const uint32_t max_sim_cycles = GetMaxSimCycles();
    const bool enable_waveform = IsWaveformEnabled();
    const char* waveform_path = GetWaveformPath();

    std::cout << "[TB] C910 Simulation Started" << std::endl;
    if (max_sim_cycles == 0)
    {
        std::cout << "[TB] Max simulation cycles: unlimited (run to completion)" << std::endl;
    }
    else
    {
        std::cout << "[TB] Max simulation cycles: " << max_sim_cycles << std::endl;
    }
    std::cout << "[TB] Waveform enabled: " << (enable_waveform ? "yes" : "no") << std::endl;

    Verilated::commandArgs(argc, argv);
    auto context = std::make_unique<VerilatedContext>();
    context->traceEverOn(enable_waveform);
    context->commandArgs(argc, argv);

    auto dut = std::make_unique<Vsim_top>(context.get());

#if VM_TRACE_FST
    std::unique_ptr<VerilatedFstC> trace_fst;
    if (enable_waveform)
    {
        trace_fst = std::make_unique<VerilatedFstC>();
        dut->trace(trace_fst.get(), 99);  // Trace 99 levels of hierarchy
        trace_fst->open(waveform_path);
        std::cout << "[TB] FST waveform will be saved to: " << waveform_path << std::endl;
    }
#else
    if (enable_waveform)
    {
        std::cerr << "[TB] Warning: Waveform requested but VM_TRACE_FST is not enabled at compile time" << std::endl;
    }
#endif

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
#if VM_TRACE_FST
        if (trace_fst) trace_fst->dump(context->time());
#endif
        context->timeInc(1);

        dut->clk = 1;
        dut->eval();
#if VM_TRACE_FST
        if (trace_fst) trace_fst->dump(context->time());
#endif
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

    // Key signals for status reporting
    uint64_t core0_retire0_pc = 0;
    uint64_t core0_retire1_pc = 0;
    uint64_t core0_retire2_pc = 0;
    uint64_t pad_cpu_sys_cnt = 0;

    dut->rst_b = 1;
    dut->jrst_b = 1;
    dut->jclk = 0;

    const int jclk_threshold = (kTclkPeriod / kClkPeriod / 2) - 1;
    bool cycle_limit_reached = false;
    while (!context->gotFinish() && !cycle_limit_reached)
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

        // Check cycle limit (0 means unlimited)
        if (max_sim_cycles > 0 && cycle_count >= max_sim_cycles)
        {
            std::cout << "**********************************************" << std::endl;
            std::cout << "*   meeting max simulation cycle limit!      *" << std::endl;
            std::cout << "*   C910_SIM_MAX_CYCLE = " << max_sim_cycles << std::endl;
            std::cout << "**********************************************" << std::endl;
            cycle_limit_reached = true;
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
        }
        else if ((cycle_count % kLastCycle) == 0)
        {
            // Simplified status report - key signals only
            std::cout << "[c910-status] cycle=" << cycle_count
                      << " retire_cnt=" << retire_inst_in_period
                      << " rst_b=" << static_cast<int>(dut->rst_b)
                      << " sys_cnt=0x" << std::hex << pad_cpu_sys_cnt
                      << " retire0=" << std::dec << static_cast<int>(dut->retire0)
                      << " retire1=" << static_cast<int>(dut->retire1)
                      << " retire2=" << static_cast<int>(dut->retire2)
                      << " pc0=0x" << std::hex << core0_retire0_pc
                      << " pc1=0x" << core0_retire1_pc
                      << " pc2=0x" << core0_retire2_pc
                      << " awaddr=0x" << dut->axi_mem_addr
                      << " araddr=0x" << dut->biu_pad_araddr
                      << std::dec << std::endl;

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

        // Test completion check
        uint64_t value0 = dut->value0;
        uint64_t value1 = dut->value1;
        uint64_t value2 = dut->value2;
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

        // UART output
        uint8_t cpu_awlen = static_cast<uint8_t>(dut->axi_awlen);
        uint32_t cpu_awaddr = dut->axi_mem_addr;
        bool cpu_wvalid = (dut->biu_pad_wvalid != 0);
        uint16_t cpu_wstrb = static_cast<uint16_t>(dut->biu_pad_wstrb);

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

        pad_cpu_sys_cnt = dut->pad_cpu_sys_cnt;

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

#if VM_TRACE_FST
    if (trace_fst)
    {
        trace_fst->close();
        std::cout << "[TB] FST waveform saved to: " << waveform_path << std::endl;
    }
#endif

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
