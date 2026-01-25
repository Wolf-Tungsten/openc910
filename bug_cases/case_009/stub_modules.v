// Stub modules for case_009 to keep dependencies minimal.

module rv_integration_platform(
  axim_clk_en,
  pad_biu_arready,
  pad_biu_awready,
  pad_biu_bid,
  pad_biu_bresp,
  pad_biu_bvalid,
  pad_biu_csysreq,
  pad_biu_rdata,
  pad_biu_rid,
  pad_biu_rlast,
  pad_biu_rresp,
  pad_biu_rvalid,
  pad_biu_wready,
  pad_core0_dbg_mask,
  pad_core0_dbgrq_b,
  pad_core0_rst_b,
  pad_core0_hartid,
  pad_cpu_apb_base,
  pad_cpu_l2cache_flush_req,
  pad_cpu_rst_b,
  pad_core0_rvba,
  pad_cpu_sys_cnt,
  pad_had_jtg_tclk,
  pad_had_jtg_tdi,
  pad_had_jtg_tms,
  pad_had_jtg_trst_b,
  pad_l2c_data_mbist_clk_ratio,
  pad_l2c_tag_mbist_clk_ratio,
  pad_plic_int_cfg,
  pad_plic_int_vld,
  pad_yy_dft_clk_rst_b,
  pad_yy_icg_scan_en,
  pad_yy_mbist_mode,
  pad_yy_scan_enable,
  pad_yy_scan_mode,
  pad_yy_scan_rst_b,
  pll_cpu_clk,
  biu_pad_araddr,
  biu_pad_arburst,
  biu_pad_arcache,
  biu_pad_arid,
  biu_pad_arlen,
  biu_pad_arlock,
  biu_pad_arprot,
  biu_pad_arsize,
  biu_pad_arvalid,
  biu_pad_awaddr,
  biu_pad_awburst,
  biu_pad_awcache,
  biu_pad_awid,
  biu_pad_awlen,
  biu_pad_awlock,
  biu_pad_awprot,
  biu_pad_awsize,
  biu_pad_awvalid,
  biu_pad_bready,
  biu_pad_cactive,
  biu_pad_csysack,
  biu_pad_rready,
  biu_pad_wdata,
  biu_pad_wlast,
  biu_pad_wstrb,
  biu_pad_wvalid,
  core0_pad_jdb_pm,
  core0_pad_lpmd_b,
  core0_pad_mstatus,
  core0_pad_retire0,
  core0_pad_retire0_pc,
  core0_pad_retire1,
  core0_pad_retire1_pc,
  core0_pad_retire2,
  core0_pad_retire2_pc,
  cpu_debug_port,
  pad_core1_dbg_mask,
  pad_core1_dbgrq_b,
  pad_core1_rst_b,
  pad_core1_hartid,
  pad_core1_rvba,
  core1_pad_jdb_pm,
  core1_pad_lpmd_b,
  core1_pad_mstatus,
  core1_pad_retire0,
  core1_pad_retire0_pc,
  core1_pad_retire1,
  core1_pad_retire1_pc,
  core1_pad_retire2,
  core1_pad_retire2_pc,
  cpu_pad_l2cache_flush_done,
  cpu_pad_no_op,
  had_pad_jtg_tdo,
  had_pad_jtg_tdo_en
);

