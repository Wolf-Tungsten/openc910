module sim_top(
  input wire clk,
  input wire jclk,
  input wire rst_b,
  input wire jrst_b,
  input wire jtg_tdi,
  input wire jtg_tms,
  input wire uart0_sin,
  output wire axim_clk_en,
  input wire tb_init_en,
  input wire [20:0] tb_init_addr,
  input wire [127:0] tb_init_wdata,
  input wire [15:0] tb_init_wen,
  output wire jtg_tdo,
  output wire uart0_sout,
  output wire retire0,
  output wire retire1,
  output wire retire2,
  output wire [39:0] core0_retire0_pc,
  output wire [39:0] core0_retire1_pc,
  output wire [39:0] core0_retire2_pc,
  output wire [63:0] value0,
  output wire [63:0] value1,
  output wire [63:0] value2,
  output wire [3:0] axi_awlen,
  output wire [31:0] axi_mem_addr,
  output wire [39:0] biu_pad_awaddr,
  output wire biu_pad_awvalid,
  output wire [39:0] biu_pad_araddr,
  output wire biu_pad_arvalid,
  output wire [39:0] ibiu0_pad_araddr,
  output wire ibiu0_pad_arvalid,
  output wire [39:0] ibiu1_pad_araddr,
  output wire ibiu1_pad_arvalid,
  output wire [1:0] biu_pad_lpmd_b,
  output wire [63:0] pad_cpu_sys_cnt,
  output wire core0_cpu_no_retire,
  output wire cpurst_b,
  output wire core0_fifo_rst_b,
  output wire core_pad_rst_b,
  output wire biu_pad_wvalid,
  output wire [15:0] biu_pad_wstrb,
  output wire [127:0] biu_pad_wdata,
  output wire ifu_biu_rd_req,
  output wire ifu_biu_rd_req_gate,
  output wire [39:0] ifu_biu_rd_addr,
  output wire [1:0] ifu_biu_rd_len,
  output wire [2:0] ifu_biu_rd_size,
  output wire ifu_biu_rd_id,
  output wire ifu_biu_r_ready,
  output wire biu_ifu_rd_grnt,
  output wire biu_ifu_rd_data_vld,
  output wire biu_ifu_rd_last,
  output wire [1:0] biu_ifu_rd_resp,
  output wire [63:0] biu_ifu_rd_data_lo,
  output wire [63:0] biu_ifu_rd_data_hi,
  output wire [63:0] pad_biu_rdata_lo,
  output wire [63:0] pad_biu_rdata_hi,
  output wire [63:0] rdata_s0_lo,
  output wire [63:0] rdata_s0_hi,
  output wire [63:0] rdata_s1_lo,
  output wire [63:0] rdata_s1_hi,
  output wire [63:0] rdata_s2_lo,
  output wire [63:0] rdata_s2_hi,
  output wire [63:0] rdata_s3_lo,
  output wire [63:0] rdata_s3_hi,
  output wire rvalid_s0,
  output wire rvalid_s1,
  output wire rvalid_s2,
  output wire rvalid_s3,
  output wire [1:0] rresp_s0,
  output wire [1:0] rresp_s1,
  output wire [1:0] rresp_s2,
  output wire [1:0] rresp_s3,
  output wire arvalid_s0,
  output wire arvalid_s1,
  output wire arvalid_s2,
  output wire arvalid_s3,
  output wire [39:0] fifo_pad_araddr,
  output wire fifo_pad_arvalid,
  output wire [31:0] had_ifu_ir,
  output wire had_ifu_ir_vld,
  output wire [38:0] had_ifu_pc,
  output wire ifu_had_no_inst,
  output wire ifu_had_no_op,
  output wire ifu_had_reset_on,
  output wire ifu_hpcp_frontend_stall,
  output wire ifu_hpcp_icache_miss,
  output wire ifu_mmu_abort,
  output wire ifu_mmu_va_vld,
  output wire [62:0] ifu_mmu_va,
  output wire ifu_rst_b,
  output wire cp0_biu_icg_en,
  output wire [1:0] cp0_biu_lpmd_b,
  output wire biu_arvalid_int,
  output wire biu_arvalid_gate,
  output wire pad_biu_arready,
  output wire pad_biu_rvalid
);
  wire [7:0] b_pad_gpio_porta;

  soc x_soc(
    .b_pad_gpio_porta    (b_pad_gpio_porta),
    .i_pad_clk           (clk),
    .i_pad_jtg_tclk      (jclk),
    .i_pad_jtg_tdi       (jtg_tdi),
    .i_pad_jtg_tms       (jtg_tms),
    .i_pad_jtg_trst_b    (jrst_b),
    .i_pad_rst_b         (rst_b),
    .i_pad_uart0_sin     (uart0_sin),
    .axim_clk_en         (axim_clk_en),
    .tb_init_en          (tb_init_en),
    .tb_init_addr        (tb_init_addr),
    .tb_init_wdata       (tb_init_wdata),
    .tb_init_wen         (tb_init_wen),
    .o_pad_jtg_tdo       (jtg_tdo),
    .o_pad_uart0_sout    (uart0_sout)
  );

  int_mnt x_int_mnt();

  assign retire0 = x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire0;
  assign retire1 = x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire1;
  assign retire2 = x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire2;
  assign core0_retire0_pc =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire0_pc;
  assign core0_retire1_pc =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire1_pc;
  assign core0_retire2_pc =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire2_pc;

  assign value0 =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_core.x_ct_iu_top.x_ct_iu_rbus.rbus_pipe0_wb_data[63:0];
  assign value1 =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_core.x_ct_iu_top.x_ct_iu_rbus.rbus_pipe1_wb_data[63:0];
  assign value2 =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_core.x_ct_lsu_top.x_ct_lsu_ld_wb
          .ld_wb_preg_data_sign_extend[63:0];

  assign axi_awlen = x_soc.x_axi_slave128.awlen[3:0];
  assign axi_mem_addr = x_soc.x_axi_slave128.mem_addr[31:0];
  assign biu_pad_awaddr = x_soc.biu_pad_awaddr;
  assign biu_pad_awvalid = x_soc.biu_pad_awvalid;
  assign biu_pad_araddr = x_soc.biu_pad_araddr;
  assign biu_pad_arvalid = x_soc.biu_pad_arvalid;
  assign ibiu0_pad_araddr =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .ibiu0_pad_araddr;
  assign ibiu0_pad_arvalid =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .ibiu0_pad_arvalid;
  assign ibiu1_pad_araddr =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .ibiu1_pad_araddr;
  assign ibiu1_pad_arvalid =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .ibiu1_pad_arvalid;
  assign ifu_biu_rd_req =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_rd_req;
  assign ifu_biu_rd_req_gate =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_rd_req_gate;
  assign ifu_biu_rd_addr =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_rd_addr;
  assign ifu_biu_rd_len =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_rd_len;
  assign ifu_biu_rd_size =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_rd_size;
  assign ifu_biu_rd_id =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_rd_id;
  assign ifu_biu_r_ready =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.ifu_biu_r_ready;
  assign biu_ifu_rd_grnt =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.biu_ifu_rd_grnt;
  assign biu_ifu_rd_data_vld =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.biu_ifu_rd_data_vld;
  assign biu_ifu_rd_last =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.biu_ifu_rd_last;
  assign biu_ifu_rd_resp =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.biu_ifu_rd_resp;
  assign biu_ifu_rd_data_lo =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.biu_ifu_rd_data[63:0];
  assign biu_ifu_rd_data_hi =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.biu_ifu_rd_data[127:64];
  assign pad_biu_rdata_lo = x_soc.pad_biu_rdata[63:0];
  assign pad_biu_rdata_hi = x_soc.pad_biu_rdata[127:64];
  assign rdata_s0_lo = x_soc.rdata_s0[63:0];
  assign rdata_s0_hi = x_soc.rdata_s0[127:64];
  assign rdata_s1_lo = x_soc.rdata_s1[63:0];
  assign rdata_s1_hi = x_soc.rdata_s1[127:64];
  assign rdata_s2_lo = x_soc.rdata_s2[63:0];
  assign rdata_s2_hi = x_soc.rdata_s2[127:64];
  assign rdata_s3_lo = x_soc.rdata_s3[63:0];
  assign rdata_s3_hi = x_soc.rdata_s3[127:64];
  assign rvalid_s0 = x_soc.rvalid_s0;
  assign rvalid_s1 = x_soc.rvalid_s1;
  assign rvalid_s2 = x_soc.rvalid_s2;
  assign rvalid_s3 = x_soc.rvalid_s3;
  assign rresp_s0 = x_soc.rresp_s0;
  assign rresp_s1 = x_soc.rresp_s1;
  assign rresp_s2 = x_soc.rresp_s2;
  assign rresp_s3 = x_soc.rresp_s3;
  assign arvalid_s0 = x_soc.arvalid_s0;
  assign arvalid_s1 = x_soc.arvalid_s1;
  assign arvalid_s2 = x_soc.arvalid_s2;
  assign arvalid_s3 = x_soc.arvalid_s3;
  assign fifo_pad_araddr = x_soc.fifo_pad_araddr;
  assign fifo_pad_arvalid = x_soc.fifo_pad_arvalid;
  assign had_ifu_ir =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.had_ifu_ir;
  assign had_ifu_ir_vld =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.had_ifu_ir_vld;
  assign had_ifu_pc =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.had_ifu_pc;
  assign ifu_had_no_inst =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_had_no_inst;
  assign ifu_had_no_op =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_had_no_op;
  assign ifu_had_reset_on =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_had_reset_on;
  assign ifu_hpcp_frontend_stall =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_hpcp_frontend_stall;
  assign ifu_hpcp_icache_miss =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_hpcp_icache_miss;
  assign ifu_mmu_abort =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_mmu_abort;
  assign ifu_mmu_va_vld =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_mmu_va_vld;
  assign ifu_mmu_va =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_mmu_va;
  assign ifu_rst_b =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.ifu_rst_b;
  assign cp0_biu_icg_en =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.cp0_biu_icg_en;
  assign cp0_biu_lpmd_b =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.cp0_biu_lpmd_b;
  assign biu_arvalid_int =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.arvalid;
  assign biu_arvalid_gate =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .x_ct_top_0.x_ct_biu_top.arvalid_gate;
  assign pad_biu_arready = x_soc.pad_biu_arready;
  assign pad_biu_rvalid = x_soc.pad_biu_rvalid;
  assign biu_pad_lpmd_b = x_soc.biu_pad_lpmd_b;
  assign pad_cpu_sys_cnt = x_soc.x_cpu_sub_system_axi.pad_cpu_sys_cnt;
  assign core0_cpu_no_retire =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .core0_cpu_no_retire;
  assign cpurst_b =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.cpurst_b;
  assign core0_fifo_rst_b =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top
          .core0_fifo_rst_b;
  assign core_pad_rst_b =
      x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.pad_cpu_rst_b;
  assign biu_pad_wvalid = x_soc.biu_pad_wvalid;
  assign biu_pad_wstrb = x_soc.biu_pad_wstrb;
  assign biu_pad_wdata = x_soc.biu_pad_wdata;
endmodule
