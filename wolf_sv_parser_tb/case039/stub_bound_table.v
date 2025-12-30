module ct_vfdsu_srt_radix16_bound_table(
  bound_sel,
  digit_bound_1,
  digit_bound_2,
  digit_bound_3,
  digit_bound_4,
  digit_bound_5,
  digit_bound_6,
  digit_bound_7,
  digit_bound_8,
  digit_bound_9,
  sqrt_first_round,
  sqrt_secd_round,
  sqrt_secd_round_sign
);
input [6:0]  bound_sel;
input        sqrt_first_round;
input        sqrt_secd_round;
input        sqrt_secd_round_sign;
output [11:0] digit_bound_1;
output [11:0] digit_bound_2;
output [11:0] digit_bound_3;
output [11:0] digit_bound_4;
output [11:0] digit_bound_5;
output [11:0] digit_bound_6;
output [11:0] digit_bound_7;
output [11:0] digit_bound_8;
output [11:0] digit_bound_9;

wire [4:0] pad = {sqrt_first_round, sqrt_secd_round, sqrt_secd_round_sign, 2'b0};
assign digit_bound_1 = {pad, bound_sel};
assign digit_bound_2 = {pad, bound_sel} ^ 12'h111;
assign digit_bound_3 = {pad, bound_sel} ^ 12'h222;
assign digit_bound_4 = {pad, bound_sel} ^ 12'h333;
assign digit_bound_5 = {pad, bound_sel} ^ 12'h444;
assign digit_bound_6 = {pad, bound_sel} ^ 12'h555;
assign digit_bound_7 = {pad, bound_sel} ^ 12'h666;
assign digit_bound_8 = {pad, bound_sel} ^ 12'h777;
assign digit_bound_9 = {pad, bound_sel} ^ 12'h888;

endmodule
