// Stub for openC910 module - simulates IFU stall behavior
// This replaces the full C910 CPU core to isolate the IFU issue

module openC910 (
    input wire axim_clk_en,
    input wire pad_biu_arready,
    input wire pad_biu_awready,
    input wire [7:0] pad_biu_bid,
    input wire [1:0] pad_biu_bresp,
    input wire pad_biu_bvalid,
    input wire pad_biu_csysreq,
    input wire [127:0] pad_biu_rdata,
    input wire [7:0] pad_biu_rid,
    input wire pad_biu_rlast,
    input wire [3:0] pad_biu_rresp,
    input wire pad_biu_rvalid,
    input wire pad_biu_wready,
    input wire pad_core0_dbg_mask,
    input wire pad_core0_dbgrq_b,
    input wire pad_core0_rst_b,
    input wire [2:0] pad_core0_hartid,
    input wire [39:0] pad_cpu_apb_base,
    input wire pad_cpu_l2cache_flush_req,
    input wire pad_cpu_rst_b,
    input wire [39:0] pad_core0_rvba,
    input wire [63:0] pad_cpu_sys_cnt,
    input wire pad_had_jtg_tclk,
    input wire pad_had_jtg_tdi,
    input wire pad_had_jtg_tms,
    input wire pad_had_jtg_trst_b,
    input wire [2:0] pad_l2c_data_mbist_clk_ratio,
    input wire [2:0] pad_l2c_tag_mbist_clk_ratio,
    input wire [143:0] pad_plic_int_cfg,
    input wire [143:0] pad_plic_int_vld,
    input wire pad_yy_dft_clk_rst_b,
    input wire pad_yy_icg_scan_en,
    input wire pad_yy_mbist_mode,
    input wire pad_yy_scan_enable,
    input wire pad_yy_scan_mode,
    input wire pad_yy_scan_rst_b,
    input wire pll_cpu_clk,
    output reg [39:0] biu_pad_araddr,
    output reg [1:0] biu_pad_arburst,
    output reg [3:0] biu_pad_arcache,
    output reg [7:0] biu_pad_arid,
    output reg [7:0] biu_pad_arlen,
    output reg biu_pad_arlock,
    output reg [2:0] biu_pad_arprot,
    output reg [2:0] biu_pad_arsize,
    output reg biu_pad_arvalid,
    output reg [39:0] biu_pad_awaddr,
    output reg [1:0] biu_pad_awburst,
    output reg [3:0] biu_pad_awcache,
    output reg [7:0] biu_pad_awid,
    output reg [7:0] biu_pad_awlen,
    output reg biu_pad_awlock,
    output reg [2:0] biu_pad_awprot,
    output reg [2:0] biu_pad_awsize,
    output reg biu_pad_awvalid,
    output reg biu_pad_bready,
    output reg biu_pad_cactive,
    output reg biu_pad_csysack,
    output reg biu_pad_rready,
    output reg [127:0] biu_pad_wdata,
    output reg biu_pad_wlast,
    output reg [15:0] biu_pad_wstrb,
    output reg biu_pad_wvalid,
    output reg [1:0] core0_pad_jdb_pm,
    output reg [1:0] core0_pad_lpmd_b,
    output reg [63:0] core0_pad_mstatus,
    output reg core0_pad_retire0,
    output reg core0_pad_retire1,
    output reg core0_pad_retire2,
    output reg [39:0] core0_pad_retire0_pc,
    output reg [39:0] core0_pad_retire1_pc,
    output reg [39:0] core0_pad_retire2_pc,
    output reg [63:0] cpu_debug_port,
    input wire pad_core1_dbg_mask,
    input wire pad_core1_dbgrq_b,
    input wire pad_core1_rst_b,
    input wire [2:0] pad_core1_hartid,
    input wire [39:0] pad_core1_rvba,
    output reg [1:0] core1_pad_jdb_pm,
    output reg [1:0] core1_pad_lpmd_b,
    output reg [63:0] core1_pad_mstatus,
    output reg core1_pad_retire0,
    output reg core1_pad_retire1,
    output reg core1_pad_retire2,
    output reg [39:0] core1_pad_retire0_pc,
    output reg [39:0] core1_pad_retire1_pc,
    output reg [39:0] core1_pad_retire2_pc,
    output reg cpu_pad_l2cache_flush_done,
    output reg cpu_pad_no_op,
    output reg had_pad_jtg_tdo,
    output reg had_pad_jtg_tdo_en
);

    // IFU state machine
    reg [3:0] state;
    reg [15:0] cycle_cnt;
    reg [7:0] retire_cnt;
    reg [39:0] ifu_addr;
    
    localparam IDLE = 4'd0;
    localparam REQ = 4'd1;
    localparam WAIT_GRNT = 4'd2;
    localparam WAIT_DATA = 4'd3;
    localparam RETIRE = 4'd4;
    localparam STALL = 4'd5;  // Bug state: IFU stops requesting
    
    // AXI write channels - unused for IFU test
    always @(*) begin
        biu_pad_awaddr = 40'h0;
        biu_pad_awburst = 2'b0;
        biu_pad_awcache = 4'b0;
        biu_pad_awid = 8'h0;
        biu_pad_awlen = 8'h0;
        biu_pad_awlock = 1'b0;
        biu_pad_awprot = 3'b0;
        biu_pad_awsize = 3'b0;
        biu_pad_awvalid = 1'b0;
        biu_pad_bready = 1'b0;
        biu_pad_wvalid = 1'b0;
        biu_pad_wdata = 128'h0;
        biu_pad_wlast = 1'b0;
        biu_pad_wstrb = 16'h0;
        biu_pad_cactive = 1'b1;
        biu_pad_csysack = 1'b0;
        core1_pad_jdb_pm = 2'b0;
        core1_pad_lpmd_b = 2'b0;
        core1_pad_mstatus = 64'h0;
        core1_pad_retire0 = 1'b0;
        core1_pad_retire1 = 1'b0;
        core1_pad_retire2 = 1'b0;
        core1_pad_retire0_pc = 40'h0;
        core1_pad_retire1_pc = 40'h0;
        core1_pad_retire2_pc = 40'h0;
        cpu_pad_l2cache_flush_done = 1'b0;
        cpu_pad_no_op = 1'b0;
        had_pad_jtg_tdo = 1'b0;
        had_pad_jtg_tdo_en = 1'b0;
        cpu_debug_port = 64'h0;
    end
    
    // Main IFU state machine - simulates the bug
    always @(posedge pll_cpu_clk or negedge pad_cpu_rst_b) begin
        if (!pad_cpu_rst_b) begin
            state <= IDLE;
            cycle_cnt <= 16'h0;
            retire_cnt <= 8'h0;
            ifu_addr <= 40'h0;
            biu_pad_arvalid <= 1'b0;
            biu_pad_araddr <= 40'h0;
            biu_pad_arlen <= 8'h0;
            biu_pad_arsize <= 3'b100;  // 16 bytes
            biu_pad_arburst <= 2'b01;  // INCR
            biu_pad_arcache <= 4'b0;
            biu_pad_arlock <= 1'b0;
            biu_pad_arprot <= 3'b0;
            biu_pad_arid <= 8'h0;
            biu_pad_rready <= 1'b1;
            core0_pad_retire0 <= 1'b0;
            core0_pad_retire1 <= 1'b0;
            core0_pad_retire2 <= 1'b0;
            core0_pad_retire0_pc <= 40'h0;
            core0_pad_retire1_pc <= 40'h0;
            core0_pad_retire2_pc <= 40'h0;
            core0_pad_lpmd_b <= 2'b11;
            core0_pad_jdb_pm <= 2'b0;
            core0_pad_mstatus <= 64'h0;
        end else begin
            cycle_cnt <= cycle_cnt + 1'b1;
            core0_pad_retire0 <= 1'b0;  // Default
            
            case (state)
                IDLE: begin
                    if (cycle_cnt > 16'd10) begin
                        state <= REQ;
                        ifu_addr <= 40'h0;
                    end
                end
                
                REQ: begin
                    biu_pad_arvalid <= 1'b1;
                    biu_pad_araddr <= ifu_addr;
                    biu_pad_arlen <= 8'h3;  // 4 beats
                    state <= WAIT_GRNT;
                end
                
                WAIT_GRNT: begin
                    if (pad_biu_arready) begin
                        biu_pad_arvalid <= 1'b0;
                        state <= WAIT_DATA;
                    end
                end
                
                WAIT_DATA: begin
                    if (pad_biu_rvalid && pad_biu_rlast) begin
                        state <= RETIRE;
                        core0_pad_retire0 <= 1'b1;
                        core0_pad_retire0_pc <= ifu_addr;
                        retire_cnt <= retire_cnt + 1'b1;
                        ifu_addr <= ifu_addr + 40'h10;
                    end
                end
                
                RETIRE: begin
                    // BUG: After 50 retires, stall for 50 cycles
                    if (retire_cnt >= 8'd50 && retire_cnt < 8'd100) begin
                        state <= STALL;
                    end else begin
                        state <= REQ;
                    end
                end
                
                STALL: begin
                    // IFU not requesting - simulating wolf bug
                    biu_pad_arvalid <= 1'b0;
                    if (retire_cnt >= 8'd100) begin
                        state <= REQ;
                    end
                end
                
                default: state <= IDLE;
            endcase
        end
    end
endmodule
