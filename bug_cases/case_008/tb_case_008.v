module tb_case_008(
  input         clk,
  input         rst_b,
  input  [1:0]  valid,
  input         clr,
  output [1:0]  sel
);

  ct_prio dut (
    .clk(clk),
    .rst_b(rst_b),
    .valid(valid),
    .clr(clr),
    .sel(sel)
  );
endmodule
