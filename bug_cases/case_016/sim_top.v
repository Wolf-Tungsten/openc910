/* verilator coverage_off */
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
  output wire [63:0] value0,
  output wire [63:0] value1,
  output wire [63:0] value2,
  output wire [3:0] axi_awlen,
  output wire [31:0] axi_mem_addr,
  output wire biu_pad_wvalid,
  output wire [15:0] biu_pad_wstrb,
  output wire [127:0] biu_pad_wdata
);
  wire [7:0] b_pad_gpio_porta;
/* verilator coverage_on */

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
  assign biu_pad_wvalid = x_soc.biu_pad_wvalid;
  assign biu_pad_wstrb = x_soc.biu_pad_wstrb;
  assign biu_pad_wdata = x_soc.biu_pad_wdata;
endmodule
