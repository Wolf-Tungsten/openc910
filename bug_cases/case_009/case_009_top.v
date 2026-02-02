module case_009_top #(
  parameter INT_NUM  = 1024,
  parameter ID_NUM   = 10,
  parameter PRIO_BIT = 5
) (
  input  plic_clk,
  input  plicrst_b,
  output arbx_hartx_sint_req,
  output arbx_hartx_mint_req,
  output [ID_NUM-1:0] arbx_hreg_claim_id,
  output arbx_hreg_claim_mmode,
  output arbx_hreg_claim_reg_ready,
  output arbx_hreg_arb_start_ack
);

  wire [ID_NUM*2-1:0] claim_bus;
  wire hreg_arbx_arb_start;
  wire hreg_arbx_arb_flush;
  wire hreg_arbx_mint_claim;
  wire hreg_arbx_sint_claim;
  wire [INT_NUM-1:0] hreg_arbx_int_en;
  wire [INT_NUM-1:0] hreg_arbx_int_mmode;
  wire [PRIO_BIT-1:0] hreg_arbx_prio_sth;
  wire [PRIO_BIT-1:0] hreg_arbx_prio_mth;
  wire ctrl_xx_amp_mode;
  wire ctrl_xx_core_sec;
  wire [INT_NUM*PRIO_BIT-1:0] kid_yy_int_prio;
  wire [INT_NUM-1:0] kid_yy_int_req;
  wire [INT_NUM-1:0] int_sec_infor;
  wire ciu_plic_icg_en;
  wire pad_yy_icg_scan_en;

  assign hreg_arbx_arb_start = 1'b0;
  assign hreg_arbx_arb_flush = 1'b0;
  assign hreg_arbx_mint_claim = 1'b0;
  assign hreg_arbx_sint_claim = 1'b0;
  assign hreg_arbx_int_en = {INT_NUM{1'b0}};
  assign hreg_arbx_int_mmode = {INT_NUM{1'b0}};
  assign hreg_arbx_prio_sth = {PRIO_BIT{1'b0}};
  assign hreg_arbx_prio_mth = {PRIO_BIT{1'b0}};
  assign ctrl_xx_amp_mode = 1'b0;
  assign ctrl_xx_core_sec = 1'b0;
  assign kid_yy_int_prio = {INT_NUM*PRIO_BIT{1'b0}};
  assign kid_yy_int_req = {INT_NUM{1'b0}};
  assign int_sec_infor = {INT_NUM{1'b0}};
  assign ciu_plic_icg_en = 1'b1;
  assign pad_yy_icg_scan_en = 1'b0;

  plic_hart_arb #(
    .INT_NUM(INT_NUM),
    .ID_NUM(ID_NUM),
    .PRIO_BIT(PRIO_BIT),
    .ECH_RD(32)
  ) x_plic_hart_arb (
    .arbx_hartx_sint_req(arbx_hartx_sint_req),
    .arbx_hartx_mint_req(arbx_hartx_mint_req),
    .arbx_hreg_claim_id(claim_bus[ID_NUM+:ID_NUM]),
    .arbx_hreg_claim_mmode(arbx_hreg_claim_mmode),
    .arbx_hreg_claim_reg_ready(arbx_hreg_claim_reg_ready),
    .arbx_hreg_arb_start_ack(arbx_hreg_arb_start_ack),
    .ciu_plic_icg_en(ciu_plic_icg_en),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .hreg_arbx_arb_start(hreg_arbx_arb_start),
    .hreg_arbx_arb_flush(hreg_arbx_arb_flush),
    .hreg_arbx_mint_claim(hreg_arbx_mint_claim),
    .hreg_arbx_sint_claim(hreg_arbx_sint_claim),
    .hreg_arbx_int_en(hreg_arbx_int_en),
    .hreg_arbx_int_mmode(hreg_arbx_int_mmode),
    .hreg_arbx_prio_sth(hreg_arbx_prio_sth),
    .hreg_arbx_prio_mth(hreg_arbx_prio_mth),
    .ctrl_xx_amp_mode(ctrl_xx_amp_mode),
    .ctrl_xx_core_sec(ctrl_xx_core_sec),
    .kid_yy_int_prio(kid_yy_int_prio),
    .kid_yy_int_req(kid_yy_int_req),
    .int_sec_infor(int_sec_infor),
    .plic_clk(plic_clk),
    .plicrst_b(plicrst_b)
  );

  assign arbx_hreg_claim_id = claim_bus[ID_NUM+:ID_NUM];
endmodule
