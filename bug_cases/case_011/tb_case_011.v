module tb_case_011(
  input  [6:2]  paddr,
  input         pclk,
  input         pclk_intr,
  input         penable,
  input         presetn,
  input         psel,
  input  [31:0] pwdata,
  input         pwrite,
  output [7:0]  gpio_intr,
  output [31:0] prdata,
  inout  [7:0]  b_pad_gpio_porta
);

  gpio dut (
    .b_pad_gpio_porta(b_pad_gpio_porta),
    .gpio_intr(gpio_intr),
    .paddr(paddr),
    .pclk(pclk),
    .pclk_intr(pclk_intr),
    .penable(penable),
    .prdata(prdata),
    .presetn(presetn),
    .psel(psel),
    .pwdata(pwdata),
    .pwrite(pwrite)
  );
endmodule
