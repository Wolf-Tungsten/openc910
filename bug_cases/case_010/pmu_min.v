module pmu(
    input  wire corec_pmu_sleep_out,
    input  wire cpu_clk,
    input  wire pad_cpu_rst_b,
    output reg  pmu_cpu_pwr_on
);
    always @(posedge cpu_clk or negedge pad_cpu_rst_b) begin
        if (!pad_cpu_rst_b) begin
            pmu_cpu_pwr_on <= 1'b0;
        end else begin
            pmu_cpu_pwr_on <= ~pmu_cpu_pwr_on;
        end
    end
endmodule
