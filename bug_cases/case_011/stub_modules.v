module c910_inst_reg_mem_top(
    input  clk,
    input  rst_b,
    input  en,
    input  [31:0] data_in,
    output [31:0] data_out0
);
  wire [31:0] mem [1:0];

  instance_reg_flog #(.DATA(32)) u_reg(
    .clk      (clk),
    .rst_b    (rst_b),
    .en       (en),
    .data_in  (data_in),
    .data_out (mem[0])
  );

  assign data_out0 = mem[0];
endmodule
