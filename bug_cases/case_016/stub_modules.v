module int_mnt();
endmodule

module ct_iu_rbus(
  input wire clk,
  input wire rst_b
);
  reg [63:0] rbus_pipe0_wb_data;
  reg [63:0] rbus_pipe1_wb_data;
  wire [63:0] pipe0_inc;
  wire [63:0] pipe1_inc;
  wire [63:0] pipe0_next;
  wire [63:0] pipe1_next;

  assign pipe0_inc = rbus_pipe0_wb_data + 64'h1;
  assign pipe1_inc = rbus_pipe1_wb_data + 64'h2;
  assign pipe0_next = pipe0_inc + 64'h1;
  assign pipe1_next = pipe1_inc + 64'h1;

  always @(posedge clk or negedge rst_b) begin
    if (!rst_b) begin
      rbus_pipe0_wb_data <= 64'h0;
      rbus_pipe1_wb_data <= 64'h1;
    end else begin
      rbus_pipe0_wb_data <= pipe0_next;
      rbus_pipe1_wb_data <= pipe1_next;
    end
  end
endmodule

module ct_iu_top(
  input wire clk,
  input wire rst_b
);
  ct_iu_rbus x_ct_iu_rbus(
    .clk(clk),
    .rst_b(rst_b)
  );
endmodule

module ct_lsu_ld_wb(
  input wire clk,
  input wire rst_b
);
  reg [63:0] ld_wb_preg_data_sign_extend;
  wire [63:0] lsu_inc;
  wire [63:0] lsu_next;

  assign lsu_inc = ld_wb_preg_data_sign_extend + 64'h3;
  assign lsu_next = lsu_inc + 64'h1;

  always @(posedge clk or negedge rst_b) begin
    if (!rst_b) begin
      ld_wb_preg_data_sign_extend <= 64'h0;
    end else begin
      ld_wb_preg_data_sign_extend <= lsu_next;
    end
  end
endmodule

module ct_lsu_top(
  input wire clk,
  input wire rst_b
);
  ct_lsu_ld_wb x_ct_lsu_ld_wb(
    .clk(clk),
    .rst_b(rst_b)
  );
endmodule

module ct_core(
  input wire clk,
  input wire rst_b
);
  ct_iu_top x_ct_iu_top(
    .clk(clk),
    .rst_b(rst_b)
  );
  ct_lsu_top x_ct_lsu_top(
    .clk(clk),
    .rst_b(rst_b)
  );
endmodule

module ct_top(
  input wire clk,
  input wire rst_b
);
  ct_core x_ct_core(
    .clk(clk),
    .rst_b(rst_b)
  );
endmodule

module cpu_top(
  input wire clk,
  input wire rst_b
);
  ct_top x_ct_top_0(
    .clk(clk),
    .rst_b(rst_b)
  );
  ct_top x_ct_top_1(
    .clk(clk),
    .rst_b(rst_b)
  );
endmodule

module rv_integration_platform(
  input wire clk,
  input wire rst_b
);
/* verilator coverage_off */
  reg core0_pad_retire0;
  reg core0_pad_retire1;
  reg core0_pad_retire2;
/* verilator coverage_on */

  cpu_top x_cpu_top(
    .clk(clk),
    .rst_b(rst_b)
  );

  always @(posedge clk or negedge rst_b) begin
    if (!rst_b) begin
      core0_pad_retire0 <= 1'b0;
      core0_pad_retire1 <= 1'b0;
      core0_pad_retire2 <= 1'b0;
    end else begin
      core0_pad_retire0 <= ~core0_pad_retire0;
      core0_pad_retire1 <= core0_pad_retire0 ^ core0_pad_retire1;
      core0_pad_retire2 <= core0_pad_retire1 ^ core0_pad_retire2;
    end
  end
endmodule

module cpu_sub_system_axi(
  input wire clk,
  input wire rst_b
);
  rv_integration_platform x_rv_integration_platform(
    .clk(clk),
    .rst_b(rst_b)
  );
endmodule

