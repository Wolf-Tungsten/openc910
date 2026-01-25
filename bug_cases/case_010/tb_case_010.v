module tb_case_010(
  input  [1:0] ctrl_receive_data_length,
  input        ctrl_receive_parity_bit,
  input        ctrl_receive_parity_en,
  input        ctrl_receive_stop_length,
  input        ctrl_trans_parity_en,
  input        receive_clk_en,
  input        rst_b,
  input        s_in,
  input        sys_clk,
  output       receive_ctrl_busy,
  output       receive_ctrl_fe,
  output       receive_ctrl_pe,
  output [7:0] receive_ctrl_rdata,
  output       receive_ctrl_redata_over
);

  uart_receive dut (
    .ctrl_receive_data_length(ctrl_receive_data_length),
    .ctrl_receive_parity_bit(ctrl_receive_parity_bit),
    .ctrl_receive_parity_en(ctrl_receive_parity_en),
    .ctrl_receive_stop_length(ctrl_receive_stop_length),
    .ctrl_trans_parity_en(ctrl_trans_parity_en),
    .receive_clk_en(receive_clk_en),
    .receive_ctrl_busy(receive_ctrl_busy),
    .receive_ctrl_fe(receive_ctrl_fe),
    .receive_ctrl_pe(receive_ctrl_pe),
    .receive_ctrl_rdata(receive_ctrl_rdata),
    .receive_ctrl_redata_over(receive_ctrl_redata_over),
    .rst_b(rst_b),
    .s_in(s_in),
    .sys_clk(sys_clk)
  );
endmodule
