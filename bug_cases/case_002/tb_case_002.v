module tb_case_002;
  reg         l2c_data_clk_x = 1'b0;
  reg [511:0] l2c_data_din = 0;
  reg [12:0]  l2c_data_index0 = 0;
  reg [12:0]  l2c_data_index1 = 0;
  reg [12:0]  l2c_data_index2 = 0;
  reg [12:0]  l2c_data_index3 = 0;
  reg [3:0]   l2c_data_ram_cen = 0;
  reg [3:0]   l2c_data_wen = 0;
  reg [143:0] l2c_dirty_din = 0;
  reg         l2c_dirty_gwen = 1'b0;
  reg         l2c_dirty_ram_cen = 1'b0;
  reg [143:0] l2c_dirty_wen = 0;
  reg         l2c_tag_clk_x = 1'b0;
  reg [383:0] l2c_tag_din = 0;
  reg         l2c_tag_gwen = 1'b0;
  reg [8:0]   l2c_tag_index = 0;
  reg         l2c_tag_ram_cen = 1'b0;
  reg [383:0] l2c_tag_wen = 0;
  reg         pad_yy_icg_scan_en = 1'b0;

  wire [511:0] l2c_data_dout;
  wire [143:0] l2c_dirty_dout;
  wire [383:0] l2c_tag_dout;

  ct_l2cache_top dut (
    .l2c_data_clk_x(l2c_data_clk_x),
    .l2c_data_din(l2c_data_din),
    .l2c_data_dout(l2c_data_dout),
    .l2c_data_index0(l2c_data_index0),
    .l2c_data_index1(l2c_data_index1),
    .l2c_data_index2(l2c_data_index2),
    .l2c_data_index3(l2c_data_index3),
    .l2c_data_ram_cen(l2c_data_ram_cen),
    .l2c_data_wen(l2c_data_wen),
    .l2c_dirty_din(l2c_dirty_din),
    .l2c_dirty_dout(l2c_dirty_dout),
    .l2c_dirty_gwen(l2c_dirty_gwen),
    .l2c_dirty_ram_cen(l2c_dirty_ram_cen),
    .l2c_dirty_wen(l2c_dirty_wen),
    .l2c_tag_clk_x(l2c_tag_clk_x),
    .l2c_tag_din(l2c_tag_din),
    .l2c_tag_dout(l2c_tag_dout),
    .l2c_tag_gwen(l2c_tag_gwen),
    .l2c_tag_index(l2c_tag_index),
    .l2c_tag_ram_cen(l2c_tag_ram_cen),
    .l2c_tag_wen(l2c_tag_wen),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en)
  );
endmodule
