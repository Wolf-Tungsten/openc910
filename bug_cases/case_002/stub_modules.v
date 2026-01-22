// Empty stubs to force blackbox behavior in the elaborator.
module ct_l2cache_tag_array_16way(
  input         tag_cen,
  input         tag_clk,
  input [383:0] tag_din,
  output [383:0] tag_dout,
  input         tag_gwen,
  input [8:0]   tag_idx,
  input [383:0] tag_wen
);
endmodule

module ct_l2cache_dirty_array_16way(
  input         dirty_cen,
  input         dirty_clk,
  input [143:0] dirty_din,
  output [143:0] dirty_dout,
  input         dirty_gwen,
  input [8:0]   dirty_idx,
  input [143:0] dirty_wen
);
endmodule

module ct_l2cache_data_array(
  input         data_cen,
  input         data_clk,
  input [127:0] data_din,
  output [127:0] data_dout,
  input         data_gwen,
  input [12:0]  data_idx,
  input [127:0] data_wen,
  input         pad_yy_icg_scan_en
);
endmodule
