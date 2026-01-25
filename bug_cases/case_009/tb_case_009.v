module tb_case_009(
  input  [39 : 0]      xx_intc_vld,
  output [144 - 1 : 0] pad_plic_int_vld
);

  cpu_sub_system_axi dut (
    .xx_intc_vld(xx_intc_vld),
    .pad_plic_int_vld(pad_plic_int_vld)
  );
endmodule
