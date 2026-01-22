module tb_case_001 (

);

  wire cp0_ifu_icache_en;
  wire cp0_ifu_icg_en;
  wire cp0_yy_clk_en;
  wire cpurst_b;
  wire forever_cpuclk;
  wire hpcp_ifu_cnt_en;
  wire [127:0] icache_if_ifctrl_inst_data0;
  wire [127:0] icache_if_ifctrl_inst_data1;
  wire [28:0] icache_if_ifctrl_tag_data0;
  wire [28:0] icache_if_ifctrl_tag_data1;
  wire icache_if_ifdp_fifo;
  wire [127:0] icache_if_ifdp_inst_data0;
  wire [127:0] icache_if_ifdp_inst_data1;
  wire [31:0] icache_if_ifdp_precode0;
  wire [31:0] icache_if_ifdp_precode1;
  wire [28:0] icache_if_ifdp_tag_data0;
  wire [28:0] icache_if_ifdp_tag_data1;
  wire [28:0] icache_if_ipb_tag_data0;
  wire [28:0] icache_if_ipb_tag_data1;
  wire [38:0] ifctrl_icache_if_index;
  wire ifctrl_icache_if_inv_fifo;
  wire ifctrl_icache_if_inv_on;
  wire ifctrl_icache_if_read_req_data0;
  wire ifctrl_icache_if_read_req_data1;
  wire [38:0] ifctrl_icache_if_read_req_index;
  wire ifctrl_icache_if_read_req_tag;
  wire ifctrl_icache_if_reset_req;
  wire ifctrl_icache_if_tag_req;
  wire [2:0] ifctrl_icache_if_tag_wen;
  wire ifu_hpcp_icache_access;
  wire ifu_hpcp_icache_miss;
  wire ifu_hpcp_icache_miss_pre;
  wire [33:0] ipb_icache_if_index;
  wire ipb_icache_if_req;
  wire ipb_icache_if_req_for_gateclk;
  wire l1_refill_icache_if_fifo;
  wire l1_refill_icache_if_first;
  wire [38:0] l1_refill_icache_if_index;
  wire [127:0] l1_refill_icache_if_inst_data;
  wire l1_refill_icache_if_last;
  wire [31:0] l1_refill_icache_if_pre_code;
  wire [27:0] l1_refill_icache_if_ptag;
  wire l1_refill_icache_if_wr;
  wire pad_yy_icg_scan_en;
  wire pcgen_icache_if_chgflw;
  wire pcgen_icache_if_chgflw_bank0;
  wire pcgen_icache_if_chgflw_bank1;
  wire pcgen_icache_if_chgflw_bank2;
  wire pcgen_icache_if_chgflw_bank3;
  wire pcgen_icache_if_chgflw_short;
  wire pcgen_icache_if_gateclk_en;
  wire [15:0] pcgen_icache_if_index;
  wire pcgen_icache_if_seq_data_req;
  wire pcgen_icache_if_seq_data_req_short;
  wire pcgen_icache_if_seq_tag_req;
  wire [1:0] pcgen_icache_if_way_pred;

  (* src = "tb_case_001.v:59.20-59.20" *)
  ct_ifu_icache_if$PC_WIDTH_40 dut (
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

module ct_ifu_icache_if$PC_WIDTH_40 (
  (* src = "ct_ifu_icache_if.v:77.18-77.18" *)
  input wire cp0_ifu_icache_en,
  (* src = "ct_ifu_icache_if.v:78.18-78.18" *)
  input wire cp0_ifu_icg_en,
  (* src = "ct_ifu_icache_if.v:79.18-79.18" *)
  input wire cp0_yy_clk_en,
  (* src = "ct_ifu_icache_if.v:80.18-80.18" *)
  input wire cpurst_b,
  (* src = "ct_ifu_icache_if.v:81.18-81.18" *)
  input wire forever_cpuclk,
  (* src = "ct_ifu_icache_if.v:82.18-82.18" *)
  input wire hpcp_ifu_cnt_en,
  (* src = "ct_ifu_icache_if.v:83.18-83.18" *)
  input wire [38:0] ifctrl_icache_if_index,
  (* src = "ct_ifu_icache_if.v:84.18-84.18" *)
  input wire ifctrl_icache_if_inv_fifo,
  (* src = "ct_ifu_icache_if.v:85.18-85.18" *)
  input wire ifctrl_icache_if_inv_on,
  (* src = "ct_ifu_icache_if.v:86.18-86.18" *)
  input wire ifctrl_icache_if_read_req_data0,
  (* src = "ct_ifu_icache_if.v:87.18-87.18" *)
  input wire ifctrl_icache_if_read_req_data1,
  (* src = "ct_ifu_icache_if.v:88.18-88.18" *)
  input wire [38:0] ifctrl_icache_if_read_req_index,
  (* src = "ct_ifu_icache_if.v:89.18-89.18" *)
  input wire ifctrl_icache_if_read_req_tag,
  (* src = "ct_ifu_icache_if.v:90.18-90.18" *)
  input wire ifctrl_icache_if_reset_req,
  (* src = "ct_ifu_icache_if.v:91.18-91.18" *)
  input wire ifctrl_icache_if_tag_req,
  (* src = "ct_ifu_icache_if.v:92.18-92.18" *)
  input wire [2:0] ifctrl_icache_if_tag_wen,
  (* src = "ct_ifu_icache_if.v:93.18-93.18" *)
  input wire ifu_hpcp_icache_miss_pre,
  (* src = "ct_ifu_icache_if.v:94.18-94.18" *)
  input wire [33:0] ipb_icache_if_index,
  (* src = "ct_ifu_icache_if.v:95.18-95.18" *)
  input wire ipb_icache_if_req,
  (* src = "ct_ifu_icache_if.v:96.18-96.18" *)
  input wire ipb_icache_if_req_for_gateclk,
  (* src = "ct_ifu_icache_if.v:97.18-97.18" *)
  input wire l1_refill_icache_if_fifo,
  (* src = "ct_ifu_icache_if.v:98.18-98.18" *)
  input wire l1_refill_icache_if_first,
  (* src = "ct_ifu_icache_if.v:99.18-99.18" *)
  input wire [38:0] l1_refill_icache_if_index,
  (* src = "ct_ifu_icache_if.v:100.18-100.18" *)
  input wire [127:0] l1_refill_icache_if_inst_data,
  (* src = "ct_ifu_icache_if.v:101.18-101.18" *)
  input wire l1_refill_icache_if_last,
  (* src = "ct_ifu_icache_if.v:102.18-102.18" *)
  input wire [31:0] l1_refill_icache_if_pre_code,
  (* src = "ct_ifu_icache_if.v:103.18-103.18" *)
  input wire [27:0] l1_refill_icache_if_ptag,
  (* src = "ct_ifu_icache_if.v:104.18-104.18" *)
  input wire l1_refill_icache_if_wr,
  (* src = "ct_ifu_icache_if.v:105.18-105.18" *)
  input wire pad_yy_icg_scan_en,
  (* src = "ct_ifu_icache_if.v:106.18-106.18" *)
  input wire pcgen_icache_if_chgflw,
  (* src = "ct_ifu_icache_if.v:107.18-107.18" *)
  input wire pcgen_icache_if_chgflw_bank0,
  (* src = "ct_ifu_icache_if.v:108.18-108.18" *)
  input wire pcgen_icache_if_chgflw_bank1,
  (* src = "ct_ifu_icache_if.v:109.18-109.18" *)
  input wire pcgen_icache_if_chgflw_bank2,
  (* src = "ct_ifu_icache_if.v:110.18-110.18" *)
  input wire pcgen_icache_if_chgflw_bank3,
  (* src = "ct_ifu_icache_if.v:111.18-111.18" *)
  input wire pcgen_icache_if_chgflw_short,
  (* src = "ct_ifu_icache_if.v:112.18-112.18" *)
  input wire pcgen_icache_if_gateclk_en,
  (* src = "ct_ifu_icache_if.v:113.18-113.18" *)
  input wire [15:0] pcgen_icache_if_index,
  (* src = "ct_ifu_icache_if.v:114.18-114.18" *)
  input wire pcgen_icache_if_seq_data_req,
  (* src = "ct_ifu_icache_if.v:115.18-115.18" *)
  input wire pcgen_icache_if_seq_data_req_short,
  (* src = "ct_ifu_icache_if.v:116.18-116.18" *)
  input wire pcgen_icache_if_seq_tag_req,
  (* src = "ct_ifu_icache_if.v:117.18-117.18" *)
  input wire [1:0] pcgen_icache_if_way_pred,
  (* src = "ct_ifu_icache_if.v:118.18-118.18" *)
  output wire [127:0] icache_if_ifctrl_inst_data0,
  (* src = "ct_ifu_icache_if.v:119.18-119.18" *)
  output wire [127:0] icache_if_ifctrl_inst_data1,
  (* src = "ct_ifu_icache_if.v:120.18-120.18" *)
  output wire [28:0] icache_if_ifctrl_tag_data0,
  (* src = "ct_ifu_icache_if.v:121.18-121.18" *)
  output wire [28:0] icache_if_ifctrl_tag_data1,
  (* src = "ct_ifu_icache_if.v:122.18-122.18" *)
  output wire icache_if_ifdp_fifo,
  (* src = "ct_ifu_icache_if.v:123.18-123.18" *)
  output wire [127:0] icache_if_ifdp_inst_data0,
  (* src = "ct_ifu_icache_if.v:124.18-124.18" *)
  output wire [127:0] icache_if_ifdp_inst_data1,
  (* src = "ct_ifu_icache_if.v:125.18-125.18" *)
  output wire [31:0] icache_if_ifdp_precode0,
  (* src = "ct_ifu_icache_if.v:126.18-126.18" *)
  output wire [31:0] icache_if_ifdp_precode1,
  (* src = "ct_ifu_icache_if.v:127.18-127.18" *)
  output wire [28:0] icache_if_ifdp_tag_data0,
  (* src = "ct_ifu_icache_if.v:128.18-128.18" *)
  output wire [28:0] icache_if_ifdp_tag_data1,
  (* src = "ct_ifu_icache_if.v:129.18-129.18" *)
  output wire [28:0] icache_if_ipb_tag_data0,
  (* src = "ct_ifu_icache_if.v:130.18-130.18" *)
  output wire [28:0] icache_if_ipb_tag_data1,
  (* src = "ct_ifu_icache_if.v:131.18-131.18" *)
  output wire ifu_hpcp_icache_access,
  (* src = "ct_ifu_icache_if.v:132.18-132.18" *)
  output wire ifu_hpcp_icache_miss
);

  wire _comb_ctrl_val_and_128_5;
  wire _comb_ctrl_val_and_128_7;
  wire _comb_ctrl_val_case_eq_88_1;
  wire _comb_ctrl_val_case_eq_88_3;
  wire _comb_ctrl_val_case_eq_88_5;
  wire _comb_ctrl_val_case_eq_88_7;
  wire _comb_ctrl_val_not_128_1;
  wire _comb_ctrl_val_not_128_3;
  wire _comb_ctrl_val_shadow_slice_4_1;
  wire _comb_ctrl_val_shadow_slice_4_3;
  wire [1:0] _comb_ctrl_val_shadow_slice_7_1;
  wire [1:0] _comb_ctrl_val_shadow_slice_7_3;
  wire _rhs_val_array_static_24_7;
  wire _rhs_val_array_static_26_7;
  wire _rhs_val_array_static_28_7;
  wire _rhs_val_array_static_30_7;
  wire _rhs_val_array_static_32_6;
  wire _rhs_val_array_static_34_6;
  wire _rhs_val_array_static_36_6;
  wire _rhs_val_array_static_38_6;
  wire _rhs_val_array_static_5_0;
  wire _rhs_val_array_static_64_7;
  wire _rhs_val_array_static_66_6;
  wire _rhs_val_array_static_95_0;
  wire _rhs_val_bin_0_0;
  wire _rhs_val_bin_0_1;
  wire _rhs_val_bin_0_10;
  wire _rhs_val_bin_0_12;
  wire _rhs_val_bin_0_13;
  wire _rhs_val_bin_0_15;
  wire _rhs_val_bin_0_16;
  wire _rhs_val_bin_0_18;
  wire _rhs_val_bin_0_2;
  wire _rhs_val_bin_0_20;
  wire _rhs_val_bin_0_5;
  wire _rhs_val_bin_0_8;
  wire _rhs_val_bin_0_9;
  wire _rhs_val_bin_117_0;
  wire _rhs_val_bin_117_1;
  wire _rhs_val_bin_126_0;
  wire _rhs_val_bin_129_3;
  wire _rhs_val_bin_16_0;
  wire _rhs_val_bin_24_1;
  wire _rhs_val_bin_24_10;
  wire _rhs_val_bin_24_12;
  wire _rhs_val_bin_24_14;
  wire _rhs_val_bin_24_4;
  wire _rhs_val_bin_24_6;
  wire _rhs_val_bin_24_8;
  wire _rhs_val_bin_26_1;
  wire _rhs_val_bin_26_10;
  wire _rhs_val_bin_26_12;
  wire _rhs_val_bin_26_14;
  wire _rhs_val_bin_26_4;
  wire _rhs_val_bin_26_6;
  wire _rhs_val_bin_26_8;
  wire _rhs_val_bin_28_1;
  wire _rhs_val_bin_28_10;
  wire _rhs_val_bin_28_12;
  wire _rhs_val_bin_28_14;
  wire _rhs_val_bin_28_4;
  wire _rhs_val_bin_28_6;
  wire _rhs_val_bin_28_8;
  wire _rhs_val_bin_2_0;
  wire _rhs_val_bin_2_1;
  wire _rhs_val_bin_2_2;
  wire _rhs_val_bin_2_3;
  wire _rhs_val_bin_2_4;
  wire _rhs_val_bin_30_1;
  wire _rhs_val_bin_30_10;
  wire _rhs_val_bin_30_12;
  wire _rhs_val_bin_30_14;
  wire _rhs_val_bin_30_4;
  wire _rhs_val_bin_30_6;
  wire _rhs_val_bin_30_8;
  wire _rhs_val_bin_32_0;
  wire _rhs_val_bin_32_11;
  wire _rhs_val_bin_32_13;
  wire _rhs_val_bin_32_3;
  wire _rhs_val_bin_32_5;
  wire _rhs_val_bin_32_7;
  wire _rhs_val_bin_32_9;
  wire _rhs_val_bin_34_0;
  wire _rhs_val_bin_34_11;
  wire _rhs_val_bin_34_13;
  wire _rhs_val_bin_34_3;
  wire _rhs_val_bin_34_5;
  wire _rhs_val_bin_34_7;
  wire _rhs_val_bin_34_9;
  wire _rhs_val_bin_36_0;
  wire _rhs_val_bin_36_11;
  wire _rhs_val_bin_36_13;
  wire _rhs_val_bin_36_3;
  wire _rhs_val_bin_36_5;
  wire _rhs_val_bin_36_7;
  wire _rhs_val_bin_36_9;
  wire _rhs_val_bin_38_0;
  wire _rhs_val_bin_38_11;
  wire _rhs_val_bin_38_13;
  wire _rhs_val_bin_38_3;
  wire _rhs_val_bin_38_5;
  wire _rhs_val_bin_38_7;
  wire _rhs_val_bin_38_9;
  wire _rhs_val_bin_40_1;
  wire _rhs_val_bin_40_2;
  wire _rhs_val_bin_40_3;
  wire _rhs_val_bin_40_4;
  wire _rhs_val_bin_40_5;
  wire _rhs_val_bin_40_6;
  wire _rhs_val_bin_42_1;
  wire _rhs_val_bin_42_2;
  wire _rhs_val_bin_42_3;
  wire _rhs_val_bin_42_4;
  wire _rhs_val_bin_42_5;
  wire _rhs_val_bin_42_6;
  wire _rhs_val_bin_44_1;
  wire _rhs_val_bin_44_2;
  wire _rhs_val_bin_44_3;
  wire _rhs_val_bin_44_4;
  wire _rhs_val_bin_44_5;
  wire _rhs_val_bin_44_6;
  wire _rhs_val_bin_46_1;
  wire _rhs_val_bin_46_2;
  wire _rhs_val_bin_46_3;
  wire _rhs_val_bin_46_4;
  wire _rhs_val_bin_46_5;
  wire _rhs_val_bin_46_6;
  wire _rhs_val_bin_48_0;
  wire _rhs_val_bin_48_1;
  wire _rhs_val_bin_48_2;
  wire _rhs_val_bin_48_3;
  wire _rhs_val_bin_48_4;
  wire _rhs_val_bin_48_5;
  wire _rhs_val_bin_50_0;
  wire _rhs_val_bin_50_1;
  wire _rhs_val_bin_50_2;
  wire _rhs_val_bin_50_3;
  wire _rhs_val_bin_50_4;
  wire _rhs_val_bin_50_5;
  wire _rhs_val_bin_52_0;
  wire _rhs_val_bin_52_1;
  wire _rhs_val_bin_52_2;
  wire _rhs_val_bin_52_3;
  wire _rhs_val_bin_52_4;
  wire _rhs_val_bin_52_5;
  wire _rhs_val_bin_54_0;
  wire _rhs_val_bin_54_1;
  wire _rhs_val_bin_54_2;
  wire _rhs_val_bin_54_3;
  wire _rhs_val_bin_54_4;
  wire _rhs_val_bin_54_5;
  wire _rhs_val_bin_56_1;
  wire _rhs_val_bin_56_4;
  wire _rhs_val_bin_58_0;
  wire _rhs_val_bin_58_3;
  wire _rhs_val_bin_5_1;
  wire _rhs_val_bin_64_1;
  wire _rhs_val_bin_64_10;
  wire _rhs_val_bin_64_12;
  wire _rhs_val_bin_64_4;
  wire _rhs_val_bin_64_6;
  wire _rhs_val_bin_64_8;
  wire _rhs_val_bin_66_0;
  wire _rhs_val_bin_66_11;
  wire _rhs_val_bin_66_3;
  wire _rhs_val_bin_66_5;
  wire _rhs_val_bin_66_7;
  wire _rhs_val_bin_66_9;
  wire _rhs_val_bin_68_1;
  wire _rhs_val_bin_68_2;
  wire _rhs_val_bin_68_3;
  wire _rhs_val_bin_68_4;
  wire _rhs_val_bin_68_5;
  wire _rhs_val_bin_70_0;
  wire _rhs_val_bin_70_1;
  wire _rhs_val_bin_70_2;
  wire _rhs_val_bin_70_3;
  wire _rhs_val_bin_70_4;
  wire _rhs_val_bin_72_1;
  wire _rhs_val_bin_72_4;
  wire _rhs_val_bin_74_0;
  wire _rhs_val_bin_74_3;
  wire _rhs_val_bin_82_0;
  wire _rhs_val_bin_82_1;
  wire _rhs_val_bin_82_2;
  wire _rhs_val_bin_82_3;
  wire _rhs_val_bin_82_4;
  wire _rhs_val_bin_82_5;
  wire _rhs_val_bin_84_0;
  wire _rhs_val_bin_84_1;
  wire _rhs_val_bin_86_0;
  wire _rhs_val_bin_8_1;
  wire _rhs_val_bin_8_2;
  wire [58:0] _rhs_val_concat_18_2;
  wire [3:0] _rhs_val_concat_86_1;
  wire [15:0] _rhs_val_concat_89_8;
  wire [1:0] _rhs_val_concat_8_4;
  wire _rhs_val_const_89_11;
  wire [4:0] _rhs_val_const_89_7;
  wire [1:0] _rhs_val_convert_0_7;
  wire _rhs_val_convert_123_0;
  wire _rhs_val_convert_129_1;
  wire _rhs_val_convert_129_2;
  wire [27:0] _rhs_val_convert_16_1;
  wire [1:0] _rhs_val_convert_20_0;
  wire _rhs_val_convert_5_2;
  wire _rhs_val_convert_5_3;
  wire [127:0] _rhs_val_convert_60_0;
  wire [127:0] _rhs_val_convert_62_0;
  wire [31:0] _rhs_val_convert_76_0;
  wire [31:0] _rhs_val_convert_78_0;
  wire [3:0] _rhs_val_convert_89_1;
  wire [3:0] _rhs_val_convert_89_3;
  wire [3:0] _rhs_val_convert_89_5;
  wire [3:0] _rhs_val_convert_89_9;
  wire [1:0] _rhs_val_convert_8_5;
  wire _rhs_val_lnot_0_11;
  wire _rhs_val_lnot_0_14;
  wire _rhs_val_lnot_0_17;
  wire _rhs_val_lnot_0_19;
  wire _rhs_val_lnot_0_3;
  wire _rhs_val_lnot_0_4;
  wire _rhs_val_lnot_129_0;
  wire _rhs_val_lnot_12_0;
  wire _rhs_val_lnot_24_0;
  wire _rhs_val_lnot_24_11;
  wire _rhs_val_lnot_24_13;
  wire _rhs_val_lnot_24_2;
  wire _rhs_val_lnot_24_3;
  wire _rhs_val_lnot_24_5;
  wire _rhs_val_lnot_24_9;
  wire _rhs_val_lnot_26_0;
  wire _rhs_val_lnot_26_11;
  wire _rhs_val_lnot_26_13;
  wire _rhs_val_lnot_26_2;
  wire _rhs_val_lnot_26_3;
  wire _rhs_val_lnot_26_5;
  wire _rhs_val_lnot_26_9;
  wire _rhs_val_lnot_28_0;
  wire _rhs_val_lnot_28_11;
  wire _rhs_val_lnot_28_13;
  wire _rhs_val_lnot_28_2;
  wire _rhs_val_lnot_28_3;
  wire _rhs_val_lnot_28_5;
  wire _rhs_val_lnot_28_9;
  wire _rhs_val_lnot_30_0;
  wire _rhs_val_lnot_30_11;
  wire _rhs_val_lnot_30_13;
  wire _rhs_val_lnot_30_2;
  wire _rhs_val_lnot_30_3;
  wire _rhs_val_lnot_30_5;
  wire _rhs_val_lnot_30_9;
  wire _rhs_val_lnot_32_1;
  wire _rhs_val_lnot_32_10;
  wire _rhs_val_lnot_32_12;
  wire _rhs_val_lnot_32_2;
  wire _rhs_val_lnot_32_4;
  wire _rhs_val_lnot_32_8;
  wire _rhs_val_lnot_34_1;
  wire _rhs_val_lnot_34_10;
  wire _rhs_val_lnot_34_12;
  wire _rhs_val_lnot_34_2;
  wire _rhs_val_lnot_34_4;
  wire _rhs_val_lnot_34_8;
  wire _rhs_val_lnot_36_1;
  wire _rhs_val_lnot_36_10;
  wire _rhs_val_lnot_36_12;
  wire _rhs_val_lnot_36_2;
  wire _rhs_val_lnot_36_4;
  wire _rhs_val_lnot_36_8;
  wire _rhs_val_lnot_38_1;
  wire _rhs_val_lnot_38_10;
  wire _rhs_val_lnot_38_12;
  wire _rhs_val_lnot_38_2;
  wire _rhs_val_lnot_38_4;
  wire _rhs_val_lnot_38_8;
  wire _rhs_val_lnot_40_0;
  wire _rhs_val_lnot_42_0;
  wire _rhs_val_lnot_44_0;
  wire _rhs_val_lnot_46_0;
  wire _rhs_val_lnot_56_0;
  wire _rhs_val_lnot_56_2;
  wire _rhs_val_lnot_56_3;
  wire _rhs_val_lnot_58_1;
  wire _rhs_val_lnot_58_2;
  wire _rhs_val_lnot_64_0;
  wire _rhs_val_lnot_64_11;
  wire _rhs_val_lnot_64_2;
  wire _rhs_val_lnot_64_3;
  wire _rhs_val_lnot_64_5;
  wire _rhs_val_lnot_64_9;
  wire _rhs_val_lnot_66_1;
  wire _rhs_val_lnot_66_10;
  wire _rhs_val_lnot_66_2;
  wire _rhs_val_lnot_66_4;
  wire _rhs_val_lnot_66_8;
  wire _rhs_val_lnot_68_0;
  wire _rhs_val_lnot_72_0;
  wire _rhs_val_lnot_72_2;
  wire _rhs_val_lnot_72_3;
  wire _rhs_val_lnot_74_1;
  wire _rhs_val_lnot_74_2;
  wire _rhs_val_lnot_8_3;
  wire _rhs_val_mux_12_1;
  wire [27:0] _rhs_val_mux_16_3;
  wire [1:0] _rhs_val_mux_20_2;
  wire [127:0] _rhs_val_mux_60_2;
  wire [127:0] _rhs_val_mux_62_2;
  wire [31:0] _rhs_val_mux_76_2;
  wire [31:0] _rhs_val_mux_78_2;
  wire [15:0] _rhs_val_mux_80_2;
  wire [1:0] _rhs_val_range_slice_0_6;
  wire [127:0] _rhs_val_range_slice_101_0;
  wire [127:0] _rhs_val_range_slice_103_0;
  wire [31:0] _rhs_val_range_slice_105_0;
  wire [127:0] _rhs_val_range_slice_107_0;
  wire [31:0] _rhs_val_range_slice_109_0;
  wire [127:0] _rhs_val_range_slice_111_0;
  wire [28:0] _rhs_val_range_slice_113_0;
  wire [28:0] _rhs_val_range_slice_115_0;
  wire [27:0] _rhs_val_range_slice_16_2;
  wire [27:0] _rhs_val_range_slice_18_0;
  wire [27:0] _rhs_val_range_slice_18_1;
  wire [1:0] _rhs_val_range_slice_20_1;
  wire [127:0] _rhs_val_range_slice_60_1;
  wire [127:0] _rhs_val_range_slice_62_1;
  wire [31:0] _rhs_val_range_slice_76_1;
  wire [31:0] _rhs_val_range_slice_78_1;
  wire [15:0] _rhs_val_range_slice_80_0;
  wire [15:0] _rhs_val_range_slice_80_1;
  wire [3:0] _rhs_val_range_slice_89_0;
  wire [15:0] _rhs_val_range_slice_89_10;
  wire [15:0] _rhs_val_range_slice_89_2;
  wire [15:0] _rhs_val_range_slice_89_4;
  wire [10:0] _rhs_val_range_slice_89_6;
  wire [1:0] _rhs_val_range_slice_8_0;
  wire [28:0] _rhs_val_range_slice_91_0;
  wire [28:0] _rhs_val_range_slice_93_0;
  wire [28:0] _rhs_val_range_slice_97_0;
  wire [28:0] _rhs_val_range_slice_99_0;
  wire [15:0] _rhs_val_replicate_89_12;
  wire fifo_bit;
  wire hpcp_clk;
  wire hpcp_clk_en;
  wire [127:0] icache_ifu_data_array0_dout;
  wire [127:0] icache_ifu_data_array1_dout;
  wire [31:0] icache_ifu_predecd_array0_dout;
  wire [31:0] icache_ifu_predecd_array1_dout;
  wire [58:0] icache_ifu_tag_dout;
  wire [15:0] icache_index_higher;
  wire [15:0] icache_index_higher_case_88_1;
  wire [15:0] icache_index_higher_case_88_3;
  wire [15:0] icache_index_higher_case_88_5;
  wire [15:0] icache_index_higher_case_88_7;
  wire [3:0] icache_index_sel;
  wire icache_read_req;
  wire icache_req_higher;
  wire icache_reset_inv;
  wire [1:0] icache_way_pred;
  wire ifu_hpcp_icache_access_pre;
  wire ifu_hpcp_icache_access_reg_if_128_1;
  wire ifu_hpcp_icache_access_reg_if_128_5;
  wire ifu_hpcp_icache_miss_reg_if_128_3;
  wire ifu_hpcp_icache_miss_reg_if_128_7;
  wire ifu_icache_data_array0_bank0_cen_b;
  wire ifu_icache_data_array0_bank0_clk_en;
  wire ifu_icache_data_array0_bank1_cen_b;
  wire ifu_icache_data_array0_bank1_clk_en;
  wire ifu_icache_data_array0_bank2_cen_b;
  wire ifu_icache_data_array0_bank2_clk_en;
  wire ifu_icache_data_array0_bank3_cen_b;
  wire ifu_icache_data_array0_bank3_clk_en;
  wire [127:0] ifu_icache_data_array0_din;
  wire ifu_icache_data_array0_wen_b;
  wire ifu_icache_data_array1_bank0_cen_b;
  wire ifu_icache_data_array1_bank0_clk_en;
  wire ifu_icache_data_array1_bank1_cen_b;
  wire ifu_icache_data_array1_bank1_clk_en;
  wire ifu_icache_data_array1_bank2_cen_b;
  wire ifu_icache_data_array1_bank2_clk_en;
  wire ifu_icache_data_array1_bank3_cen_b;
  wire ifu_icache_data_array1_bank3_clk_en;
  wire [127:0] ifu_icache_data_array1_din;
  wire ifu_icache_data_array1_wen_b;
  wire [15:0] ifu_icache_index;
  wire ifu_icache_predecd_array0_cen_b;
  wire ifu_icache_predecd_array0_clk_en;
  wire [31:0] ifu_icache_predecd_array0_din;
  wire ifu_icache_predecd_array0_wen_b;
  wire ifu_icache_predecd_array1_cen_b;
  wire ifu_icache_predecd_array1_clk_en;
  wire [31:0] ifu_icache_predecd_array1_din;
  wire ifu_icache_predecd_array1_wen_b;
  wire ifu_icache_tag_cen_b;
  wire ifu_icache_tag_clk_en;
  wire [58:0] ifu_icache_tag_din;
  wire [2:0] ifu_icache_tag_wen;
  wire [2:0] ifu_icache_tag_wen_concat_3;
  wire [1:0] ifu_icache_tag_wen_hold_4_1;
  wire [1:0] ifu_icache_tag_wen_hold_4_11;
  wire [1:0] ifu_icache_tag_wen_hold_4_5;
  wire ifu_icache_tag_wen_hold_7_1;
  wire ifu_icache_tag_wen_hold_7_11;
  wire ifu_icache_tag_wen_hold_7_5;
  wire [2:0] ifu_icache_tag_wen_if_4_15;
  wire [2:0] ifu_icache_tag_wen_if_4_9;
  wire [2:0] ifu_icache_tag_wen_if_7_15;
  wire [2:0] ifu_icache_tag_wen_if_7_9;
  wire [2:0] ifu_icache_tag_wen_shadow_4_13;
  wire [2:0] ifu_icache_tag_wen_shadow_4_3;
  wire [2:0] ifu_icache_tag_wen_shadow_4_7;
  wire [2:0] ifu_icache_tag_wen_shadow_7_13;
  wire [2:0] ifu_icache_tag_wen_shadow_7_3;
  wire [2:0] ifu_icache_tag_wen_shadow_7_7;
  wire tag_fifo_din;
  wire [27:0] tag_pc_din;
  wire tag_valid_din;

  reg ifu_hpcp_icache_access_reg;
  reg ifu_hpcp_icache_miss_reg;

  (* src = "ct_ifu_icache_if.v:701.17-701.17" *)
  gated_clk_cell x_hpcp_clk (
    .clk_in(forever_cpuclk),
    .global_en(cp0_yy_clk_en),
    .module_en(cp0_ifu_icg_en),
    .local_en(hpcp_clk_en),
    .external_en(_rhs_val_convert_123_0),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .clk_out(hpcp_clk)
  );
  (* src = "ct_ifu_icache_if.v:746.26-746.26" *)
  ct_ifu_icache_tag_array x_ct_ifu_icache_tag_array (
    .forever_cpuclk(forever_cpuclk),
    .cp0_ifu_icg_en(cp0_ifu_icg_en),
    .ifu_icache_index(ifu_icache_index),
    .ifu_icache_tag_cen_b(ifu_icache_tag_cen_b),
    .ifu_icache_tag_clk_en(ifu_icache_tag_clk_en),
    .ifu_icache_tag_din(ifu_icache_tag_din),
    .ifu_icache_tag_wen(ifu_icache_tag_wen),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .icache_ifu_tag_dout(icache_ifu_tag_dout)
  );
  (* src = "ct_ifu_icache_if.v:759.28-759.28" *)
  ct_ifu_icache_data_array0 x_ct_ifu_icache_data_array0 (
    .cp0_yy_clk_en(cp0_yy_clk_en),
    .cp0_ifu_icg_en(cp0_ifu_icg_en),
    .forever_cpuclk(forever_cpuclk),
    .ifu_icache_data_array0_bank0_cen_b(ifu_icache_data_array0_bank0_cen_b),
    .ifu_icache_data_array0_bank0_clk_en(ifu_icache_data_array0_bank0_clk_en),
    .ifu_icache_data_array0_bank1_cen_b(ifu_icache_data_array0_bank1_cen_b),
    .ifu_icache_data_array0_bank1_clk_en(ifu_icache_data_array0_bank1_clk_en),
    .ifu_icache_data_array0_bank2_cen_b(ifu_icache_data_array0_bank2_cen_b),
    .ifu_icache_data_array0_bank2_clk_en(ifu_icache_data_array0_bank2_clk_en),
    .ifu_icache_data_array0_bank3_cen_b(ifu_icache_data_array0_bank3_cen_b),
    .ifu_icache_data_array0_bank3_clk_en(ifu_icache_data_array0_bank3_clk_en),
    .ifu_icache_data_array0_din(ifu_icache_data_array0_din),
    .ifu_icache_data_array0_wen_b(ifu_icache_data_array0_wen_b),
    .ifu_icache_index(ifu_icache_index),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .icache_ifu_data_array0_dout(icache_ifu_data_array0_dout)
  );
  (* src = "ct_ifu_icache_if.v:779.28-779.28" *)
  ct_ifu_icache_data_array1 x_ct_ifu_icache_data_array1 (
    .cp0_yy_clk_en(cp0_yy_clk_en),
    .cp0_ifu_icg_en(cp0_ifu_icg_en),
    .forever_cpuclk(forever_cpuclk),
    .ifu_icache_data_array1_bank0_cen_b(ifu_icache_data_array1_bank0_cen_b),
    .ifu_icache_data_array1_bank0_clk_en(ifu_icache_data_array1_bank0_clk_en),
    .ifu_icache_data_array1_bank1_cen_b(ifu_icache_data_array1_bank1_cen_b),
    .ifu_icache_data_array1_bank1_clk_en(ifu_icache_data_array1_bank1_clk_en),
    .ifu_icache_data_array1_bank2_cen_b(ifu_icache_data_array1_bank2_cen_b),
    .ifu_icache_data_array1_bank2_clk_en(ifu_icache_data_array1_bank2_clk_en),
    .ifu_icache_data_array1_bank3_cen_b(ifu_icache_data_array1_bank3_cen_b),
    .ifu_icache_data_array1_bank3_clk_en(ifu_icache_data_array1_bank3_clk_en),
    .ifu_icache_data_array1_din(ifu_icache_data_array1_din),
    .ifu_icache_data_array1_wen_b(ifu_icache_data_array1_wen_b),
    .ifu_icache_index(ifu_icache_index),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .icache_ifu_data_array1_dout(icache_ifu_data_array1_dout)
  );
  (* src = "ct_ifu_icache_if.v:799.31-799.31" *)
  ct_ifu_icache_predecd_array0 x_ct_ifu_icache_predecd_array0 (
    .cp0_ifu_icg_en(cp0_ifu_icg_en),
    .cp0_yy_clk_en(cp0_yy_clk_en),
    .forever_cpuclk(forever_cpuclk),
    .ifu_icache_data_array0_wen_b(ifu_icache_data_array0_wen_b),
    .ifu_icache_index(ifu_icache_index),
    .ifu_icache_predecd_array0_cen_b(ifu_icache_predecd_array0_cen_b),
    .ifu_icache_predecd_array0_clk_en(ifu_icache_predecd_array0_clk_en),
    .ifu_icache_predecd_array0_din(ifu_icache_predecd_array0_din),
    .ifu_icache_predecd_array0_wen_b(ifu_icache_predecd_array0_wen_b),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .icache_ifu_predecd_array0_dout(icache_ifu_predecd_array0_dout)
  );
  (* src = "ct_ifu_icache_if.v:814.31-814.31" *)
  ct_ifu_icache_predecd_array1 x_ct_ifu_icache_predecd_array1 (
    .cp0_ifu_icg_en(cp0_ifu_icg_en),
    .cp0_yy_clk_en(cp0_yy_clk_en),
    .forever_cpuclk(forever_cpuclk),
    .ifu_icache_data_array1_wen_b(ifu_icache_data_array1_wen_b),
    .ifu_icache_index(ifu_icache_index),
    .ifu_icache_predecd_array1_cen_b(ifu_icache_predecd_array1_cen_b),
    .ifu_icache_predecd_array1_clk_en(ifu_icache_predecd_array1_clk_en),
    .ifu_icache_predecd_array1_din(ifu_icache_predecd_array1_din),
    .ifu_icache_predecd_array1_wen_b(ifu_icache_predecd_array1_wen_b),
    .pad_yy_icg_scan_en(pad_yy_icg_scan_en),
    .icache_ifu_predecd_array1_dout(icache_ifu_predecd_array1_dout)
  );

  (* src = "ct_ifu_icache_if.v:267.35-268.59" *)
  assign _rhs_val_bin_0_0 = l1_refill_icache_if_first || l1_refill_icache_if_last;
  (* src = "ct_ifu_icache_if.v:266.33-268.60" *)
  assign _rhs_val_bin_0_1 = l1_refill_icache_if_wr && _rhs_val_bin_0_0;
  (* src = "ct_ifu_icache_if.v:266.33-269.50" *)
  assign _rhs_val_bin_0_2 = _rhs_val_bin_0_1 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:266.31-270.33" *)
  assign _rhs_val_lnot_0_3 = !_rhs_val_bin_0_2;
  (* src = "ct_ifu_icache_if.v:271.31-272.33" *)
  assign _rhs_val_lnot_0_4 = !ifctrl_icache_if_tag_req;
  (* src = "ct_ifu_icache_if.v:266.31-272.33" *)
  assign _rhs_val_bin_0_5 = _rhs_val_lnot_0_3 && _rhs_val_lnot_0_4;
  (* src = "ct_ifu_icache_if.v:274.35-274.64" *)
  assign _rhs_val_range_slice_0_6 = pcgen_icache_if_way_pred[1:0];
  (* src = "ct_ifu_icache_if.v:274.68-274.73" *)
  assign _rhs_val_convert_0_7 = 2'h0;
  (* src = "ct_ifu_icache_if.v:274.35-274.73" *)
  assign _rhs_val_bin_0_8 = _rhs_val_range_slice_0_6 != _rhs_val_convert_0_7;
  (* src = "ct_ifu_icache_if.v:273.33-274.74" *)
  assign _rhs_val_bin_0_9 = pcgen_icache_if_chgflw && _rhs_val_bin_0_8;
  (* src = "ct_ifu_icache_if.v:273.33-275.50" *)
  assign _rhs_val_bin_0_10 = _rhs_val_bin_0_9 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:273.31-276.33" *)
  assign _rhs_val_lnot_0_11 = !_rhs_val_bin_0_10;
  (* src = "ct_ifu_icache_if.v:266.31-276.33" *)
  assign _rhs_val_bin_0_12 = _rhs_val_bin_0_5 && _rhs_val_lnot_0_11;
  (* src = "ct_ifu_icache_if.v:277.33-278.50" *)
  assign _rhs_val_bin_0_13 = pcgen_icache_if_seq_tag_req && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:277.31-279.33" *)
  assign _rhs_val_lnot_0_14 = !_rhs_val_bin_0_13;
  (* src = "ct_ifu_icache_if.v:266.31-279.33" *)
  assign _rhs_val_bin_0_15 = _rhs_val_bin_0_12 && _rhs_val_lnot_0_14;
  (* src = "ct_ifu_icache_if.v:280.33-281.50" *)
  assign _rhs_val_bin_0_16 = ipb_icache_if_req && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:280.31-282.33" *)
  assign _rhs_val_lnot_0_17 = !_rhs_val_bin_0_16;
  (* src = "ct_ifu_icache_if.v:266.31-282.33" *)
  assign _rhs_val_bin_0_18 = _rhs_val_bin_0_15 && _rhs_val_lnot_0_17;
  (* src = "ct_ifu_icache_if.v:283.31-283.61" *)
  assign _rhs_val_lnot_0_19 = !ifctrl_icache_if_read_req_tag;
  (* src = "ct_ifu_icache_if.v:266.31-283.61" *)
  assign _rhs_val_bin_0_20 = _rhs_val_bin_0_18 && _rhs_val_lnot_0_19;
  (* src = "ct_ifu_icache_if.v:285.32-286.61" *)
  assign _rhs_val_bin_2_0 = ifctrl_icache_if_tag_req || ifctrl_icache_if_read_req_tag;
  (* src = "ct_ifu_icache_if.v:289.34-290.60" *)
  assign _rhs_val_bin_2_1 = l1_refill_icache_if_wr || pcgen_icache_if_gateclk_en;
  (* src = "ct_ifu_icache_if.v:289.34-291.63" *)
  assign _rhs_val_bin_2_2 = _rhs_val_bin_2_1 || ipb_icache_if_req_for_gateclk;
  (* src = "ct_ifu_icache_if.v:287.32-292.33" *)
  assign _rhs_val_bin_2_3 = cp0_ifu_icache_en && _rhs_val_bin_2_2;
  (* src = "ct_ifu_icache_if.v:285.32-292.33" *)
  assign _rhs_val_bin_2_4 = _rhs_val_bin_2_0 || _rhs_val_bin_2_3;
  (* src = "ct_ifu_icache_if.v:311.27-311.54" *)
  assign _rhs_val_array_static_5_0 = ifctrl_icache_if_tag_wen[2];
  (* src = "ct_ifu_icache_if.v:312.9-312.59" *)
  assign _rhs_val_bin_5_1 = l1_refill_icache_if_wr && l1_refill_icache_if_last;
  (* src = "ct_ifu_icache_if.v:313.27-313.31" *)
  assign _rhs_val_convert_5_2 = 1'h0;
  (* src = "ct_ifu_icache_if.v:315.27-315.31" *)
  assign _rhs_val_convert_5_3 = 1'h1;
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_hold_4_1 = ifu_icache_tag_wen[1:0];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_shadow_4_3 = {_rhs_val_convert_5_2, ifu_icache_tag_wen_hold_4_1};
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_hold_4_5 = ifu_icache_tag_wen[1:0];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_shadow_4_7 = {_rhs_val_convert_5_3, ifu_icache_tag_wen_hold_4_5};
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_if_4_9 = _rhs_val_bin_5_1 ? ifu_icache_tag_wen_shadow_4_3 : ifu_icache_tag_wen_shadow_4_7;
  (* src = "ct_ifu_icache_if.v:305.1-305.1" *)
  assign _comb_ctrl_val_shadow_slice_4_1 = ifu_icache_tag_wen_if_4_9[2];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_hold_4_11 = ifu_icache_tag_wen[1:0];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_shadow_4_13 = {_rhs_val_array_static_5_0, ifu_icache_tag_wen_hold_4_11};
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_if_4_15 = ifctrl_icache_if_inv_on ? ifu_icache_tag_wen_shadow_4_13 : ifu_icache_tag_wen_if_4_9;
  (* src = "ct_ifu_icache_if.v:305.1-305.1" *)
  assign _comb_ctrl_val_shadow_slice_4_3 = ifu_icache_tag_wen_if_4_15[2];
  (* src = "ct_ifu_icache_if.v:328.29-328.58" *)
  assign _rhs_val_range_slice_8_0 = ifctrl_icache_if_tag_wen[1:0];
  (* src = "ct_ifu_icache_if.v:330.11-330.64" *)
  assign _rhs_val_bin_8_1 = l1_refill_icache_if_first || l1_refill_icache_if_last;
  (* src = "ct_ifu_icache_if.v:329.9-330.65" *)
  assign _rhs_val_bin_8_2 = l1_refill_icache_if_wr && _rhs_val_bin_8_1;
  (* src = "ct_ifu_icache_if.v:331.30-331.39" *)
  assign _rhs_val_lnot_8_3 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:331.29-331.50" *)
  assign _rhs_val_concat_8_4 = {_rhs_val_lnot_8_3, fifo_bit};
  (* src = "ct_ifu_icache_if.v:333.29-333.34" *)
  assign _rhs_val_convert_8_5 = 2'h3;
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_hold_7_1 = ifu_icache_tag_wen[2];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_shadow_7_3 = {ifu_icache_tag_wen_hold_7_1, _rhs_val_concat_8_4};
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_hold_7_5 = ifu_icache_tag_wen[2];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_shadow_7_7 = {ifu_icache_tag_wen_hold_7_5, _rhs_val_convert_8_5};
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_if_7_9 = _rhs_val_bin_8_2 ? ifu_icache_tag_wen_shadow_7_3 : ifu_icache_tag_wen_shadow_7_7;
  (* src = "ct_ifu_icache_if.v:320.1-320.1" *)
  assign _comb_ctrl_val_shadow_slice_7_1 = ifu_icache_tag_wen_if_7_9[1:0];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_hold_7_11 = ifu_icache_tag_wen[2];
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_shadow_7_13 = {ifu_icache_tag_wen_hold_7_11, _rhs_val_range_slice_8_0};
  (* src = "ct_ifu_icache_if.v:138.18-138.18" *)
  assign ifu_icache_tag_wen_if_7_15 = ifctrl_icache_if_inv_on ? ifu_icache_tag_wen_shadow_7_13 : ifu_icache_tag_wen_if_7_9;
  (* src = "ct_ifu_icache_if.v:320.1-320.1" *)
  assign _comb_ctrl_val_shadow_slice_7_3 = ifu_icache_tag_wen_if_7_15[1:0];
  (* src = "ct_ifu_icache_if.v:348.29-348.38" *)
  assign _rhs_val_lnot_12_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:346.27-348.38" *)
  assign _rhs_val_mux_12_1 = ifctrl_icache_if_inv_on ? ifctrl_icache_if_inv_fifo : _rhs_val_lnot_12_0;
  (* src = "ct_ifu_icache_if.v:351.28-351.80" *)
  assign _rhs_val_bin_16_0 = ifctrl_icache_if_inv_on || l1_refill_icache_if_first;
  (* src = "ct_ifu_icache_if.v:352.29-352.34" *)
  assign _rhs_val_convert_16_1 = 28'h0;
  (* src = "ct_ifu_icache_if.v:353.29-353.59" *)
  assign _rhs_val_range_slice_16_2 = l1_refill_icache_if_ptag[27:0];
  (* src = "ct_ifu_icache_if.v:351.27-353.59" *)
  assign _rhs_val_mux_16_3 = _rhs_val_bin_16_0 ? _rhs_val_convert_16_1 : _rhs_val_range_slice_16_2;
  (* src = "ct_ifu_icache_if.v:355.51-355.67" *)
  assign _rhs_val_range_slice_18_0 = tag_pc_din[27:0];
  (* src = "ct_ifu_icache_if.v:356.51-356.67" *)
  assign _rhs_val_range_slice_18_1 = tag_pc_din[27:0];
  (* src = "ct_ifu_icache_if.v:354.35-357.36" *)
  assign _rhs_val_concat_18_2 = {tag_fifo_din, tag_valid_din, _rhs_val_range_slice_18_0, tag_valid_din, _rhs_val_range_slice_18_1};
  (* src = "ct_ifu_icache_if.v:371.31-371.36" *)
  assign _rhs_val_convert_20_0 = 2'h3;
  (* src = "ct_ifu_icache_if.v:372.31-372.60" *)
  assign _rhs_val_range_slice_20_1 = pcgen_icache_if_way_pred[1:0];
  (* src = "ct_ifu_icache_if.v:370.31-372.60" *)
  assign _rhs_val_mux_20_2 = l1_refill_icache_if_wr ? _rhs_val_convert_20_0 : _rhs_val_range_slice_20_1;
  (* src = "ct_ifu_icache_if.v:375.74-375.83" *)
  assign _rhs_val_lnot_24_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:375.48-375.83" *)
  assign _rhs_val_bin_24_1 = l1_refill_icache_if_wr && _rhs_val_lnot_24_0;
  (* src = "ct_ifu_icache_if.v:375.46-376.48" *)
  assign _rhs_val_lnot_24_2 = !_rhs_val_bin_24_1;
  (* src = "ct_ifu_icache_if.v:377.46-378.48" *)
  assign _rhs_val_lnot_24_3 = !pcgen_icache_if_chgflw_bank0;
  (* src = "ct_ifu_icache_if.v:375.46-378.48" *)
  assign _rhs_val_bin_24_4 = _rhs_val_lnot_24_2 && _rhs_val_lnot_24_3;
  (* src = "ct_ifu_icache_if.v:379.46-380.48" *)
  assign _rhs_val_lnot_24_5 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:375.46-380.48" *)
  assign _rhs_val_bin_24_6 = _rhs_val_bin_24_4 && _rhs_val_lnot_24_5;
  (* src = "ct_ifu_icache_if.v:381.73-381.91" *)
  assign _rhs_val_array_static_24_7 = icache_way_pred[0];
  (* src = "ct_ifu_icache_if.v:381.52-381.91" *)
  assign _rhs_val_bin_24_8 = cp0_ifu_icache_en && _rhs_val_array_static_24_7;
  (* src = "ct_ifu_icache_if.v:381.50-381.92" *)
  assign _rhs_val_lnot_24_9 = !_rhs_val_bin_24_8;
  (* src = "ct_ifu_icache_if.v:375.46-381.92" *)
  assign _rhs_val_bin_24_10 = _rhs_val_bin_24_6 || _rhs_val_lnot_24_9;
  (* src = "ct_ifu_icache_if.v:383.45-383.77" *)
  assign _rhs_val_lnot_24_11 = !ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:374.45-383.77" *)
  assign _rhs_val_bin_24_12 = _rhs_val_bin_24_10 && _rhs_val_lnot_24_11;
  (* src = "ct_ifu_icache_if.v:384.45-384.62" *)
  assign _rhs_val_lnot_24_13 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:374.45-384.62" *)
  assign _rhs_val_bin_24_14 = _rhs_val_bin_24_12 && _rhs_val_lnot_24_13;
  (* src = "ct_ifu_icache_if.v:386.74-386.83" *)
  assign _rhs_val_lnot_26_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:386.48-386.83" *)
  assign _rhs_val_bin_26_1 = l1_refill_icache_if_wr && _rhs_val_lnot_26_0;
  (* src = "ct_ifu_icache_if.v:386.46-387.48" *)
  assign _rhs_val_lnot_26_2 = !_rhs_val_bin_26_1;
  (* src = "ct_ifu_icache_if.v:388.46-389.48" *)
  assign _rhs_val_lnot_26_3 = !pcgen_icache_if_chgflw_bank1;
  (* src = "ct_ifu_icache_if.v:386.46-389.48" *)
  assign _rhs_val_bin_26_4 = _rhs_val_lnot_26_2 && _rhs_val_lnot_26_3;
  (* src = "ct_ifu_icache_if.v:390.46-391.48" *)
  assign _rhs_val_lnot_26_5 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:386.46-391.48" *)
  assign _rhs_val_bin_26_6 = _rhs_val_bin_26_4 && _rhs_val_lnot_26_5;
  (* src = "ct_ifu_icache_if.v:392.73-392.91" *)
  assign _rhs_val_array_static_26_7 = icache_way_pred[0];
  (* src = "ct_ifu_icache_if.v:392.52-392.91" *)
  assign _rhs_val_bin_26_8 = cp0_ifu_icache_en && _rhs_val_array_static_26_7;
  (* src = "ct_ifu_icache_if.v:392.50-392.92" *)
  assign _rhs_val_lnot_26_9 = !_rhs_val_bin_26_8;
  (* src = "ct_ifu_icache_if.v:386.46-392.92" *)
  assign _rhs_val_bin_26_10 = _rhs_val_bin_26_6 || _rhs_val_lnot_26_9;
  (* src = "ct_ifu_icache_if.v:394.45-394.77" *)
  assign _rhs_val_lnot_26_11 = !ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:385.45-394.77" *)
  assign _rhs_val_bin_26_12 = _rhs_val_bin_26_10 && _rhs_val_lnot_26_11;
  (* src = "ct_ifu_icache_if.v:395.45-395.62" *)
  assign _rhs_val_lnot_26_13 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:385.45-395.62" *)
  assign _rhs_val_bin_26_14 = _rhs_val_bin_26_12 && _rhs_val_lnot_26_13;
  (* src = "ct_ifu_icache_if.v:397.74-397.83" *)
  assign _rhs_val_lnot_28_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:397.48-397.83" *)
  assign _rhs_val_bin_28_1 = l1_refill_icache_if_wr && _rhs_val_lnot_28_0;
  (* src = "ct_ifu_icache_if.v:397.46-398.48" *)
  assign _rhs_val_lnot_28_2 = !_rhs_val_bin_28_1;
  (* src = "ct_ifu_icache_if.v:399.46-400.48" *)
  assign _rhs_val_lnot_28_3 = !pcgen_icache_if_chgflw_bank2;
  (* src = "ct_ifu_icache_if.v:397.46-400.48" *)
  assign _rhs_val_bin_28_4 = _rhs_val_lnot_28_2 && _rhs_val_lnot_28_3;
  (* src = "ct_ifu_icache_if.v:401.46-402.48" *)
  assign _rhs_val_lnot_28_5 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:397.46-402.48" *)
  assign _rhs_val_bin_28_6 = _rhs_val_bin_28_4 && _rhs_val_lnot_28_5;
  (* src = "ct_ifu_icache_if.v:403.73-403.91" *)
  assign _rhs_val_array_static_28_7 = icache_way_pred[0];
  (* src = "ct_ifu_icache_if.v:403.52-403.91" *)
  assign _rhs_val_bin_28_8 = cp0_ifu_icache_en && _rhs_val_array_static_28_7;
  (* src = "ct_ifu_icache_if.v:403.50-403.92" *)
  assign _rhs_val_lnot_28_9 = !_rhs_val_bin_28_8;
  (* src = "ct_ifu_icache_if.v:397.46-403.92" *)
  assign _rhs_val_bin_28_10 = _rhs_val_bin_28_6 || _rhs_val_lnot_28_9;
  (* src = "ct_ifu_icache_if.v:405.45-405.77" *)
  assign _rhs_val_lnot_28_11 = !ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:396.45-405.77" *)
  assign _rhs_val_bin_28_12 = _rhs_val_bin_28_10 && _rhs_val_lnot_28_11;
  (* src = "ct_ifu_icache_if.v:406.45-406.62" *)
  assign _rhs_val_lnot_28_13 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:396.45-406.62" *)
  assign _rhs_val_bin_28_14 = _rhs_val_bin_28_12 && _rhs_val_lnot_28_13;
  (* src = "ct_ifu_icache_if.v:408.74-408.83" *)
  assign _rhs_val_lnot_30_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:408.48-408.83" *)
  assign _rhs_val_bin_30_1 = l1_refill_icache_if_wr && _rhs_val_lnot_30_0;
  (* src = "ct_ifu_icache_if.v:408.46-409.48" *)
  assign _rhs_val_lnot_30_2 = !_rhs_val_bin_30_1;
  (* src = "ct_ifu_icache_if.v:410.46-411.48" *)
  assign _rhs_val_lnot_30_3 = !pcgen_icache_if_chgflw_bank3;
  (* src = "ct_ifu_icache_if.v:408.46-411.48" *)
  assign _rhs_val_bin_30_4 = _rhs_val_lnot_30_2 && _rhs_val_lnot_30_3;
  (* src = "ct_ifu_icache_if.v:412.46-413.48" *)
  assign _rhs_val_lnot_30_5 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:408.46-413.48" *)
  assign _rhs_val_bin_30_6 = _rhs_val_bin_30_4 && _rhs_val_lnot_30_5;
  (* src = "ct_ifu_icache_if.v:414.73-414.91" *)
  assign _rhs_val_array_static_30_7 = icache_way_pred[0];
  (* src = "ct_ifu_icache_if.v:414.52-414.91" *)
  assign _rhs_val_bin_30_8 = cp0_ifu_icache_en && _rhs_val_array_static_30_7;
  (* src = "ct_ifu_icache_if.v:414.50-414.92" *)
  assign _rhs_val_lnot_30_9 = !_rhs_val_bin_30_8;
  (* src = "ct_ifu_icache_if.v:408.46-414.92" *)
  assign _rhs_val_bin_30_10 = _rhs_val_bin_30_6 || _rhs_val_lnot_30_9;
  (* src = "ct_ifu_icache_if.v:416.45-416.77" *)
  assign _rhs_val_lnot_30_11 = !ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:407.45-416.77" *)
  assign _rhs_val_bin_30_12 = _rhs_val_bin_30_10 && _rhs_val_lnot_30_11;
  (* src = "ct_ifu_icache_if.v:417.45-417.62" *)
  assign _rhs_val_lnot_30_13 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:407.45-417.62" *)
  assign _rhs_val_bin_30_14 = _rhs_val_bin_30_12 && _rhs_val_lnot_30_13;
  (* src = "ct_ifu_icache_if.v:420.48-420.82" *)
  assign _rhs_val_bin_32_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:420.46-421.48" *)
  assign _rhs_val_lnot_32_1 = !_rhs_val_bin_32_0;
  (* src = "ct_ifu_icache_if.v:422.46-423.48" *)
  assign _rhs_val_lnot_32_2 = !pcgen_icache_if_chgflw_bank0;
  (* src = "ct_ifu_icache_if.v:420.46-423.48" *)
  assign _rhs_val_bin_32_3 = _rhs_val_lnot_32_1 && _rhs_val_lnot_32_2;
  (* src = "ct_ifu_icache_if.v:424.46-425.48" *)
  assign _rhs_val_lnot_32_4 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:420.46-425.48" *)
  assign _rhs_val_bin_32_5 = _rhs_val_bin_32_3 && _rhs_val_lnot_32_4;
  (* src = "ct_ifu_icache_if.v:426.73-426.91" *)
  assign _rhs_val_array_static_32_6 = icache_way_pred[1];
  (* src = "ct_ifu_icache_if.v:426.52-426.91" *)
  assign _rhs_val_bin_32_7 = cp0_ifu_icache_en && _rhs_val_array_static_32_6;
  (* src = "ct_ifu_icache_if.v:426.50-426.92" *)
  assign _rhs_val_lnot_32_8 = !_rhs_val_bin_32_7;
  (* src = "ct_ifu_icache_if.v:420.46-426.92" *)
  assign _rhs_val_bin_32_9 = _rhs_val_bin_32_5 || _rhs_val_lnot_32_8;
  (* src = "ct_ifu_icache_if.v:428.45-428.77" *)
  assign _rhs_val_lnot_32_10 = !ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:419.45-428.77" *)
  assign _rhs_val_bin_32_11 = _rhs_val_bin_32_9 && _rhs_val_lnot_32_10;
  (* src = "ct_ifu_icache_if.v:429.45-429.62" *)
  assign _rhs_val_lnot_32_12 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:419.45-429.62" *)
  assign _rhs_val_bin_32_13 = _rhs_val_bin_32_11 && _rhs_val_lnot_32_12;
  (* src = "ct_ifu_icache_if.v:431.48-431.82" *)
  assign _rhs_val_bin_34_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:431.46-432.48" *)
  assign _rhs_val_lnot_34_1 = !_rhs_val_bin_34_0;
  (* src = "ct_ifu_icache_if.v:433.46-434.48" *)
  assign _rhs_val_lnot_34_2 = !pcgen_icache_if_chgflw_bank1;
  (* src = "ct_ifu_icache_if.v:431.46-434.48" *)
  assign _rhs_val_bin_34_3 = _rhs_val_lnot_34_1 && _rhs_val_lnot_34_2;
  (* src = "ct_ifu_icache_if.v:435.46-436.48" *)
  assign _rhs_val_lnot_34_4 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:431.46-436.48" *)
  assign _rhs_val_bin_34_5 = _rhs_val_bin_34_3 && _rhs_val_lnot_34_4;
  (* src = "ct_ifu_icache_if.v:437.73-437.91" *)
  assign _rhs_val_array_static_34_6 = icache_way_pred[1];
  (* src = "ct_ifu_icache_if.v:437.52-437.91" *)
  assign _rhs_val_bin_34_7 = cp0_ifu_icache_en && _rhs_val_array_static_34_6;
  (* src = "ct_ifu_icache_if.v:437.50-437.92" *)
  assign _rhs_val_lnot_34_8 = !_rhs_val_bin_34_7;
  (* src = "ct_ifu_icache_if.v:431.46-437.92" *)
  assign _rhs_val_bin_34_9 = _rhs_val_bin_34_5 || _rhs_val_lnot_34_8;
  (* src = "ct_ifu_icache_if.v:439.45-439.77" *)
  assign _rhs_val_lnot_34_10 = !ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:430.45-439.77" *)
  assign _rhs_val_bin_34_11 = _rhs_val_bin_34_9 && _rhs_val_lnot_34_10;
  (* src = "ct_ifu_icache_if.v:440.45-440.62" *)
  assign _rhs_val_lnot_34_12 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:430.45-440.62" *)
  assign _rhs_val_bin_34_13 = _rhs_val_bin_34_11 && _rhs_val_lnot_34_12;
  (* src = "ct_ifu_icache_if.v:442.48-442.82" *)
  assign _rhs_val_bin_36_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:442.46-443.48" *)
  assign _rhs_val_lnot_36_1 = !_rhs_val_bin_36_0;
  (* src = "ct_ifu_icache_if.v:444.46-445.48" *)
  assign _rhs_val_lnot_36_2 = !pcgen_icache_if_chgflw_bank2;
  (* src = "ct_ifu_icache_if.v:442.46-445.48" *)
  assign _rhs_val_bin_36_3 = _rhs_val_lnot_36_1 && _rhs_val_lnot_36_2;
  (* src = "ct_ifu_icache_if.v:446.46-447.48" *)
  assign _rhs_val_lnot_36_4 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:442.46-447.48" *)
  assign _rhs_val_bin_36_5 = _rhs_val_bin_36_3 && _rhs_val_lnot_36_4;
  (* src = "ct_ifu_icache_if.v:448.73-448.91" *)
  assign _rhs_val_array_static_36_6 = icache_way_pred[1];
  (* src = "ct_ifu_icache_if.v:448.52-448.91" *)
  assign _rhs_val_bin_36_7 = cp0_ifu_icache_en && _rhs_val_array_static_36_6;
  (* src = "ct_ifu_icache_if.v:448.50-448.92" *)
  assign _rhs_val_lnot_36_8 = !_rhs_val_bin_36_7;
  (* src = "ct_ifu_icache_if.v:442.46-448.92" *)
  assign _rhs_val_bin_36_9 = _rhs_val_bin_36_5 || _rhs_val_lnot_36_8;
  (* src = "ct_ifu_icache_if.v:450.45-450.77" *)
  assign _rhs_val_lnot_36_10 = !ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:441.45-450.77" *)
  assign _rhs_val_bin_36_11 = _rhs_val_bin_36_9 && _rhs_val_lnot_36_10;
  (* src = "ct_ifu_icache_if.v:451.45-451.62" *)
  assign _rhs_val_lnot_36_12 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:441.45-451.62" *)
  assign _rhs_val_bin_36_13 = _rhs_val_bin_36_11 && _rhs_val_lnot_36_12;
  (* src = "ct_ifu_icache_if.v:453.48-453.82" *)
  assign _rhs_val_bin_38_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:453.46-454.48" *)
  assign _rhs_val_lnot_38_1 = !_rhs_val_bin_38_0;
  (* src = "ct_ifu_icache_if.v:455.46-456.48" *)
  assign _rhs_val_lnot_38_2 = !pcgen_icache_if_chgflw_bank3;
  (* src = "ct_ifu_icache_if.v:453.46-456.48" *)
  assign _rhs_val_bin_38_3 = _rhs_val_lnot_38_1 && _rhs_val_lnot_38_2;
  (* src = "ct_ifu_icache_if.v:457.46-458.48" *)
  assign _rhs_val_lnot_38_4 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:453.46-458.48" *)
  assign _rhs_val_bin_38_5 = _rhs_val_bin_38_3 && _rhs_val_lnot_38_4;
  (* src = "ct_ifu_icache_if.v:459.73-459.91" *)
  assign _rhs_val_array_static_38_6 = icache_way_pred[1];
  (* src = "ct_ifu_icache_if.v:459.52-459.91" *)
  assign _rhs_val_bin_38_7 = cp0_ifu_icache_en && _rhs_val_array_static_38_6;
  (* src = "ct_ifu_icache_if.v:459.50-459.92" *)
  assign _rhs_val_lnot_38_8 = !_rhs_val_bin_38_7;
  (* src = "ct_ifu_icache_if.v:453.46-459.92" *)
  assign _rhs_val_bin_38_9 = _rhs_val_bin_38_5 || _rhs_val_lnot_38_8;
  (* src = "ct_ifu_icache_if.v:461.45-461.77" *)
  assign _rhs_val_lnot_38_10 = !ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:452.45-461.77" *)
  assign _rhs_val_bin_38_11 = _rhs_val_bin_38_9 && _rhs_val_lnot_38_10;
  (* src = "ct_ifu_icache_if.v:462.45-462.62" *)
  assign _rhs_val_lnot_38_12 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:452.45-462.62" *)
  assign _rhs_val_bin_38_13 = _rhs_val_bin_38_11 && _rhs_val_lnot_38_12;
  (* src = "ct_ifu_icache_if.v:466.73-466.82" *)
  assign _rhs_val_lnot_40_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:466.47-466.82" *)
  assign _rhs_val_bin_40_1 = l1_refill_icache_if_wr && _rhs_val_lnot_40_0;
  (* src = "ct_ifu_icache_if.v:466.47-467.75" *)
  assign _rhs_val_bin_40_2 = _rhs_val_bin_40_1 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:466.47-468.81" *)
  assign _rhs_val_bin_40_3 = _rhs_val_bin_40_2 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:465.46-470.63" *)
  assign _rhs_val_bin_40_4 = _rhs_val_bin_40_3 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:465.46-471.77" *)
  assign _rhs_val_bin_40_5 = _rhs_val_bin_40_4 || ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:465.46-472.62" *)
  assign _rhs_val_bin_40_6 = _rhs_val_bin_40_5 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:474.73-474.82" *)
  assign _rhs_val_lnot_42_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:474.47-474.82" *)
  assign _rhs_val_bin_42_1 = l1_refill_icache_if_wr && _rhs_val_lnot_42_0;
  (* src = "ct_ifu_icache_if.v:474.47-475.75" *)
  assign _rhs_val_bin_42_2 = _rhs_val_bin_42_1 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:474.47-476.81" *)
  assign _rhs_val_bin_42_3 = _rhs_val_bin_42_2 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:473.46-478.63" *)
  assign _rhs_val_bin_42_4 = _rhs_val_bin_42_3 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:473.46-479.77" *)
  assign _rhs_val_bin_42_5 = _rhs_val_bin_42_4 || ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:473.46-480.62" *)
  assign _rhs_val_bin_42_6 = _rhs_val_bin_42_5 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:482.73-482.82" *)
  assign _rhs_val_lnot_44_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:482.47-482.82" *)
  assign _rhs_val_bin_44_1 = l1_refill_icache_if_wr && _rhs_val_lnot_44_0;
  (* src = "ct_ifu_icache_if.v:482.47-483.75" *)
  assign _rhs_val_bin_44_2 = _rhs_val_bin_44_1 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:482.47-484.81" *)
  assign _rhs_val_bin_44_3 = _rhs_val_bin_44_2 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:481.46-486.63" *)
  assign _rhs_val_bin_44_4 = _rhs_val_bin_44_3 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:481.46-487.77" *)
  assign _rhs_val_bin_44_5 = _rhs_val_bin_44_4 || ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:481.46-488.62" *)
  assign _rhs_val_bin_44_6 = _rhs_val_bin_44_5 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:490.73-490.82" *)
  assign _rhs_val_lnot_46_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:490.47-490.82" *)
  assign _rhs_val_bin_46_1 = l1_refill_icache_if_wr && _rhs_val_lnot_46_0;
  (* src = "ct_ifu_icache_if.v:490.47-491.75" *)
  assign _rhs_val_bin_46_2 = _rhs_val_bin_46_1 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:490.47-492.81" *)
  assign _rhs_val_bin_46_3 = _rhs_val_bin_46_2 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:489.46-494.63" *)
  assign _rhs_val_bin_46_4 = _rhs_val_bin_46_3 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:489.46-495.77" *)
  assign _rhs_val_bin_46_5 = _rhs_val_bin_46_4 || ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:489.46-496.62" *)
  assign _rhs_val_bin_46_6 = _rhs_val_bin_46_5 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:499.47-499.81" *)
  assign _rhs_val_bin_48_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:499.47-500.75" *)
  assign _rhs_val_bin_48_1 = _rhs_val_bin_48_0 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:499.47-501.81" *)
  assign _rhs_val_bin_48_2 = _rhs_val_bin_48_1 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:498.46-503.63" *)
  assign _rhs_val_bin_48_3 = _rhs_val_bin_48_2 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:498.46-504.77" *)
  assign _rhs_val_bin_48_4 = _rhs_val_bin_48_3 || ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:498.46-505.62" *)
  assign _rhs_val_bin_48_5 = _rhs_val_bin_48_4 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:507.47-507.81" *)
  assign _rhs_val_bin_50_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:507.47-508.75" *)
  assign _rhs_val_bin_50_1 = _rhs_val_bin_50_0 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:507.47-509.81" *)
  assign _rhs_val_bin_50_2 = _rhs_val_bin_50_1 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:506.46-511.63" *)
  assign _rhs_val_bin_50_3 = _rhs_val_bin_50_2 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:506.46-512.77" *)
  assign _rhs_val_bin_50_4 = _rhs_val_bin_50_3 || ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:506.46-513.62" *)
  assign _rhs_val_bin_50_5 = _rhs_val_bin_50_4 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:515.47-515.81" *)
  assign _rhs_val_bin_52_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:515.47-516.75" *)
  assign _rhs_val_bin_52_1 = _rhs_val_bin_52_0 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:515.47-517.81" *)
  assign _rhs_val_bin_52_2 = _rhs_val_bin_52_1 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:514.46-519.63" *)
  assign _rhs_val_bin_52_3 = _rhs_val_bin_52_2 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:514.46-520.77" *)
  assign _rhs_val_bin_52_4 = _rhs_val_bin_52_3 || ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:514.46-521.62" *)
  assign _rhs_val_bin_52_5 = _rhs_val_bin_52_4 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:523.47-523.81" *)
  assign _rhs_val_bin_54_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:523.47-524.75" *)
  assign _rhs_val_bin_54_1 = _rhs_val_bin_54_0 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:523.47-525.81" *)
  assign _rhs_val_bin_54_2 = _rhs_val_bin_54_1 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:522.46-527.63" *)
  assign _rhs_val_bin_54_3 = _rhs_val_bin_54_2 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:522.46-528.77" *)
  assign _rhs_val_bin_54_4 = _rhs_val_bin_54_3 || ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:522.46-529.62" *)
  assign _rhs_val_bin_54_5 = _rhs_val_bin_54_4 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:535.67-535.76" *)
  assign _rhs_val_lnot_56_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:535.41-535.76" *)
  assign _rhs_val_bin_56_1 = l1_refill_icache_if_wr && _rhs_val_lnot_56_0;
  (* src = "ct_ifu_icache_if.v:535.39-536.41" *)
  assign _rhs_val_lnot_56_2 = !_rhs_val_bin_56_1;
  (* src = "ct_ifu_icache_if.v:537.39-537.56" *)
  assign _rhs_val_lnot_56_3 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:535.39-537.56" *)
  assign _rhs_val_bin_56_4 = _rhs_val_lnot_56_2 && _rhs_val_lnot_56_3;
  (* src = "ct_ifu_icache_if.v:538.41-538.75" *)
  assign _rhs_val_bin_58_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:538.39-539.41" *)
  assign _rhs_val_lnot_58_1 = !_rhs_val_bin_58_0;
  (* src = "ct_ifu_icache_if.v:540.39-540.56" *)
  assign _rhs_val_lnot_58_2 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:538.39-540.56" *)
  assign _rhs_val_bin_58_3 = _rhs_val_lnot_58_1 && _rhs_val_lnot_58_2;
  (* src = "ct_ifu_icache_if.v:545.65-545.71" *)
  assign _rhs_val_convert_60_0 = 128'h0;
  (* src = "ct_ifu_icache_if.v:545.74-545.110" *)
  assign _rhs_val_range_slice_60_1 = l1_refill_icache_if_inst_data[127:0];
  (* src = "ct_ifu_icache_if.v:545.44-545.110" *)
  assign _rhs_val_mux_60_2 = icache_reset_inv ? _rhs_val_convert_60_0 : _rhs_val_range_slice_60_1;
  (* src = "ct_ifu_icache_if.v:546.65-546.71" *)
  assign _rhs_val_convert_62_0 = 128'h0;
  (* src = "ct_ifu_icache_if.v:546.74-546.110" *)
  assign _rhs_val_range_slice_62_1 = l1_refill_icache_if_inst_data[127:0];
  (* src = "ct_ifu_icache_if.v:546.44-546.110" *)
  assign _rhs_val_mux_62_2 = icache_reset_inv ? _rhs_val_convert_62_0 : _rhs_val_range_slice_62_1;
  (* src = "ct_ifu_icache_if.v:551.72-551.81" *)
  assign _rhs_val_lnot_64_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:551.46-551.81" *)
  assign _rhs_val_bin_64_1 = l1_refill_icache_if_wr && _rhs_val_lnot_64_0;
  (* src = "ct_ifu_icache_if.v:551.44-552.45" *)
  assign _rhs_val_lnot_64_2 = !_rhs_val_bin_64_1;
  (* src = "ct_ifu_icache_if.v:553.44-554.45" *)
  assign _rhs_val_lnot_64_3 = !pcgen_icache_if_chgflw;
  (* src = "ct_ifu_icache_if.v:551.44-554.45" *)
  assign _rhs_val_bin_64_4 = _rhs_val_lnot_64_2 && _rhs_val_lnot_64_3;
  (* src = "ct_ifu_icache_if.v:555.44-556.45" *)
  assign _rhs_val_lnot_64_5 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:551.44-556.45" *)
  assign _rhs_val_bin_64_6 = _rhs_val_bin_64_4 && _rhs_val_lnot_64_5;
  (* src = "ct_ifu_icache_if.v:557.70-557.88" *)
  assign _rhs_val_array_static_64_7 = icache_way_pred[0];
  (* src = "ct_ifu_icache_if.v:557.49-557.88" *)
  assign _rhs_val_bin_64_8 = cp0_ifu_icache_en && _rhs_val_array_static_64_7;
  (* src = "ct_ifu_icache_if.v:557.47-557.89" *)
  assign _rhs_val_lnot_64_9 = !_rhs_val_bin_64_8;
  (* src = "ct_ifu_icache_if.v:551.44-557.89" *)
  assign _rhs_val_bin_64_10 = _rhs_val_bin_64_6 || _rhs_val_lnot_64_9;
  (* src = "ct_ifu_icache_if.v:559.43-559.60" *)
  assign _rhs_val_lnot_64_11 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:551.42-559.60" *)
  assign _rhs_val_bin_64_12 = _rhs_val_bin_64_10 && _rhs_val_lnot_64_11;
  (* src = "ct_ifu_icache_if.v:560.46-560.80" *)
  assign _rhs_val_bin_66_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:560.44-561.46" *)
  assign _rhs_val_lnot_66_1 = !_rhs_val_bin_66_0;
  (* src = "ct_ifu_icache_if.v:562.44-563.46" *)
  assign _rhs_val_lnot_66_2 = !pcgen_icache_if_chgflw;
  (* src = "ct_ifu_icache_if.v:560.44-563.46" *)
  assign _rhs_val_bin_66_3 = _rhs_val_lnot_66_1 && _rhs_val_lnot_66_2;
  (* src = "ct_ifu_icache_if.v:564.44-565.46" *)
  assign _rhs_val_lnot_66_4 = !pcgen_icache_if_seq_data_req;
  (* src = "ct_ifu_icache_if.v:560.44-565.46" *)
  assign _rhs_val_bin_66_5 = _rhs_val_bin_66_3 && _rhs_val_lnot_66_4;
  (* src = "ct_ifu_icache_if.v:566.71-566.89" *)
  assign _rhs_val_array_static_66_6 = icache_way_pred[1];
  (* src = "ct_ifu_icache_if.v:566.50-566.89" *)
  assign _rhs_val_bin_66_7 = cp0_ifu_icache_en && _rhs_val_array_static_66_6;
  (* src = "ct_ifu_icache_if.v:566.48-566.90" *)
  assign _rhs_val_lnot_66_8 = !_rhs_val_bin_66_7;
  (* src = "ct_ifu_icache_if.v:560.44-566.90" *)
  assign _rhs_val_bin_66_9 = _rhs_val_bin_66_5 || _rhs_val_lnot_66_8;
  (* src = "ct_ifu_icache_if.v:568.43-568.60" *)
  assign _rhs_val_lnot_66_10 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:560.42-568.60" *)
  assign _rhs_val_bin_66_11 = _rhs_val_bin_66_9 && _rhs_val_lnot_66_10;
  (* src = "ct_ifu_icache_if.v:572.70-572.79" *)
  assign _rhs_val_lnot_68_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:572.44-572.79" *)
  assign _rhs_val_bin_68_1 = l1_refill_icache_if_wr && _rhs_val_lnot_68_0;
  (* src = "ct_ifu_icache_if.v:572.44-573.72" *)
  assign _rhs_val_bin_68_2 = _rhs_val_bin_68_1 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:572.44-574.78" *)
  assign _rhs_val_bin_68_3 = _rhs_val_bin_68_2 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:571.43-576.60" *)
  assign _rhs_val_bin_68_4 = _rhs_val_bin_68_3 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:571.43-577.59" *)
  assign _rhs_val_bin_68_5 = _rhs_val_bin_68_4 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:579.44-579.78" *)
  assign _rhs_val_bin_70_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:579.44-580.72" *)
  assign _rhs_val_bin_70_1 = _rhs_val_bin_70_0 || pcgen_icache_if_chgflw_short;
  (* src = "ct_ifu_icache_if.v:579.44-581.78" *)
  assign _rhs_val_bin_70_2 = _rhs_val_bin_70_1 || pcgen_icache_if_seq_data_req_short;
  (* src = "ct_ifu_icache_if.v:578.43-583.60" *)
  assign _rhs_val_bin_70_3 = _rhs_val_bin_70_2 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:578.43-584.59" *)
  assign _rhs_val_bin_70_4 = _rhs_val_bin_70_3 || icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:590.70-590.79" *)
  assign _rhs_val_lnot_72_0 = !fifo_bit;
  (* src = "ct_ifu_icache_if.v:590.44-590.79" *)
  assign _rhs_val_bin_72_1 = l1_refill_icache_if_wr && _rhs_val_lnot_72_0;
  (* src = "ct_ifu_icache_if.v:590.42-590.80" *)
  assign _rhs_val_lnot_72_2 = !_rhs_val_bin_72_1;
  (* src = "ct_ifu_icache_if.v:591.42-591.59" *)
  assign _rhs_val_lnot_72_3 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:590.42-591.59" *)
  assign _rhs_val_bin_72_4 = _rhs_val_lnot_72_2 && _rhs_val_lnot_72_3;
  (* src = "ct_ifu_icache_if.v:592.44-592.78" *)
  assign _rhs_val_bin_74_0 = l1_refill_icache_if_wr && fifo_bit;
  (* src = "ct_ifu_icache_if.v:592.42-592.79" *)
  assign _rhs_val_lnot_74_1 = !_rhs_val_bin_74_0;
  (* src = "ct_ifu_icache_if.v:593.42-593.59" *)
  assign _rhs_val_lnot_74_2 = !icache_reset_inv;
  (* src = "ct_ifu_icache_if.v:592.42-593.59" *)
  assign _rhs_val_bin_74_3 = _rhs_val_lnot_74_1 && _rhs_val_lnot_74_2;
  (* src = "ct_ifu_icache_if.v:598.67-598.72" *)
  assign _rhs_val_convert_76_0 = 32'h0;
  (* src = "ct_ifu_icache_if.v:598.75-598.109" *)
  assign _rhs_val_range_slice_76_1 = l1_refill_icache_if_pre_code[31:0];
  (* src = "ct_ifu_icache_if.v:598.46-598.109" *)
  assign _rhs_val_mux_76_2 = icache_reset_inv ? _rhs_val_convert_76_0 : _rhs_val_range_slice_76_1;
  (* src = "ct_ifu_icache_if.v:599.67-599.72" *)
  assign _rhs_val_convert_78_0 = 32'h0;
  (* src = "ct_ifu_icache_if.v:599.75-599.109" *)
  assign _rhs_val_range_slice_78_1 = l1_refill_icache_if_pre_code[31:0];
  (* src = "ct_ifu_icache_if.v:599.46-599.109" *)
  assign _rhs_val_mux_78_2 = icache_reset_inv ? _rhs_val_convert_78_0 : _rhs_val_range_slice_78_1;
  (* src = "ct_ifu_icache_if.v:617.33-617.58" *)
  assign _rhs_val_range_slice_80_0 = icache_index_higher[15:0];
  (* src = "ct_ifu_icache_if.v:618.33-618.60" *)
  assign _rhs_val_range_slice_80_1 = pcgen_icache_if_index[15:0];
  (* src = "ct_ifu_icache_if.v:616.33-618.60" *)
  assign _rhs_val_mux_80_2 = icache_req_higher ? _rhs_val_range_slice_80_0 : _rhs_val_range_slice_80_1;
  (* src = "ct_ifu_icache_if.v:619.33-620.59" *)
  assign _rhs_val_bin_82_0 = ifctrl_icache_if_tag_req || ifctrl_icache_if_reset_req;
  (* src = "ct_ifu_icache_if.v:619.33-622.55" *)
  assign _rhs_val_bin_82_1 = _rhs_val_bin_82_0 || l1_refill_icache_if_wr;
  (* src = "ct_ifu_icache_if.v:619.33-623.50" *)
  assign _rhs_val_bin_82_2 = _rhs_val_bin_82_1 || ipb_icache_if_req;
  (* src = "ct_ifu_icache_if.v:619.33-624.64" *)
  assign _rhs_val_bin_82_3 = _rhs_val_bin_82_2 || ifctrl_icache_if_read_req_data0;
  (* src = "ct_ifu_icache_if.v:619.33-625.64" *)
  assign _rhs_val_bin_82_4 = _rhs_val_bin_82_3 || ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:619.33-626.62" *)
  assign _rhs_val_bin_82_5 = _rhs_val_bin_82_4 || ifctrl_icache_if_read_req_tag;
  (* src = "ct_ifu_icache_if.v:633.26-634.57" *)
  assign _rhs_val_bin_84_0 = ifctrl_icache_if_read_req_data0 || ifctrl_icache_if_read_req_data1;
  (* src = "ct_ifu_icache_if.v:633.26-635.55" *)
  assign _rhs_val_bin_84_1 = _rhs_val_bin_84_0 || ifctrl_icache_if_read_req_tag;
  (* src = "ct_ifu_icache_if.v:636.33-636.87" *)
  assign _rhs_val_bin_86_0 = ifctrl_icache_if_tag_req || ifctrl_icache_if_reset_req;
  (* src = "ct_ifu_icache_if.v:636.32-639.49" *)
  assign _rhs_val_concat_86_1 = {_rhs_val_bin_86_0, l1_refill_icache_if_wr, ipb_icache_if_req, icache_read_req};
  (* src = "ct_ifu_icache_if.v:647.6-647.27" *)
  assign _rhs_val_range_slice_89_0 = icache_index_sel[3:0];
  (* src = "ct_ifu_icache_if.v:648.3-648.10" *)
  assign _rhs_val_convert_89_1 = 4'h8;
  (* src = "ct_ifu_icache_if.v:641.1-641.1" *)
  assign _comb_ctrl_val_case_eq_88_1 = _rhs_val_range_slice_89_0 == _rhs_val_convert_89_1;
  (* src = "ct_ifu_icache_if.v:648.40-648.68" *)
  assign _rhs_val_range_slice_89_2 = ifctrl_icache_if_index[15:0];
  (* src = "ct_ifu_icache_if.v:649.3-649.10" *)
  assign _rhs_val_convert_89_3 = 4'h4;
  (* src = "ct_ifu_icache_if.v:641.1-641.1" *)
  assign _comb_ctrl_val_case_eq_88_3 = _rhs_val_range_slice_89_0 == _rhs_val_convert_89_3;
  (* src = "ct_ifu_icache_if.v:649.40-649.71" *)
  assign _rhs_val_range_slice_89_4 = l1_refill_icache_if_index[15:0];
  (* src = "ct_ifu_icache_if.v:650.3-650.10" *)
  assign _rhs_val_convert_89_5 = 4'h2;
  (* src = "ct_ifu_icache_if.v:641.1-641.1" *)
  assign _comb_ctrl_val_case_eq_88_5 = _rhs_val_range_slice_89_0 == _rhs_val_convert_89_5;
  (* src = "ct_ifu_icache_if.v:650.41-650.66" *)
  assign _rhs_val_range_slice_89_6 = ipb_icache_if_index[10:0];
  (* src = "ct_ifu_icache_if.v:650.67-650.71" *)
  assign _rhs_val_const_89_7 = 5'h0;
  (* src = "ct_ifu_icache_if.v:650.40-650.72" *)
  assign _rhs_val_concat_89_8 = {_rhs_val_range_slice_89_6, _rhs_val_const_89_7};
  (* src = "ct_ifu_icache_if.v:651.3-651.10" *)
  assign _rhs_val_convert_89_9 = 4'h1;
  (* src = "ct_ifu_icache_if.v:641.1-641.1" *)
  assign _comb_ctrl_val_case_eq_88_7 = _rhs_val_range_slice_89_0 == _rhs_val_convert_89_9;
  (* src = "ct_ifu_icache_if.v:651.40-651.77" *)
  assign _rhs_val_range_slice_89_10 = ifctrl_icache_if_read_req_index[15:0];
  (* src = "ct_ifu_icache_if.v:652.44-652.48" *)
  assign _rhs_val_const_89_11 = 1'bx;
  (* src = "ct_ifu_icache_if.v:652.40-652.50" *)
  assign _rhs_val_replicate_89_12 = {16{_rhs_val_const_89_11}};
  (* src = "ct_ifu_icache_if.v:135.18-135.18" *)
  assign icache_index_higher_case_88_1 = _comb_ctrl_val_case_eq_88_7 ? _rhs_val_range_slice_89_10 : _rhs_val_replicate_89_12;
  (* src = "ct_ifu_icache_if.v:135.18-135.18" *)
  assign icache_index_higher_case_88_3 = _comb_ctrl_val_case_eq_88_5 ? _rhs_val_concat_89_8 : icache_index_higher_case_88_1;
  (* src = "ct_ifu_icache_if.v:135.18-135.18" *)
  assign icache_index_higher_case_88_5 = _comb_ctrl_val_case_eq_88_3 ? _rhs_val_range_slice_89_4 : icache_index_higher_case_88_3;
  (* src = "ct_ifu_icache_if.v:135.18-135.18" *)
  assign icache_index_higher_case_88_7 = _comb_ctrl_val_case_eq_88_1 ? _rhs_val_range_slice_89_2 : icache_index_higher_case_88_5;
  (* src = "ct_ifu_icache_if.v:662.45-662.70" *)
  assign _rhs_val_range_slice_91_0 = icache_ifu_tag_dout[28:0];
  (* src = "ct_ifu_icache_if.v:663.45-663.71" *)
  assign _rhs_val_range_slice_93_0 = icache_ifu_tag_dout[57:29];
  (* src = "ct_ifu_icache_if.v:664.45-664.68" *)
  assign _rhs_val_array_static_95_0 = icache_ifu_tag_dout[58];
  (* src = "ct_ifu_icache_if.v:665.45-665.70" *)
  assign _rhs_val_range_slice_97_0 = icache_ifu_tag_dout[28:0];
  (* src = "ct_ifu_icache_if.v:666.45-666.71" *)
  assign _rhs_val_range_slice_99_0 = icache_ifu_tag_dout[57:29];
  (* src = "ct_ifu_icache_if.v:667.45-667.81" *)
  assign _rhs_val_range_slice_101_0 = icache_ifu_data_array0_dout[127:0];
  (* src = "ct_ifu_icache_if.v:668.45-668.81" *)
  assign _rhs_val_range_slice_103_0 = icache_ifu_data_array1_dout[127:0];
  (* src = "ct_ifu_icache_if.v:673.43-673.79" *)
  assign _rhs_val_range_slice_105_0 = icache_ifu_predecd_array0_dout[31:0];
  (* src = "ct_ifu_icache_if.v:674.43-674.77" *)
  assign _rhs_val_range_slice_107_0 = icache_ifu_data_array0_dout[127:0];
  (* src = "ct_ifu_icache_if.v:676.43-676.79" *)
  assign _rhs_val_range_slice_109_0 = icache_ifu_predecd_array1_dout[31:0];
  (* src = "ct_ifu_icache_if.v:677.43-677.77" *)
  assign _rhs_val_range_slice_111_0 = icache_ifu_data_array1_dout[127:0];
  (* src = "ct_ifu_icache_if.v:691.40-691.66" *)
  assign _rhs_val_range_slice_113_0 = icache_ifu_tag_dout[28:0];
  (* src = "ct_ifu_icache_if.v:692.40-692.66" *)
  assign _rhs_val_range_slice_115_0 = icache_ifu_tag_dout[57:29];
  (* src = "ct_ifu_icache_if.v:697.46-697.100" *)
  assign _rhs_val_bin_117_0 = pcgen_icache_if_seq_data_req || pcgen_icache_if_chgflw;
  (* src = "ct_ifu_icache_if.v:697.45-697.121" *)
  assign _rhs_val_bin_117_1 = _rhs_val_bin_117_0 && cp0_ifu_icache_en;
  (* src = "ct_ifu_icache_if.v:704.24-704.28" *)
  assign _rhs_val_convert_123_0 = 1'h0;
  (* src = "ct_ifu_icache_if.v:718.23-718.59" *)
  assign _rhs_val_bin_126_0 = cp0_ifu_icache_en && hpcp_ifu_cnt_en;
  (* src = "ct_ifu_icache_if.v:722.6-722.15" *)
  assign _rhs_val_lnot_129_0 = !cpurst_b;
  (* src = "ct_ifu_icache_if.v:720.1-720.1" *)
  assign _comb_ctrl_val_not_128_1 = !_rhs_val_lnot_129_0;
  (* src = "ct_ifu_icache_if.v:724.35-724.39" *)
  assign _rhs_val_convert_129_1 = 1'h0;
  (* src = "ct_ifu_icache_if.v:725.35-725.39" *)
  assign _rhs_val_convert_129_2 = 1'h0;
  (* src = "ct_ifu_icache_if.v:727.11-727.47" *)
  assign _rhs_val_bin_129_3 = cp0_ifu_icache_en && hpcp_ifu_cnt_en;
  (* src = "ct_ifu_icache_if.v:720.1-720.1" *)
  assign _comb_ctrl_val_not_128_3 = !_rhs_val_bin_129_3;
  (* src = "ct_ifu_icache_if.v:720.1-720.1" *)
  assign _comb_ctrl_val_and_128_5 = _comb_ctrl_val_not_128_1 & _rhs_val_bin_129_3;
  (* src = "ct_ifu_icache_if.v:720.1-720.1" *)
  assign _comb_ctrl_val_and_128_7 = _comb_ctrl_val_not_128_1 & _comb_ctrl_val_not_128_3;
  (* src = "ct_ifu_icache_if.v:136.18-136.18" *)
  assign ifu_hpcp_icache_access_reg_if_128_1 = _rhs_val_bin_129_3 ? ifu_hpcp_icache_access_pre : ifu_hpcp_icache_access_reg;
  (* src = "ct_ifu_icache_if.v:137.18-137.18" *)
  assign ifu_hpcp_icache_miss_reg_if_128_3 = _rhs_val_bin_129_3 ? ifu_hpcp_icache_miss_pre : ifu_hpcp_icache_miss_reg;
  (* src = "ct_ifu_icache_if.v:136.18-136.18" *)
  assign ifu_hpcp_icache_access_reg_if_128_5 = _rhs_val_lnot_129_0 ? _rhs_val_convert_129_1 : ifu_hpcp_icache_access_reg_if_128_1;
  (* src = "ct_ifu_icache_if.v:137.18-137.18" *)
  assign ifu_hpcp_icache_miss_reg_if_128_7 = _rhs_val_lnot_129_0 ? _rhs_val_convert_129_2 : ifu_hpcp_icache_miss_reg_if_128_3;
  (* src = "ct_ifu_icache_if.v:266.8-266.28" *)
  assign ifu_icache_tag_cen_b = _rhs_val_bin_0_20;
  (* src = "ct_ifu_icache_if.v:285.8-285.29" *)
  assign ifu_icache_tag_clk_en = _rhs_val_bin_2_4;
  (* src = "ct_ifu_icache_if.v:305.1-305.1" *)
  assign ifu_icache_tag_wen_concat_3 = {_comb_ctrl_val_shadow_slice_4_3, _comb_ctrl_val_shadow_slice_7_3};
  (* src = "ct_ifu_icache_if.v:305.1-305.1" *)
  assign ifu_icache_tag_wen = ifu_icache_tag_wen_concat_3;
  (* src = "ct_ifu_icache_if.v:337.8-337.16" *)
  assign fifo_bit = l1_refill_icache_if_fifo;
  (* src = "ct_ifu_icache_if.v:346.8-346.20" *)
  assign tag_fifo_din = _rhs_val_mux_12_1;
  (* src = "ct_ifu_icache_if.v:350.8-350.21" *)
  assign tag_valid_din = l1_refill_icache_if_last;
  (* src = "ct_ifu_icache_if.v:351.8-351.24" *)
  assign tag_pc_din = _rhs_val_mux_16_3;
  (* src = "ct_ifu_icache_if.v:354.8-354.32" *)
  assign ifu_icache_tag_din = _rhs_val_concat_18_2;
  (* src = "ct_ifu_icache_if.v:370.8-370.28" *)
  assign icache_way_pred = _rhs_val_mux_20_2;
  (* src = "ct_ifu_icache_if.v:373.8-373.24" *)
  assign icache_reset_inv = ifctrl_icache_if_reset_req;
  (* src = "ct_ifu_icache_if.v:374.8-374.42" *)
  assign ifu_icache_data_array0_bank0_cen_b = _rhs_val_bin_24_14;
  (* src = "ct_ifu_icache_if.v:385.8-385.42" *)
  assign ifu_icache_data_array0_bank1_cen_b = _rhs_val_bin_26_14;
  (* src = "ct_ifu_icache_if.v:396.8-396.42" *)
  assign ifu_icache_data_array0_bank2_cen_b = _rhs_val_bin_28_14;
  (* src = "ct_ifu_icache_if.v:407.8-407.42" *)
  assign ifu_icache_data_array0_bank3_cen_b = _rhs_val_bin_30_14;
  (* src = "ct_ifu_icache_if.v:419.8-419.42" *)
  assign ifu_icache_data_array1_bank0_cen_b = _rhs_val_bin_32_13;
  (* src = "ct_ifu_icache_if.v:430.8-430.42" *)
  assign ifu_icache_data_array1_bank1_cen_b = _rhs_val_bin_34_13;
  (* src = "ct_ifu_icache_if.v:441.8-441.42" *)
  assign ifu_icache_data_array1_bank2_cen_b = _rhs_val_bin_36_13;
  (* src = "ct_ifu_icache_if.v:452.8-452.42" *)
  assign ifu_icache_data_array1_bank3_cen_b = _rhs_val_bin_38_13;
  (* src = "ct_ifu_icache_if.v:465.8-465.43" *)
  assign ifu_icache_data_array0_bank0_clk_en = _rhs_val_bin_40_6;
  (* src = "ct_ifu_icache_if.v:473.8-473.43" *)
  assign ifu_icache_data_array0_bank1_clk_en = _rhs_val_bin_42_6;
  (* src = "ct_ifu_icache_if.v:481.8-481.43" *)
  assign ifu_icache_data_array0_bank2_clk_en = _rhs_val_bin_44_6;
  (* src = "ct_ifu_icache_if.v:489.8-489.43" *)
  assign ifu_icache_data_array0_bank3_clk_en = _rhs_val_bin_46_6;
  (* src = "ct_ifu_icache_if.v:498.8-498.43" *)
  assign ifu_icache_data_array1_bank0_clk_en = _rhs_val_bin_48_5;
  (* src = "ct_ifu_icache_if.v:506.8-506.43" *)
  assign ifu_icache_data_array1_bank1_clk_en = _rhs_val_bin_50_5;
  (* src = "ct_ifu_icache_if.v:514.8-514.43" *)
  assign ifu_icache_data_array1_bank2_clk_en = _rhs_val_bin_52_5;
  (* src = "ct_ifu_icache_if.v:522.8-522.43" *)
  assign ifu_icache_data_array1_bank3_clk_en = _rhs_val_bin_54_5;
  (* src = "ct_ifu_icache_if.v:535.8-535.36" *)
  assign ifu_icache_data_array0_wen_b = _rhs_val_bin_56_4;
  (* src = "ct_ifu_icache_if.v:538.8-538.36" *)
  assign ifu_icache_data_array1_wen_b = _rhs_val_bin_58_3;
  (* src = "ct_ifu_icache_if.v:545.8-545.41" *)
  assign ifu_icache_data_array0_din = _rhs_val_mux_60_2;
  (* src = "ct_ifu_icache_if.v:546.8-546.41" *)
  assign ifu_icache_data_array1_din = _rhs_val_mux_62_2;
  (* src = "ct_ifu_icache_if.v:551.8-551.39" *)
  assign ifu_icache_predecd_array0_cen_b = _rhs_val_bin_64_12;
  (* src = "ct_ifu_icache_if.v:560.8-560.39" *)
  assign ifu_icache_predecd_array1_cen_b = _rhs_val_bin_66_11;
  (* src = "ct_ifu_icache_if.v:571.8-571.40" *)
  assign ifu_icache_predecd_array0_clk_en = _rhs_val_bin_68_5;
  (* src = "ct_ifu_icache_if.v:578.8-578.40" *)
  assign ifu_icache_predecd_array1_clk_en = _rhs_val_bin_70_4;
  (* src = "ct_ifu_icache_if.v:590.8-590.39" *)
  assign ifu_icache_predecd_array0_wen_b = _rhs_val_bin_72_4;
  (* src = "ct_ifu_icache_if.v:592.8-592.39" *)
  assign ifu_icache_predecd_array1_wen_b = _rhs_val_bin_74_3;
  (* src = "ct_ifu_icache_if.v:598.8-598.43" *)
  assign ifu_icache_predecd_array0_din = _rhs_val_mux_76_2;
  (* src = "ct_ifu_icache_if.v:599.8-599.43" *)
  assign ifu_icache_predecd_array1_din = _rhs_val_mux_78_2;
  (* src = "ct_ifu_icache_if.v:616.8-616.30" *)
  assign ifu_icache_index = _rhs_val_mux_80_2;
  (* src = "ct_ifu_icache_if.v:619.8-619.25" *)
  assign icache_req_higher = _rhs_val_bin_82_5;
  (* src = "ct_ifu_icache_if.v:633.8-633.23" *)
  assign icache_read_req = _rhs_val_bin_84_1;
  (* src = "ct_ifu_icache_if.v:636.8-636.29" *)
  assign icache_index_sel = _rhs_val_concat_86_1;
  (* src = "ct_ifu_icache_if.v:641.1-641.1" *)
  assign icache_index_higher = icache_index_higher_case_88_7;
  (* src = "ct_ifu_icache_if.v:662.8-662.38" *)
  assign icache_if_ifdp_tag_data0 = _rhs_val_range_slice_91_0;
  (* src = "ct_ifu_icache_if.v:663.8-663.38" *)
  assign icache_if_ifdp_tag_data1 = _rhs_val_range_slice_93_0;
  (* src = "ct_ifu_icache_if.v:664.8-664.27" *)
  assign icache_if_ifdp_fifo = _rhs_val_array_static_95_0;
  (* src = "ct_ifu_icache_if.v:665.8-665.40" *)
  assign icache_if_ifctrl_tag_data0 = _rhs_val_range_slice_97_0;
  (* src = "ct_ifu_icache_if.v:666.8-666.40" *)
  assign icache_if_ifctrl_tag_data1 = _rhs_val_range_slice_99_0;
  (* src = "ct_ifu_icache_if.v:667.8-667.42" *)
  assign icache_if_ifctrl_inst_data0 = _rhs_val_range_slice_101_0;
  (* src = "ct_ifu_icache_if.v:668.8-668.42" *)
  assign icache_if_ifctrl_inst_data1 = _rhs_val_range_slice_103_0;
  (* src = "ct_ifu_icache_if.v:673.8-673.37" *)
  assign icache_if_ifdp_precode0 = _rhs_val_range_slice_105_0;
  (* src = "ct_ifu_icache_if.v:674.8-674.40" *)
  assign icache_if_ifdp_inst_data0 = _rhs_val_range_slice_107_0;
  (* src = "ct_ifu_icache_if.v:676.8-676.37" *)
  assign icache_if_ifdp_precode1 = _rhs_val_range_slice_109_0;
  (* src = "ct_ifu_icache_if.v:677.8-677.40" *)
  assign icache_if_ifdp_inst_data1 = _rhs_val_range_slice_111_0;
  (* src = "ct_ifu_icache_if.v:691.8-691.37" *)
  assign icache_if_ipb_tag_data0 = _rhs_val_range_slice_113_0;
  (* src = "ct_ifu_icache_if.v:692.8-692.37" *)
  assign icache_if_ipb_tag_data1 = _rhs_val_range_slice_115_0;
  (* src = "ct_ifu_icache_if.v:697.8-697.34" *)
  assign ifu_hpcp_icache_access_pre = _rhs_val_bin_117_1;
  (* src = "ct_ifu_icache_if.v:718.8-718.19" *)
  assign hpcp_clk_en = _rhs_val_bin_126_0;
  (* src = "ct_ifu_icache_if.v:739.8-739.30" *)
  assign ifu_hpcp_icache_access = ifu_hpcp_icache_access_reg;
  (* src = "ct_ifu_icache_if.v:740.8-740.28" *)
  assign ifu_hpcp_icache_miss = ifu_hpcp_icache_miss_reg;

  (* src = "ct_ifu_icache_if.v:136.18-136.18" *)
  always @(posedge hpcp_clk or negedge cpurst_b) begin
    if (!cpurst_b) begin
      ifu_hpcp_icache_access_reg <= _rhs_val_convert_129_1;
    end else if (_rhs_val_bin_129_3) begin
      ifu_hpcp_icache_access_reg <= ifu_hpcp_icache_access_pre;
    end
  end
  (* src = "ct_ifu_icache_if.v:137.18-137.18" *)
  always @(posedge hpcp_clk or negedge cpurst_b) begin
    if (!cpurst_b) begin
      ifu_hpcp_icache_miss_reg <= _rhs_val_convert_129_2;
    end else if (_rhs_val_bin_129_3) begin
      ifu_hpcp_icache_miss_reg <= ifu_hpcp_icache_miss_pre;
    end
  end
endmodule

module gated_clk_cell (
  (* src = "stub_modules.v:158.9-158.9" *)
  input wire clk_in,
  (* src = "stub_modules.v:159.9-159.9" *)
  input wire global_en,
  (* src = "stub_modules.v:160.9-160.9" *)
  input wire module_en,
  (* src = "stub_modules.v:161.9-161.9" *)
  input wire local_en,
  (* src = "stub_modules.v:162.9-162.9" *)
  input wire external_en,
  (* src = "stub_modules.v:163.9-163.9" *)
  input wire pad_yy_icg_scan_en,
  (* src = "stub_modules.v:164.10-164.10" *)
  output wire clk_out
);
endmodule

module ct_ifu_icache_tag_array (
  (* src = "stub_modules.v:14.9-14.9" *)
  input wire forever_cpuclk,
  (* src = "stub_modules.v:15.9-15.9" *)
  input wire cp0_ifu_icg_en,
  (* src = "stub_modules.v:16.16-16.16" *)
  input wire [15:0] ifu_icache_index,
  (* src = "stub_modules.v:17.9-17.9" *)
  input wire ifu_icache_tag_cen_b,
  (* src = "stub_modules.v:18.9-18.9" *)
  input wire ifu_icache_tag_clk_en,
  (* src = "stub_modules.v:19.16-19.16" *)
  input wire [58:0] ifu_icache_tag_din,
  (* src = "stub_modules.v:20.16-20.16" *)
  input wire [2:0] ifu_icache_tag_wen,
  (* src = "stub_modules.v:21.9-21.9" *)
  input wire pad_yy_icg_scan_en,
  (* src = "stub_modules.v:22.17-22.17" *)
  output wire [58:0] icache_ifu_tag_dout
);
endmodule

module ct_ifu_icache_data_array0 (
  (* src = "stub_modules.v:43.9-43.9" *)
  input wire cp0_yy_clk_en,
  (* src = "stub_modules.v:44.9-44.9" *)
  input wire cp0_ifu_icg_en,
  (* src = "stub_modules.v:45.9-45.9" *)
  input wire forever_cpuclk,
  (* src = "stub_modules.v:46.9-46.9" *)
  input wire ifu_icache_data_array0_bank0_cen_b,
  (* src = "stub_modules.v:47.9-47.9" *)
  input wire ifu_icache_data_array0_bank0_clk_en,
  (* src = "stub_modules.v:48.9-48.9" *)
  input wire ifu_icache_data_array0_bank1_cen_b,
  (* src = "stub_modules.v:49.9-49.9" *)
  input wire ifu_icache_data_array0_bank1_clk_en,
  (* src = "stub_modules.v:50.9-50.9" *)
  input wire ifu_icache_data_array0_bank2_cen_b,
  (* src = "stub_modules.v:51.9-51.9" *)
  input wire ifu_icache_data_array0_bank2_clk_en,
  (* src = "stub_modules.v:52.9-52.9" *)
  input wire ifu_icache_data_array0_bank3_cen_b,
  (* src = "stub_modules.v:53.9-53.9" *)
  input wire ifu_icache_data_array0_bank3_clk_en,
  (* src = "stub_modules.v:54.17-54.17" *)
  input wire [127:0] ifu_icache_data_array0_din,
  (* src = "stub_modules.v:55.9-55.9" *)
  input wire ifu_icache_data_array0_wen_b,
  (* src = "stub_modules.v:56.17-56.17" *)
  input wire [15:0] ifu_icache_index,
  (* src = "stub_modules.v:57.9-57.9" *)
  input wire pad_yy_icg_scan_en,
  (* src = "stub_modules.v:58.18-58.18" *)
  output wire [127:0] icache_ifu_data_array0_dout
);
endmodule

module ct_ifu_icache_data_array1 (
  (* src = "stub_modules.v:79.9-79.9" *)
  input wire cp0_yy_clk_en,
  (* src = "stub_modules.v:80.9-80.9" *)
  input wire cp0_ifu_icg_en,
  (* src = "stub_modules.v:81.9-81.9" *)
  input wire forever_cpuclk,
  (* src = "stub_modules.v:82.9-82.9" *)
  input wire ifu_icache_data_array1_bank0_cen_b,
  (* src = "stub_modules.v:83.9-83.9" *)
  input wire ifu_icache_data_array1_bank0_clk_en,
  (* src = "stub_modules.v:84.9-84.9" *)
  input wire ifu_icache_data_array1_bank1_cen_b,
  (* src = "stub_modules.v:85.9-85.9" *)
  input wire ifu_icache_data_array1_bank1_clk_en,
  (* src = "stub_modules.v:86.9-86.9" *)
  input wire ifu_icache_data_array1_bank2_cen_b,
  (* src = "stub_modules.v:87.9-87.9" *)
  input wire ifu_icache_data_array1_bank2_clk_en,
  (* src = "stub_modules.v:88.9-88.9" *)
  input wire ifu_icache_data_array1_bank3_cen_b,
  (* src = "stub_modules.v:89.9-89.9" *)
  input wire ifu_icache_data_array1_bank3_clk_en,
  (* src = "stub_modules.v:90.17-90.17" *)
  input wire [127:0] ifu_icache_data_array1_din,
  (* src = "stub_modules.v:91.9-91.9" *)
  input wire ifu_icache_data_array1_wen_b,
  (* src = "stub_modules.v:92.17-92.17" *)
  input wire [15:0] ifu_icache_index,
  (* src = "stub_modules.v:93.9-93.9" *)
  input wire pad_yy_icg_scan_en,
  (* src = "stub_modules.v:94.18-94.18" *)
  output wire [127:0] icache_ifu_data_array1_dout
);
endmodule

module ct_ifu_icache_predecd_array0 (
  (* src = "stub_modules.v:110.9-110.9" *)
  input wire cp0_ifu_icg_en,
  (* src = "stub_modules.v:111.9-111.9" *)
  input wire cp0_yy_clk_en,
  (* src = "stub_modules.v:112.9-112.9" *)
  input wire forever_cpuclk,
  (* src = "stub_modules.v:113.9-113.9" *)
  input wire ifu_icache_data_array0_wen_b,
  (* src = "stub_modules.v:114.16-114.16" *)
  input wire [15:0] ifu_icache_index,
  (* src = "stub_modules.v:115.9-115.9" *)
  input wire ifu_icache_predecd_array0_cen_b,
  (* src = "stub_modules.v:116.9-116.9" *)
  input wire ifu_icache_predecd_array0_clk_en,
  (* src = "stub_modules.v:117.16-117.16" *)
  input wire [31:0] ifu_icache_predecd_array0_din,
  (* src = "stub_modules.v:118.9-118.9" *)
  input wire ifu_icache_predecd_array0_wen_b,
  (* src = "stub_modules.v:119.9-119.9" *)
  input wire pad_yy_icg_scan_en,
  (* src = "stub_modules.v:120.17-120.17" *)
  output wire [31:0] icache_ifu_predecd_array0_dout
);
endmodule

module ct_ifu_icache_predecd_array1 (
  (* src = "stub_modules.v:136.9-136.9" *)
  input wire cp0_ifu_icg_en,
  (* src = "stub_modules.v:137.9-137.9" *)
  input wire cp0_yy_clk_en,
  (* src = "stub_modules.v:138.9-138.9" *)
  input wire forever_cpuclk,
  (* src = "stub_modules.v:139.9-139.9" *)
  input wire ifu_icache_data_array1_wen_b,
  (* src = "stub_modules.v:140.16-140.16" *)
  input wire [15:0] ifu_icache_index,
  (* src = "stub_modules.v:141.9-141.9" *)
  input wire ifu_icache_predecd_array1_cen_b,
  (* src = "stub_modules.v:142.9-142.9" *)
  input wire ifu_icache_predecd_array1_clk_en,
  (* src = "stub_modules.v:143.16-143.16" *)
  input wire [31:0] ifu_icache_predecd_array1_din,
  (* src = "stub_modules.v:144.9-144.9" *)
  input wire ifu_icache_predecd_array1_wen_b,
  (* src = "stub_modules.v:145.9-145.9" *)
  input wire pad_yy_icg_scan_en,
  (* src = "stub_modules.v:146.17-146.17" *)
  output wire [31:0] icache_ifu_predecd_array1_dout
);
endmodule