module axi_slave128(
  input wire clk,
  input wire rst_b
);
  reg [7:0] awlen;
  reg [31:0] mem_addr;
  wire [7:0] awlen_inc;
  wire [7:0] awlen_next;
  wire [31:0] mem_addr_inc;
  wire [31:0] mem_addr_next;

  assign awlen_inc = awlen + 8'h1;
  assign awlen_next = awlen_inc + 8'h1;
  assign mem_addr_inc = mem_addr + 32'h4;
  assign mem_addr_next = mem_addr_inc + 32'h4;

  always @(posedge clk or negedge rst_b) begin
    if (!rst_b) begin
      awlen <= 8'h0;
      mem_addr <= 32'h0;
    end else begin
      awlen <= awlen_next;
      mem_addr <= mem_addr_next;
    end
  end
endmodule

/* verilator coverage_off */
module soc(
  input wire [7:0] b_pad_gpio_porta,
  input wire i_pad_clk,
  input wire i_pad_jtg_tclk,
  input wire i_pad_jtg_tdi,
  input wire i_pad_jtg_tms,
  input wire i_pad_jtg_trst_b,
  input wire i_pad_rst_b,
  input wire i_pad_uart0_sin,
  output wire axim_clk_en,
  input wire tb_init_en,
  input wire [20:0] tb_init_addr,
  input wire [127:0] tb_init_wdata,
  input wire [15:0] tb_init_wen,
  output wire o_pad_jtg_tdo,
  output wire o_pad_uart0_sout,
  output reg biu_pad_wvalid,
  output reg [15:0] biu_pad_wstrb,
  output reg [127:0] biu_pad_wdata
);
/* verilator coverage_on */

  cpu_sub_system_axi x_cpu_sub_system_axi(
    .clk(i_pad_clk),
    .rst_b(i_pad_rst_b)
  );

  axi_slave128 x_axi_slave128(
    .clk(i_pad_clk),
    .rst_b(i_pad_rst_b)
  );

  wire axim_stage0;
  wire axim_stage1;
  wire axim_stage2;
  wire axim_stage3;
  wire jtg_stage0;
  wire jtg_stage1;
  wire jtg_stage2;
  wire uart_stage0;
  wire uart_stage1;
  wire uart_stage2;

  assign axim_stage0 = i_pad_clk;
  assign axim_stage1 = axim_stage0 ^ i_pad_jtg_tclk;
  assign axim_stage2 = axim_stage1 ^ i_pad_jtg_tms;
  assign axim_stage3 = axim_stage2 ^ i_pad_jtg_tdi;
  assign axim_clk_en = axim_stage3;

  assign jtg_stage0 = i_pad_jtg_tdi ^ i_pad_jtg_tms;
  assign jtg_stage1 = jtg_stage0 ^ i_pad_jtg_trst_b;
  assign jtg_stage2 = jtg_stage1 ^ i_pad_jtg_tclk;
  assign o_pad_jtg_tdo = jtg_stage2;

  assign uart_stage0 = i_pad_uart0_sin;
  assign uart_stage1 = uart_stage0 ^ i_pad_rst_b;
  assign uart_stage2 = uart_stage1 ^ i_pad_jtg_trst_b;
  assign o_pad_uart0_sout = uart_stage2;

  wire [127:0] wdata_stage0;
  wire [127:0] wdata_stage1;
  wire [127:0] wdata_stage2;
  wire [127:0] wdata_stage3;
  wire [127:0] wdata_stage4;
  wire [127:0] wdata_stage5;
  wire [127:0] wdata_stage6;
  wire [127:0] wdata_stage7;
  wire [127:0] wdata_stage8;
  wire [127:0] wdata_stage9;
  wire [127:0] wdata_stage10;
  wire [127:0] wdata_stage11;
  wire [127:0] wdata_stage12;
  wire [127:0] wdata_stage13;
  wire [127:0] wdata_stage14;
  wire [127:0] wdata_stage15;
  wire [127:0] wdata_stage16;
  wire [127:0] wdata_stage17;

  assign wdata_stage0 = tb_init_wdata;
  assign wdata_stage1 = {wdata_stage0[95:0], wdata_stage0[127:96]};
  assign wdata_stage2 = wdata_stage1 ^ 128'hA5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5;
  assign wdata_stage3 = {wdata_stage2[63:0], wdata_stage2[127:64]};
  assign wdata_stage4 = wdata_stage3 ^ 128'h5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A;
  assign wdata_stage5 = {wdata_stage4[31:0], wdata_stage4[127:32]};
  assign wdata_stage6 = wdata_stage5 ^ 128'h3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C;
  assign wdata_stage7 = {wdata_stage6[95:0], wdata_stage6[127:96]};
  assign wdata_stage8 = wdata_stage7 ^ 128'hC3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3;
  assign wdata_stage9 = {wdata_stage8[63:0], wdata_stage8[127:64]};
  assign wdata_stage10 = wdata_stage9 ^ 128'h0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F;
  assign wdata_stage11 = {wdata_stage10[31:0], wdata_stage10[127:32]};
  assign wdata_stage12 = wdata_stage11 ^ 128'hF0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0;
  assign wdata_stage13 = {wdata_stage12[95:0], wdata_stage12[127:96]};
  assign wdata_stage14 = wdata_stage13 ^ 128'hAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA;
  assign wdata_stage15 = {wdata_stage14[63:0], wdata_stage14[127:64]};
  assign wdata_stage16 = wdata_stage15 ^ 128'h55555555555555555555555555555555;
  assign wdata_stage17 = {wdata_stage16[31:0], wdata_stage16[127:32]};

  wire [15:0] wstrb_stage0;
  wire [15:0] wstrb_stage1;
  wire [15:0] wstrb_stage2;
  wire [15:0] wstrb_stage3;
  wire [15:0] wstrb_stage4;
  wire [15:0] wstrb_stage5;
  wire [15:0] wstrb_stage6;
  wire [15:0] wstrb_stage7;
  wire [15:0] wstrb_stage8;
  wire [15:0] wstrb_stage9;
  wire [15:0] wstrb_stage10;

  assign wstrb_stage0 = tb_init_wen;
  assign wstrb_stage1 = {wstrb_stage0[7:0], wstrb_stage0[15:8]};
  assign wstrb_stage2 = wstrb_stage1 ^ 16'hA5A5;
  assign wstrb_stage3 = {wstrb_stage2[7:0], wstrb_stage2[15:8]};
  assign wstrb_stage4 = wstrb_stage3 ^ 16'h5A5A;
  assign wstrb_stage5 = {wstrb_stage4[7:0], wstrb_stage4[15:8]};
  assign wstrb_stage6 = wstrb_stage5 ^ 16'h3C3C;
  assign wstrb_stage7 = {wstrb_stage6[7:0], wstrb_stage6[15:8]};
  assign wstrb_stage8 = wstrb_stage7 ^ 16'hA3A3;
  assign wstrb_stage9 = {wstrb_stage8[7:0], wstrb_stage8[15:8]};
  assign wstrb_stage10 = wstrb_stage9 ^ 16'h5C5C;

  wire wvalid_stage0;
  wire wvalid_stage1;
  wire wvalid_stage2;
  wire wvalid_stage3;
  wire wvalid_stage4;
  wire wvalid_stage5;
  wire wvalid_stage6;

  assign wvalid_stage0 = tb_init_en;
  assign wvalid_stage1 = wvalid_stage0 ^ tb_init_addr[0];
  assign wvalid_stage2 = wvalid_stage1 ^ tb_init_addr[1];
  assign wvalid_stage3 = wvalid_stage2 ^ tb_init_addr[2];
  assign wvalid_stage4 = wvalid_stage3 ^ tb_init_addr[3];
  assign wvalid_stage5 = wvalid_stage4 ^ tb_init_addr[4];
  assign wvalid_stage6 = wvalid_stage5 ^ tb_init_addr[5];

  always @(posedge i_pad_clk or negedge i_pad_rst_b) begin
    if (!i_pad_rst_b) begin
      biu_pad_wvalid <= 1'b0;
      biu_pad_wstrb <= 16'h0;
      biu_pad_wdata <= 128'h0;
    end else begin
      biu_pad_wvalid <= wvalid_stage6;
      biu_pad_wstrb <= wstrb_stage10;
      biu_pad_wdata <= wdata_stage17;
    end
  end

  wire unused = i_pad_jtg_tclk ^ i_pad_jtg_trst_b ^ i_pad_rst_b ^ i_pad_uart0_sin ^ tb_init_en ^
                ^tb_init_addr ^ ^tb_init_wdata ^ ^tb_init_wen ^ ^b_pad_gpio_porta;
endmodule
