module tb_case_002 (

);

  wire l2c_data_clk_x;
  wire [511:0] l2c_data_din;
  wire [511:0] l2c_data_dout;
  wire [12:0] l2c_data_index0;
  wire [12:0] l2c_data_index1;
  wire [12:0] l2c_data_index2;
  wire [12:0] l2c_data_index3;
  wire [3:0] l2c_data_ram_cen;
  wire [3:0] l2c_data_wen;
  wire [143:0] l2c_dirty_din;
  wire [143:0] l2c_dirty_dout;
  wire l2c_dirty_gwen;
  wire l2c_dirty_ram_cen;
  wire [143:0] l2c_dirty_wen;
  wire l2c_tag_clk_x;
  wire [383:0] l2c_tag_din;
  wire [383:0] l2c_tag_dout;
  wire l2c_tag_gwen;
  wire [8:0] l2c_tag_index;
  wire l2c_tag_ram_cen;
  wire [383:0] l2c_tag_wen;
  wire pad_yy_icg_scan_en;

  (* src = "tb_case_002.v:26.18-26.18" *)
  ct_l2cache_top dut (
    .l2c_data_clk_x(l2c_data_clk_x),
    .l2c_data_din(l2c_data_din),
    .l2c_data_index0(l2c_data_index0),
    .l2c_data_index1(l2c_data_index1),
    .l2c_data_index2(l2c_data_index2),
    .l2c_data_index3(l2c_data_index3),
    .l2c_data_ram_cen(l2c_data_ram_cen),
    .l2c_data_wen(l2c_data_wen),
    .l2c_dirty_din(l2c_dirty_din),
    .l2c_dirty_gwen(l2c_dirty_gwen),
    .l2c_dirty_ram_cen(l2c_dirty_ram_cen),
    .l2c_dirty_wen(l2c_dirty_wen),
    .l2c_tag_clk_x(l2c_tag_clk_x),
    .l2c_tag_din(l2c_tag_din),
    .l2c_tag_gwen(l2c_tag_gwen),
    .l2c_tag_index(l2c_tag_index),
    .l2c_tag_ram_cen(l2c_tag_ram_cen),
    .l2c_tag_wen(l2c_tag_wen),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .l2c_data_dout(l2c_data_dout),
    .l2c_dirty_dout(l2c_dirty_dout),
    .l2c_tag_dout(l2c_tag_dout)
  );
endmodule

