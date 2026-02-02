// Stub module definitions for minimal bugcase build.

module instance_reg_flog(
  clk,
  rst_b,
  en,
  data_in,
  data_out
);
parameter DATA = 32;

input             clk;
input             rst_b;
input             en;
input  [DATA-1:0] data_in;
output [DATA-1:0] data_out;

reg    [DATA-1:0] data_flop;

always @(posedge clk or negedge rst_b)
begin
  if(!rst_b)
    data_flop[DATA-1:0] <= {DATA{1'b0}};
  else if(en)
    data_flop[DATA-1:0] <= data_in[DATA-1:0];
end

assign data_out[DATA-1:0] = data_flop[DATA-1:0];

endmodule
