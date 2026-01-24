module tb_case_004(
  input  clk,
  input  rst_b,
  input  sync_in,
  output sync_out
);

  sync_level2level dut (
    .clk(clk),
    .rst_b(rst_b),
    .sync_in(sync_in),
    .sync_out(sync_out)
  );
endmodule
