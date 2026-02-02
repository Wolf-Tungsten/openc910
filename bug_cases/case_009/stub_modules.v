module plic_arb_ctrl(
  plic_clk,
  arb_ctrl_int_prio,
  arb_ctrl_int_req,
  arbx_hartx_mint_req,
  arbx_hartx_sint_req,
  arbx_hreg_claim_reg_ready,
  arbx_hreg_arb_start_ack,
  ctrl_arb_int_prio,
  ctrl_arb_int_req,
  ctrl_arb_select_round,
  ctrl_arb_new_arb_start,
  hreg_arbx_arb_start,
  hreg_arbx_arb_flush,
  hreg_arbx_mint_claim,
  hreg_arbx_sint_claim,
  hreg_arbx_int_en,
  hreg_arbx_int_mmode,
  hreg_arbx_prio_sth,
  hreg_arbx_prio_mth,
  kid_yy_int_prio,
  kid_yy_int_req,
  int_sec_infor,
  ctrl_xx_core_sec,
  ctrl_xx_amp_mode,
  ciu_plic_icg_en,
  pad_yy_icg_scan_en,
  arb_clk,
  plicrst_b
);
  parameter INT_NUM  = 1024;
  parameter ECH_RD   = 32;
  parameter PRIO_BIT = 5;

  input plic_clk;
  input hreg_arbx_arb_start;
  input hreg_arbx_arb_flush;
  input hreg_arbx_mint_claim;
  input hreg_arbx_sint_claim;
  input [INT_NUM-1:0] hreg_arbx_int_en;
  input [INT_NUM-1:0] hreg_arbx_int_mmode;
  input [PRIO_BIT-1:0] hreg_arbx_prio_sth;
  input [PRIO_BIT-1:0] hreg_arbx_prio_mth;
  input [INT_NUM*PRIO_BIT-1:0] kid_yy_int_prio;
  input [INT_NUM-1:0] kid_yy_int_req;
  input [INT_NUM-1:0] int_sec_infor;
  input ctrl_xx_core_sec;
  input ctrl_xx_amp_mode;
  input plicrst_b;
  input ciu_plic_icg_en;
  input pad_yy_icg_scan_en;

  output [PRIO_BIT:0] arb_ctrl_int_prio;
  output arb_ctrl_int_req;
  output arbx_hartx_sint_req;
  output arbx_hartx_mint_req;
  output arbx_hreg_claim_reg_ready;
  output [1024*(PRIO_BIT+1)-1:0] ctrl_arb_int_prio;
  output [1023:0] ctrl_arb_int_req;
  output [4:0] ctrl_arb_select_round;
  output arbx_hreg_arb_start_ack;
  output arb_clk;
  output ctrl_arb_new_arb_start;

  assign arb_ctrl_int_prio = {PRIO_BIT+1{1'b0}};
  assign arb_ctrl_int_req = 1'b0;
  assign arbx_hartx_sint_req = 1'b0;
  assign arbx_hartx_mint_req = 1'b0;
  assign arbx_hreg_claim_reg_ready = 1'b0;
  assign ctrl_arb_int_prio = {1024*(PRIO_BIT+1){1'b0}};
  assign ctrl_arb_int_req = {1024{1'b0}};
  assign ctrl_arb_select_round = 5'b0;
  assign arbx_hreg_arb_start_ack = 1'b0;
  assign arb_clk = plic_clk;
  assign ctrl_arb_new_arb_start = 1'b0;
endmodule

module plic_32to1_arb(
  arb_clk,
  plicrst_b,
  int_in_prio,
  int_in_req,
  int_select_round,
  ctrl_arb_new_arb_start,
  int_out_req,
  int_out_id,
  int_out_prio
);
  parameter PRIO_BIT = 5;
  parameter ID_NUM   = 10;
  parameter INT_NUM  = 1024;
  parameter SEL_NUM  = 4;
  parameter ECH_RD   = 32;

  input arb_clk;
  input plicrst_b;
  input [INT_NUM-1:0] int_in_req;
  input [INT_NUM*PRIO_BIT-1:0] int_in_prio;
  input [4:0] int_select_round;
  input ctrl_arb_new_arb_start;

  output [ID_NUM-1:0] int_out_id;
  output int_out_req;
  output [PRIO_BIT-1:0] int_out_prio;

  assign int_out_id = {ID_NUM{1'b0}};
  assign int_out_req = 1'b0;
  assign int_out_prio = {PRIO_BIT{1'b0}};
endmodule
