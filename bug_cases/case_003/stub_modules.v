// Auto-generated stubs for minimal reproduction.

module gated_clk_cell(
  clk_in,
  global_en,
  module_en,
  local_en,
  external_en,
  pad_yy_icg_scan_en,
  clk_out
);
  input clk_in;
  input global_en;
  input module_en;
  input local_en;
  input external_en;
  input pad_yy_icg_scan_en;
  output clk_out;
  assign clk_out = clk_in & global_en & module_en & local_en;
endmodule
