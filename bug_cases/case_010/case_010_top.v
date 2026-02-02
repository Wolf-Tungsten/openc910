module case_010_top(
    input  wire cpu_clk,
    input  wire pad_cpu_rst_b,
    output wire pmu_cpu_pwr_on
);
    pmu u_pmu (
        .corec_pmu_sleep_out(),
        .cpu_clk(cpu_clk),
        .pad_cpu_rst_b(pad_cpu_rst_b),
        .pmu_cpu_pwr_on(pmu_cpu_pwr_on)
    );
endmodule
