// Auto-generated stubs for minimal reproduction.

module ct_ifu_icache_tag_array(
  forever_cpuclk,
  cp0_ifu_icg_en,
  ifu_icache_index,
  ifu_icache_tag_cen_b,
  ifu_icache_tag_clk_en,
  ifu_icache_tag_din,
  ifu_icache_tag_wen,
  pad_yy_icg_scan_en,
  icache_ifu_tag_dout
);
  input forever_cpuclk;
  input cp0_ifu_icg_en;
  input [15:0] ifu_icache_index;
  input ifu_icache_tag_cen_b;
  input ifu_icache_tag_clk_en;
  input [58:0] ifu_icache_tag_din;
  input [2 :0] ifu_icache_tag_wen;
  input pad_yy_icg_scan_en;
  output [58:0] icache_ifu_tag_dout;
endmodule

module ct_ifu_icache_data_array0(
  cp0_yy_clk_en,
  cp0_ifu_icg_en,
  forever_cpuclk,
  ifu_icache_data_array0_bank0_cen_b,
  ifu_icache_data_array0_bank0_clk_en,
  ifu_icache_data_array0_bank1_cen_b,
  ifu_icache_data_array0_bank1_clk_en,
  ifu_icache_data_array0_bank2_cen_b,
  ifu_icache_data_array0_bank2_clk_en,
  ifu_icache_data_array0_bank3_cen_b,
  ifu_icache_data_array0_bank3_clk_en,
  ifu_icache_data_array0_din,
  ifu_icache_data_array0_wen_b,
  ifu_icache_index,
  pad_yy_icg_scan_en,
  icache_ifu_data_array0_dout
);
  input cp0_yy_clk_en;
  input cp0_ifu_icg_en;
  input forever_cpuclk;
  input ifu_icache_data_array0_bank0_cen_b;
  input ifu_icache_data_array0_bank0_clk_en;
  input ifu_icache_data_array0_bank1_cen_b;
  input ifu_icache_data_array0_bank1_clk_en;
  input ifu_icache_data_array0_bank2_cen_b;
  input ifu_icache_data_array0_bank2_clk_en;
  input ifu_icache_data_array0_bank3_cen_b;
  input ifu_icache_data_array0_bank3_clk_en;
  input [127:0] ifu_icache_data_array0_din;
  input ifu_icache_data_array0_wen_b;
  input [15 :0] ifu_icache_index;
  input pad_yy_icg_scan_en;
  output [127:0] icache_ifu_data_array0_dout;
endmodule

module ct_ifu_icache_data_array1(
  cp0_yy_clk_en,
  cp0_ifu_icg_en,
  forever_cpuclk,
  ifu_icache_data_array1_bank0_cen_b,
  ifu_icache_data_array1_bank0_clk_en,
  ifu_icache_data_array1_bank1_cen_b,
  ifu_icache_data_array1_bank1_clk_en,
  ifu_icache_data_array1_bank2_cen_b,
  ifu_icache_data_array1_bank2_clk_en,
  ifu_icache_data_array1_bank3_cen_b,
  ifu_icache_data_array1_bank3_clk_en,
  ifu_icache_data_array1_din,
  ifu_icache_data_array1_wen_b,
  ifu_icache_index,
  pad_yy_icg_scan_en,
  icache_ifu_data_array1_dout
);
  input cp0_yy_clk_en;
  input cp0_ifu_icg_en;
  input forever_cpuclk;
  input ifu_icache_data_array1_bank0_cen_b;
  input ifu_icache_data_array1_bank0_clk_en;
  input ifu_icache_data_array1_bank1_cen_b;
  input ifu_icache_data_array1_bank1_clk_en;
  input ifu_icache_data_array1_bank2_cen_b;
  input ifu_icache_data_array1_bank2_clk_en;
  input ifu_icache_data_array1_bank3_cen_b;
  input ifu_icache_data_array1_bank3_clk_en;
  input [127:0] ifu_icache_data_array1_din;
  input ifu_icache_data_array1_wen_b;
  input [15 :0] ifu_icache_index;
  input pad_yy_icg_scan_en;
  output [127:0] icache_ifu_data_array1_dout;
endmodule

module ct_ifu_icache_predecd_array0(
  cp0_ifu_icg_en,
  cp0_yy_clk_en,
  forever_cpuclk,
  ifu_icache_data_array0_wen_b,
  ifu_icache_index,
  ifu_icache_predecd_array0_cen_b,
  ifu_icache_predecd_array0_clk_en,
  ifu_icache_predecd_array0_din,
  ifu_icache_predecd_array0_wen_b,
  pad_yy_icg_scan_en,
  icache_ifu_predecd_array0_dout
);
  input cp0_ifu_icg_en;
  input cp0_yy_clk_en;
  input forever_cpuclk;
  input ifu_icache_data_array0_wen_b;
  input [15:0] ifu_icache_index;
  input ifu_icache_predecd_array0_cen_b;
  input ifu_icache_predecd_array0_clk_en;
  input [31:0] ifu_icache_predecd_array0_din;
  input ifu_icache_predecd_array0_wen_b;
  input pad_yy_icg_scan_en;
  output [31:0] icache_ifu_predecd_array0_dout;
endmodule

module ct_ifu_icache_predecd_array1(
  cp0_ifu_icg_en,
  cp0_yy_clk_en,
  forever_cpuclk,
  ifu_icache_data_array1_wen_b,
  ifu_icache_index,
  ifu_icache_predecd_array1_cen_b,
  ifu_icache_predecd_array1_clk_en,
  ifu_icache_predecd_array1_din,
  ifu_icache_predecd_array1_wen_b,
  pad_yy_icg_scan_en,
  icache_ifu_predecd_array1_dout
);
  input cp0_ifu_icg_en;
  input cp0_yy_clk_en;
  input forever_cpuclk;
  input ifu_icache_data_array1_wen_b;
  input [15:0] ifu_icache_index;
  input ifu_icache_predecd_array1_cen_b;
  input ifu_icache_predecd_array1_clk_en;
  input [31:0] ifu_icache_predecd_array1_din;
  input ifu_icache_predecd_array1_wen_b;
  input pad_yy_icg_scan_en;
  output [31:0] icache_ifu_predecd_array1_dout;
endmodule

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
endmodule