module ct_l2cache_top (
  (* src = "ct_l2cache_top.v:43.18-43.18" *)
  input wire l2c_data_clk_x,
  (* src = "ct_l2cache_top.v:44.18-44.18" *)
  input wire [511:0] l2c_data_din,
  (* src = "ct_l2cache_top.v:45.18-45.18" *)
  input wire [12:0] l2c_data_index0,
  (* src = "ct_l2cache_top.v:46.18-46.18" *)
  input wire [12:0] l2c_data_index1,
  (* src = "ct_l2cache_top.v:47.18-47.18" *)
  input wire [12:0] l2c_data_index2,
  (* src = "ct_l2cache_top.v:48.18-48.18" *)
  input wire [12:0] l2c_data_index3,
  (* src = "ct_l2cache_top.v:49.18-49.18" *)
  input wire [3:0] l2c_data_ram_cen,
  (* src = "ct_l2cache_top.v:50.18-50.18" *)
  input wire [3:0] l2c_data_wen,
  (* src = "ct_l2cache_top.v:51.18-51.18" *)
  input wire [143:0] l2c_dirty_din,
  (* src = "ct_l2cache_top.v:52.18-52.18" *)
  input wire l2c_dirty_gwen,
  (* src = "ct_l2cache_top.v:53.18-53.18" *)
  input wire l2c_dirty_ram_cen,
  (* src = "ct_l2cache_top.v:54.18-54.18" *)
  input wire [143:0] l2c_dirty_wen,
  (* src = "ct_l2cache_top.v:55.18-55.18" *)
  input wire l2c_tag_clk_x,
  (* src = "ct_l2cache_top.v:56.18-56.18" *)
  input wire [383:0] l2c_tag_din,
  (* src = "ct_l2cache_top.v:57.18-57.18" *)
  input wire l2c_tag_gwen,
  (* src = "ct_l2cache_top.v:58.18-58.18" *)
  input wire [8:0] l2c_tag_index,
  (* src = "ct_l2cache_top.v:59.18-59.18" *)
  input wire l2c_tag_ram_cen,
  (* src = "ct_l2cache_top.v:60.18-60.18" *)
  input wire [383:0] l2c_tag_wen,
  (* src = "ct_l2cache_top.v:61.18-61.18" *)
  input wire pad_yy_icg_scan_en,
  (* src = "ct_l2cache_top.v:62.18-62.18" *)
  output wire [511:0] l2c_data_dout,
  (* src = "ct_l2cache_top.v:63.18-63.18" *)
  output wire [143:0] l2c_dirty_dout,
  (* src = "ct_l2cache_top.v:64.18-64.18" *)
  output wire [383:0] l2c_tag_dout
);

  wire _rhs_val_array_static_24_0;
  wire _rhs_val_array_static_26_0;
  wire _rhs_val_array_static_28_0;
  wire _rhs_val_array_static_30_0;
  wire _rhs_val_array_static_34_0;
  wire _rhs_val_array_static_39_0;
  wire _rhs_val_array_static_43_0;
  wire _rhs_val_array_static_48_0;
  wire _rhs_val_array_static_52_0;
  wire _rhs_val_array_static_57_0;
  wire _rhs_val_array_static_61_0;
  wire _rhs_val_array_static_66_0;
  wire [3:0] _rhs_val_range_slice_22_0;
  wire [127:0] _rhs_val_range_slice_36_0;
  wire [127:0] _rhs_val_range_slice_37_0;
  wire [127:0] _rhs_val_range_slice_45_0;
  wire [127:0] _rhs_val_range_slice_46_0;
  wire [127:0] _rhs_val_range_slice_54_0;
  wire [127:0] _rhs_val_range_slice_55_0;
  wire [127:0] _rhs_val_range_slice_63_0;
  wire [127:0] _rhs_val_range_slice_64_0;
  wire [127:0] _rhs_val_replicate_24_1;
  wire [127:0] _rhs_val_replicate_26_1;
  wire [127:0] _rhs_val_replicate_28_1;
  wire [127:0] _rhs_val_replicate_30_1;
  wire l2c_data_clk;
  wire [511:0] l2c_data_dout_concat_8;
  wire [3:0] l2c_data_gwen;
  wire [127:0] l2c_data_wen0;
  wire [127:0] l2c_data_wen1;
  wire [127:0] l2c_data_wen2;
  wire [127:0] l2c_data_wen3;
  wire l2c_tag_clk;
  wire [127:0] x_ct_l2cache_data_array_bank0_data_dout;
  wire [127:0] x_ct_l2cache_data_array_bank1_data_dout;
  wire [127:0] x_ct_l2cache_data_array_bank2_data_dout;
  wire [127:0] x_ct_l2cache_data_array_bank3_data_dout;

  (* src = "ct_l2cache_top.v:109.29-109.29" *)
  ct_l2cache_tag_array_16way x_ct_l2cache_tag_array (
    .tag_cen(l2c_tag_ram_cen),
    .tag_clk(l2c_tag_clk),
    .tag_din(l2c_tag_din),
    .tag_gwen(l2c_tag_gwen),
    .tag_idx(l2c_tag_index),
    .tag_wen(l2c_tag_wen),
    .tag_dout(l2c_tag_dout)
  );
  (* src = "ct_l2cache_top.v:130.31-130.31" *)
  ct_l2cache_dirty_array_16way x_ct_l2cache_dirty_array (
    .dirty_cen(l2c_dirty_ram_cen),
    .dirty_clk(l2c_tag_clk),
    .dirty_din(l2c_dirty_din),
    .dirty_gwen(l2c_dirty_gwen),
    .dirty_idx(l2c_tag_index),
    .dirty_wen(l2c_dirty_wen),
    .dirty_dout(l2c_dirty_dout)
  );
  (* src = "ct_l2cache_top.v:157.24-157.24" *)
  ct_l2cache_data_array x_ct_l2cache_data_array_bank0 (
    .data_cen(_rhs_val_array_static_34_0),
    .data_clk(l2c_data_clk),
    .data_din(_rhs_val_range_slice_36_0),
    .data_gwen(_rhs_val_array_static_39_0),
    .data_idx(l2c_data_index0),
    .data_wen(l2c_data_wen0),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .data_dout(x_ct_l2cache_data_array_bank0_data_dout)
  );
  (* src = "ct_l2cache_top.v:179.24-179.24" *)
  ct_l2cache_data_array x_ct_l2cache_data_array_bank1 (
    .data_cen(_rhs_val_array_static_43_0),
    .data_clk(l2c_data_clk),
    .data_din(_rhs_val_range_slice_45_0),
    .data_gwen(_rhs_val_array_static_48_0),
    .data_idx(l2c_data_index1),
    .data_wen(l2c_data_wen1),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .data_dout(x_ct_l2cache_data_array_bank1_data_dout)
  );
  (* src = "ct_l2cache_top.v:200.24-200.24" *)
  ct_l2cache_data_array x_ct_l2cache_data_array_bank2 (
    .data_cen(_rhs_val_array_static_52_0),
    .data_clk(l2c_data_clk),
    .data_din(_rhs_val_range_slice_54_0),
    .data_gwen(_rhs_val_array_static_57_0),
    .data_idx(l2c_data_index2),
    .data_wen(l2c_data_wen2),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .data_dout(x_ct_l2cache_data_array_bank2_data_dout)
  );
  (* src = "ct_l2cache_top.v:221.24-221.24" *)
  ct_l2cache_data_array x_ct_l2cache_data_array_bank3 (
    .data_cen(_rhs_val_array_static_61_0),
    .data_clk(l2c_data_clk),
    .data_din(_rhs_val_range_slice_63_0),
    .data_gwen(_rhs_val_array_static_66_0),
    .data_idx(l2c_data_index3),
    .data_wen(l2c_data_wen3),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .data_dout(x_ct_l2cache_data_array_bank3_data_dout)
  );

  (* src = "ct_l2cache_top.v:150.31-150.48" *)
  assign _rhs_val_range_slice_22_0 = l2c_data_wen[3:0];
  (* src = "ct_l2cache_top.v:151.36-151.51" *)
  assign _rhs_val_array_static_24_0 = l2c_data_wen[0];
  (* src = "ct_l2cache_top.v:151.31-151.53" *)
  assign _rhs_val_replicate_24_1 = {128{_rhs_val_array_static_24_0}};
  (* src = "ct_l2cache_top.v:152.36-152.51" *)
  assign _rhs_val_array_static_26_0 = l2c_data_wen[1];
  (* src = "ct_l2cache_top.v:152.31-152.53" *)
  assign _rhs_val_replicate_26_1 = {128{_rhs_val_array_static_26_0}};
  (* src = "ct_l2cache_top.v:153.36-153.51" *)
  assign _rhs_val_array_static_28_0 = l2c_data_wen[2];
  (* src = "ct_l2cache_top.v:153.31-153.53" *)
  assign _rhs_val_replicate_28_1 = {128{_rhs_val_array_static_28_0}};
  (* src = "ct_l2cache_top.v:154.36-154.51" *)
  assign _rhs_val_array_static_30_0 = l2c_data_wen[3];
  (* src = "ct_l2cache_top.v:154.31-154.53" *)
  assign _rhs_val_replicate_30_1 = {128{_rhs_val_array_static_30_0}};
  (* src = "ct_l2cache_top.v:158.26-158.45" *)
  assign _rhs_val_array_static_34_0 = l2c_data_ram_cen[0];
  (* src = "ct_l2cache_top.v:160.26-160.45" *)
  assign _rhs_val_range_slice_36_0 = l2c_data_din[127:0];
  (* src = "ct_l2cache_top.v:161.26-161.46" *)
  assign _rhs_val_range_slice_37_0 = l2c_data_dout[127:0];
  (* src = "ct_l2cache_top.v:162.26-162.42" *)
  assign _rhs_val_array_static_39_0 = l2c_data_gwen[0];
  (* src = "ct_l2cache_top.v:180.28-180.47" *)
  assign _rhs_val_array_static_43_0 = l2c_data_ram_cen[1];
  (* src = "ct_l2cache_top.v:182.28-182.49" *)
  assign _rhs_val_range_slice_45_0 = l2c_data_din[255:128];
  (* src = "ct_l2cache_top.v:183.28-183.50" *)
  assign _rhs_val_range_slice_46_0 = l2c_data_dout[255:128];
  (* src = "ct_l2cache_top.v:184.28-184.44" *)
  assign _rhs_val_array_static_48_0 = l2c_data_gwen[1];
  (* src = "ct_l2cache_top.v:201.28-201.47" *)
  assign _rhs_val_array_static_52_0 = l2c_data_ram_cen[2];
  (* src = "ct_l2cache_top.v:203.28-203.49" *)
  assign _rhs_val_range_slice_54_0 = l2c_data_din[383:256];
  (* src = "ct_l2cache_top.v:204.28-204.50" *)
  assign _rhs_val_range_slice_55_0 = l2c_data_dout[383:256];
  (* src = "ct_l2cache_top.v:205.28-205.44" *)
  assign _rhs_val_array_static_57_0 = l2c_data_gwen[2];
  (* src = "ct_l2cache_top.v:222.28-222.47" *)
  assign _rhs_val_array_static_61_0 = l2c_data_ram_cen[3];
  (* src = "ct_l2cache_top.v:224.28-224.49" *)
  assign _rhs_val_range_slice_63_0 = l2c_data_din[511:384];
  (* src = "ct_l2cache_top.v:225.28-225.50" *)
  assign _rhs_val_range_slice_64_0 = l2c_data_dout[511:384];
  (* src = "ct_l2cache_top.v:226.28-226.44" *)
  assign _rhs_val_array_static_66_0 = l2c_data_gwen[3];
  (* src = "ct_l2cache_top.v:100.8-100.19" *)
  assign l2c_tag_clk = l2c_tag_clk_x;
  (* src = "ct_l2cache_top.v:101.8-101.20" *)
  assign l2c_data_clk = l2c_data_clk_x;
  (* src = "ct_l2cache_top.v:150.8-150.26" *)
  assign l2c_data_gwen = _rhs_val_range_slice_22_0;
  (* src = "ct_l2cache_top.v:151.8-151.28" *)
  assign l2c_data_wen0 = _rhs_val_replicate_24_1;
  (* src = "ct_l2cache_top.v:152.8-152.28" *)
  assign l2c_data_wen1 = _rhs_val_replicate_26_1;
  (* src = "ct_l2cache_top.v:153.8-153.28" *)
  assign l2c_data_wen2 = _rhs_val_replicate_28_1;
  (* src = "ct_l2cache_top.v:154.8-154.28" *)
  assign l2c_data_wen3 = _rhs_val_replicate_30_1;
  (* src = "ct_l2cache_top.v:225.28-225.50" *)
  assign l2c_data_dout_concat_8 = {x_ct_l2cache_data_array_bank3_data_dout, x_ct_l2cache_data_array_bank2_data_dout, x_ct_l2cache_data_array_bank1_data_dout, x_ct_l2cache_data_array_bank0_data_dout};
  (* src = "ct_l2cache_top.v:225.28-225.50" *)
  assign l2c_data_dout = l2c_data_dout_concat_8;
