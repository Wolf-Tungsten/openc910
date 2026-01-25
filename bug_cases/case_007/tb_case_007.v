module tb_case_007(
  input  I,
  input  CE,
  output O
);

  BUFGCE dut (
    .I(I),
    .CE(CE),
    .O(O)
  );
endmodule