input                  axim_clk_en;
input                  pad_biu_arready;
input                  pad_biu_awready;
input      [7 : 0]     pad_biu_bid;
input      [1 : 0]     pad_biu_bresp;
input                  pad_biu_bvalid;
input                  pad_biu_csysreq;
input      [127 : 0]   pad_biu_rdata;
input      [7 : 0]     pad_biu_rid;
input                  pad_biu_rlast;
input      [3 : 0]     pad_biu_rresp;
input                  pad_biu_rvalid;
input                  pad_biu_wready;
input                  pad_core0_dbg_mask;
input                  pad_core0_dbgrq_b;
input                  pad_core0_rst_b;
input      [2 : 0]     pad_core0_hartid;
input      [39 : 0]    pad_cpu_apb_base;
input                  pad_cpu_l2cache_flush_req;
input                  pad_cpu_rst_b;
input      [39 : 0]    pad_core0_rvba;
input      [63 : 0]    pad_cpu_sys_cnt;
input                  pad_had_jtg_tclk;
input                  pad_had_jtg_tdi;
input                  pad_had_jtg_tms;
input                  pad_had_jtg_trst_b;
input      [2 : 0]     pad_l2c_data_mbist_clk_ratio;
input      [2 : 0]     pad_l2c_tag_mbist_clk_ratio;
input      [144 - 1 : 0] pad_plic_int_cfg;
input      [144 - 1 : 0] pad_plic_int_vld;
input                  pad_yy_dft_clk_rst_b;
input                  pad_yy_icg_scan_en;
input                  pad_yy_mbist_mode;
input                  pad_yy_scan_enable;
input                  pad_yy_scan_mode;
input                  pad_yy_scan_rst_b;
input                  pll_cpu_clk;
output     [39 : 0]    biu_pad_araddr;
output     [1 : 0]     biu_pad_arburst;
output     [3 : 0]     biu_pad_arcache;
output     [7 : 0]     biu_pad_arid;
output     [7 : 0]     biu_pad_arlen;
output                 biu_pad_arlock;
output     [2 : 0]     biu_pad_arprot;
output     [2 : 0]     biu_pad_arsize;
output                 biu_pad_arvalid;
output     [39 : 0]    biu_pad_awaddr;
output     [1 : 0]     biu_pad_awburst;
output     [3 : 0]     biu_pad_awcache;
output     [7 : 0]     biu_pad_awid;
output     [7 : 0]     biu_pad_awlen;
output                 biu_pad_awlock;
output     [2 : 0]     biu_pad_awprot;
output     [2 : 0]     biu_pad_awsize;
output                 biu_pad_awvalid;
output                 biu_pad_bready;
output                 biu_pad_cactive;
output                 biu_pad_csysack;
output                 biu_pad_rready;
output     [127 : 0]   biu_pad_wdata;
output                 biu_pad_wlast;
output     [15 : 0]    biu_pad_wstrb;
output                 biu_pad_wvalid;
output     [1 : 0]     core0_pad_jdb_pm;
output     [1 : 0]     core0_pad_lpmd_b;
output     [63 : 0]    core0_pad_mstatus;
output                 core0_pad_retire0;
output     [39 : 0]    core0_pad_retire0_pc;
output                 core0_pad_retire1;
output     [39 : 0]    core0_pad_retire1_pc;
output                 core0_pad_retire2;
output     [39 : 0]    core0_pad_retire2_pc;
output                 cpu_pad_l2cache_flush_done;
output                 cpu_pad_no_op;
output                 had_pad_jtg_tdo;
output                 had_pad_jtg_tdo_en;
output                 cpu_debug_port;
input                  pad_core1_dbg_mask;
input                  pad_core1_dbgrq_b;
input                  pad_core1_rst_b;
input      [2 : 0]     pad_core1_hartid;
input      [39 : 0]    pad_core1_rvba;
output     [1 : 0]     core1_pad_jdb_pm;
output     [1 : 0]     core1_pad_lpmd_b;
output     [63 : 0]    core1_pad_mstatus;
output                 core1_pad_retire0;
output     [39 : 0]    core1_pad_retire0_pc;
output                 core1_pad_retire1;
output     [39 : 0]    core1_pad_retire1_pc;
output                 core1_pad_retire2;
output     [39 : 0]    core1_pad_retire2_pc;

assign biu_pad_araddr = 40'b0;
assign biu_pad_arburst = 2'b0;
assign biu_pad_arcache = 4'b0;
assign biu_pad_arid = 8'b0;
assign biu_pad_arlen = 8'b0;
assign biu_pad_arlock = 1'b0;
assign biu_pad_arprot = 3'b0;
assign biu_pad_arsize = 3'b0;
assign biu_pad_arvalid = 1'b0;
assign biu_pad_awaddr = 40'b0;
assign biu_pad_awburst = 2'b0;
assign biu_pad_awcache = 4'b0;
assign biu_pad_awid = 8'b0;
assign biu_pad_awlen = 8'b0;
assign biu_pad_awlock = 1'b0;
assign biu_pad_awprot = 3'b0;
assign biu_pad_awsize = 3'b0;
assign biu_pad_awvalid = 1'b0;
assign biu_pad_bready = 1'b0;
assign biu_pad_cactive = 1'b0;
assign biu_pad_csysack = 1'b0;
assign biu_pad_rready = 1'b0;
assign biu_pad_wdata = pad_plic_int_vld[127:0];
assign biu_pad_wlast = 1'b0;
assign biu_pad_wstrb = pad_plic_int_vld[143:128];
assign biu_pad_wvalid = 1'b0;
assign core0_pad_jdb_pm = 2'b0;
assign core0_pad_lpmd_b = 2'b0;
assign core0_pad_mstatus = 64'b0;
assign core0_pad_retire0 = 1'b0;
assign core0_pad_retire0_pc = 40'b0;
assign core0_pad_retire1 = 1'b0;
assign core0_pad_retire1_pc = 40'b0;
assign core0_pad_retire2 = 1'b0;
assign core0_pad_retire2_pc = 40'b0;
assign cpu_pad_l2cache_flush_done = 1'b0;
assign cpu_pad_no_op = 1'b0;
assign had_pad_jtg_tdo = 1'b0;
assign had_pad_jtg_tdo_en = 1'b0;
assign cpu_debug_port = 1'b0;
assign core1_pad_jdb_pm = 2'b0;
assign core1_pad_lpmd_b = 2'b0;
assign core1_pad_mstatus = 64'b0;
assign core1_pad_retire0 = 1'b0;
assign core1_pad_retire0_pc = 40'b0;
assign core1_pad_retire1 = 1'b0;
assign core1_pad_retire1_pc = 40'b0;
assign core1_pad_retire2 = 1'b0;
assign core1_pad_retire2_pc = 40'b0;

endmodule

module wid_for_axi4(
  input  [7 : 0] biu_pad_awid,
  input          biu_pad_awvalid,
  input          biu_pad_wvalid,
  input          biu_pad_wlast,
  input          pad_biu_awready,
  input          pad_biu_wready,
  input          pad_cpu_rst_b,
  output [7 : 0] biu_pad_wid,
  input          per_clk
);
  assign biu_pad_wid = 8'b0;
endmodule