endmodule

module ct_l2cache_tag_array_16way (
  (* src = "stub_modules.v:2.17-2.17" *)
  input wire tag_cen,
  (* src = "stub_modules.v:3.17-3.17" *)
  input wire tag_clk,
  (* src = "stub_modules.v:4.17-4.17" *)
  input wire [383:0] tag_din,
  (* src = "stub_modules.v:6.17-6.17" *)
  input wire tag_gwen,
  (* src = "stub_modules.v:7.17-7.17" *)
  input wire [8:0] tag_idx,
  (* src = "stub_modules.v:8.17-8.17" *)
  input wire [383:0] tag_wen,
  output wire [383:0] tag_dout
);

  wire [383:0] __constfold_tag_dout_assign_0_0;
  wire [383:0] _rhs_val_convert_4_0;

  assign tag_dout = __constfold_tag_dout_assign_0_0;

  (* src = "stub_modules.v:10.21-10.32" *)
  assign _rhs_val_convert_4_0 = 384'h0;
  assign __constfold_tag_dout_assign_0_0 = 384'h0;
endmodule

module ct_l2cache_dirty_array_16way (
  (* src = "stub_modules.v:14.17-14.17" *)
  input wire dirty_cen,
  (* src = "stub_modules.v:15.17-15.17" *)
  input wire dirty_clk,
  (* src = "stub_modules.v:16.17-16.17" *)
  input wire [143:0] dirty_din,
  (* src = "stub_modules.v:18.17-18.17" *)
  input wire dirty_gwen,
  (* src = "stub_modules.v:19.17-19.17" *)
  input wire [8:0] dirty_idx,
  (* src = "stub_modules.v:20.17-20.17" *)
  input wire [143:0] dirty_wen,
  output wire [143:0] dirty_dout
);

  wire [143:0] __constfold_dirty_dout_assign_0_0;
  wire [143:0] _rhs_val_convert_13_0;

  assign dirty_dout = __constfold_dirty_dout_assign_0_0;

  (* src = "stub_modules.v:22.23-22.34" *)
  assign _rhs_val_convert_13_0 = 144'h0;
  assign __constfold_dirty_dout_assign_0_0 = 144'h0;
endmodule

module ct_l2cache_data_array (
  (* src = "stub_modules.v:26.17-26.17" *)
  input wire data_cen,
  (* src = "stub_modules.v:27.17-27.17" *)
  input wire data_clk,
  (* src = "stub_modules.v:28.17-28.17" *)
  input wire [127:0] data_din,
  (* src = "stub_modules.v:30.17-30.17" *)
  input wire data_gwen,
  (* src = "stub_modules.v:31.17-31.17" *)
  input wire [12:0] data_idx,
  (* src = "stub_modules.v:32.17-32.17" *)
  input wire [127:0] data_wen,
  (* src = "stub_modules.v:33.17-33.17" *)
  input wire pad_yy_icg_scan_en,
  output wire [127:0] data_dout
);

  wire [127:0] __constfold_data_dout_assign_0_0;
  wire [127:0] _rhs_val_convert_32_0;

  assign data_dout = __constfold_data_dout_assign_0_0;

  (* src = "stub_modules.v:35.22-35.33" *)
  assign _rhs_val_convert_32_0 = 128'h0;
  assign __constfold_data_dout_assign_0_0 = 128'h0;
endmodule
