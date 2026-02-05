module inout_vec_oe(
    input  wire       clk,
    input  wire       reset,
    input  wire [7:0] drive_data,
    input  wire [7:0] drive_oe,
    inout  wire [7:0] io,
    output reg  [7:0] sampled
);
    genvar i;
    generate
        for (i = 0; i < 8; i = i + 1) begin : gen_io
            assign io[i] = drive_oe[i] ? drive_data[i] : 1'bz;
        end
    endgenerate

    always @(posedge clk) begin
        if (reset) begin
            sampled <= 8'h00;
        end else begin
            sampled <= io;
        end
    end
endmodule
