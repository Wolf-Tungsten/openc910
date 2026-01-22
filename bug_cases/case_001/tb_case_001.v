module tb_case_001;
  reg cp0_ifu_icache_en = 1'b0;
  reg cp0_ifu_icg_en = 1'b0;
  reg cp0_yy_clk_en = 1'b0;
  reg cpurst_b = 1'b0;
  reg forever_cpuclk = 1'b0;
  reg hpcp_ifu_cnt_en = 1'b0;
  reg [38 :0] ifctrl_icache_if_index = 0;
  reg ifctrl_icache_if_inv_fifo = 1'b0;
  reg ifctrl_icache_if_inv_on = 1'b0;
  reg ifctrl_icache_if_read_req_data0 = 1'b0;
  reg ifctrl_icache_if_read_req_data1 = 1'b0;
  reg [38 :0] ifctrl_icache_if_read_req_index = 0;
  reg ifctrl_icache_if_read_req_tag = 1'b0;
  reg ifctrl_icache_if_reset_req = 1'b0;
  reg ifctrl_icache_if_tag_req = 1'b0;
  reg [2  :0] ifctrl_icache_if_tag_wen = 0;
  reg ifu_hpcp_icache_miss_pre = 1'b0;
  reg [33 :0] ipb_icache_if_index = 0;
  reg ipb_icache_if_req = 1'b0;
  reg ipb_icache_if_req_for_gateclk = 1'b0;
  reg l1_refill_icache_if_fifo = 1'b0;
  reg l1_refill_icache_if_first = 1'b0;
  reg [38 :0] l1_refill_icache_if_index = 0;
  reg [127:0] l1_refill_icache_if_inst_data = 0;
  reg l1_refill_icache_if_last = 1'b0;
  reg [31 :0] l1_refill_icache_if_pre_code = 0;
  reg [27 :0] l1_refill_icache_if_ptag = 0;
  reg l1_refill_icache_if_wr = 1'b0;
  reg pad_yy_icg_scan_en = 1'b0;
  reg pcgen_icache_if_chgflw = 1'b0;
  reg pcgen_icache_if_chgflw_bank0 = 1'b0;
  reg pcgen_icache_if_chgflw_bank1 = 1'b0;
  reg pcgen_icache_if_chgflw_bank2 = 1'b0;
  reg pcgen_icache_if_chgflw_bank3 = 1'b0;
  reg pcgen_icache_if_chgflw_short = 1'b0;
  reg pcgen_icache_if_gateclk_en = 1'b0;
  reg [15 :0] pcgen_icache_if_index = 0;
  reg pcgen_icache_if_seq_data_req = 1'b0;
  reg pcgen_icache_if_seq_data_req_short = 1'b0;
  reg pcgen_icache_if_seq_tag_req = 1'b0;
  reg [1  :0] pcgen_icache_if_way_pred = 0;
  wire [127:0] icache_if_ifctrl_inst_data0;
  wire [127:0] icache_if_ifctrl_inst_data1;
  wire [28 :0] icache_if_ifctrl_tag_data0;
  wire [28 :0] icache_if_ifctrl_tag_data1;
  wire icache_if_ifdp_fifo;
  wire [127:0] icache_if_ifdp_inst_data0;
  wire [127:0] icache_if_ifdp_inst_data1;
  wire [31 :0] icache_if_ifdp_precode0;
  wire [31 :0] icache_if_ifdp_precode1;
  wire [28 :0] icache_if_ifdp_tag_data0;
  wire [28 :0] icache_if_ifdp_tag_data1;
  wire [28 :0] icache_if_ipb_tag_data0;
  wire [28 :0] icache_if_ipb_tag_data1;
  wire ifu_hpcp_icache_access;
  wire ifu_hpcp_icache_miss;

  ct_ifu_icache_if dut (
    .cp0_ifu_icache_en(cp0_ifu_icache_en),
    .cp0_ifu_icg_en(cp0_ifu_icg_en),
    .cp0_yy_clk_en(cp0_yy_clk_en),
    .cpurst_b(cpurst_b),
    .forever_cpuclk(forever_cpuclk),
    .hpcp_ifu_cnt_en(hpcp_ifu_cnt_en),
    .ifctrl_icache_if_index(ifctrl_icache_if_index),
    .ifctrl_icache_if_inv_fifo(ifctrl_icache_if_inv_fifo),
    .ifctrl_icache_if_inv_on(ifctrl_icache_if_inv_on),
    .ifctrl_icache_if_read_req_data0(ifctrl_icache_if_read_req_data0),
    .ifctrl_icache_if_read_req_data1(ifctrl_icache_if_read_req_data1),
    .ifctrl_icache_if_read_req_index(ifctrl_icache_if_read_req_index),
    .ifctrl_icache_if_read_req_tag(ifctrl_icache_if_read_req_tag),
    .ifctrl_icache_if_reset_req(ifctrl_icache_if_reset_req),
    .ifctrl_icache_if_tag_req(ifctrl_icache_if_tag_req),
    .ifctrl_icache_if_tag_wen(ifctrl_icache_if_tag_wen),
    .ifu_hpcp_icache_miss_pre(ifu_hpcp_icache_miss_pre),
    .ipb_icache_if_index(ipb_icache_if_index),
    .ipb_icache_if_req(ipb_icache_if_req),
    .ipb_icache_if_req_for_gateclk(ipb_icache_if_req_for_gateclk),
    .l1_refill_icache_if_fifo(l1_refill_icache_if_fifo),
    .l1_refill_icache_if_first(l1_refill_icache_if_first),
    .l1_refill_icache_if_index(l1_refill_icache_if_index),
    .l1_refill_icache_if_inst_data(l1_refill_icache_if_inst_data),
    .l1_refill_icache_if_last(l1_refill_icache_if_last),
    .l1_refill_icache_if_pre_code(l1_refill_icache_if_pre_code),
    .l1_refill_icache_if_ptag(l1_refill_icache_if_ptag),
    .l1_refill_icache_if_wr(l1_refill_icache_if_wr),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .pcgen_icache_if_chgflw(pcgen_icache_if_chgflw),
    .pcgen_icache_if_chgflw_bank0(pcgen_icache_if_chgflw_bank0),
    .pcgen_icache_if_chgflw_bank1(pcgen_icache_if_chgflw_bank1),
    .pcgen_icache_if_chgflw_bank2(pcgen_icache_if_chgflw_bank2),
    .pcgen_icache_if_chgflw_bank3(pcgen_icache_if_chgflw_bank3),
    .pcgen_icache_if_chgflw_short(pcgen_icache_if_chgflw_short),
    .pcgen_icache_if_gateclk_en(pcgen_icache_if_gateclk_en),
    .pcgen_icache_if_index(pcgen_icache_if_index),
    .pcgen_icache_if_seq_data_req(pcgen_icache_if_seq_data_req),
    .pcgen_icache_if_seq_data_req_short(pcgen_icache_if_seq_data_req_short),
    .pcgen_icache_if_seq_tag_req(pcgen_icache_if_seq_tag_req),
    .pcgen_icache_if_way_pred(pcgen_icache_if_way_pred),
    .icache_if_ifctrl_inst_data0(icache_if_ifctrl_inst_data0),
    .icache_if_ifctrl_inst_data1(icache_if_ifctrl_inst_data1),
    .icache_if_ifctrl_tag_data0(icache_if_ifctrl_tag_data0),
    .icache_if_ifctrl_tag_data1(icache_if_ifctrl_tag_data1),
    .icache_if_ifdp_fifo(icache_if_ifdp_fifo),
    .icache_if_ifdp_inst_data0(icache_if_ifdp_inst_data0),
    .icache_if_ifdp_inst_data1(icache_if_ifdp_inst_data1),
    .icache_if_ifdp_precode0(icache_if_ifdp_precode0),
    .icache_if_ifdp_precode1(icache_if_ifdp_precode1),
    .icache_if_ifdp_tag_data0(icache_if_ifdp_tag_data0),
    .icache_if_ifdp_tag_data1(icache_if_ifdp_tag_data1),
    .icache_if_ipb_tag_data0(icache_if_ipb_tag_data0),
    .icache_if_ipb_tag_data1(icache_if_ipb_tag_data1),
    .ifu_hpcp_icache_access(ifu_hpcp_icache_access),
    .ifu_hpcp_icache_miss(ifu_hpcp_icache_miss)
  );
endmodule
