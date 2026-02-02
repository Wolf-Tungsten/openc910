module sim_top(
  input wire clk,
  input wire rst_n,
  inout wire [7:0] b_pad_gpio_porta,
  output wire [3:0] count,
  output wire [7:0] pad_sample
);
  soc x_soc(
    .clk(clk),
    .rst_n(rst_n),
    .b_pad_gpio_porta(b_pad_gpio_porta),
    .count(count),
    .pad_sample(pad_sample)
  );
endmodule
