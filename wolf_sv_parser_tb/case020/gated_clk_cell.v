/* verilator coverage_off */
module gated_clk_cell (
  input  clk_in,
  input  global_en,
  input  module_en,
  input  local_en,
  input  external_en,
  input  pad_yy_icg_scan_en,
  output clk_out
);
assign clk_out = clk_in;
endmodule
/* verilator coverage_on */
