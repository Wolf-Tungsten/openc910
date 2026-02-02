module soc(input wire clk,
           input wire rst_n,
           inout wire [7:0] b_pad_gpio_porta,
           output reg [3:0] count,
           output reg [7:0] pad_sample);
  reg [1:0] phase;
  reg [7:0] b_pad_gpio_porta__out;
  reg [7:0] b_pad_gpio_porta__oe;
  reg [3:0] shadow0;
  reg [3:0] shadow1;
  reg [3:0] shadow2;
  reg [3:0] shadow3;
  always @* begin
    case (phase)
      2'd0: begin
        b_pad_gpio_porta__out = 8'hA5;
        b_pad_gpio_porta__oe = 8'hFF;
      end
      2'd1: begin
        b_pad_gpio_porta__out = 8'h3C;
        b_pad_gpio_porta__oe = 8'hFF;
      end
      2'd2: begin
        b_pad_gpio_porta__out = 8'hF0;
        b_pad_gpio_porta__oe = 8'h00;
      end
      default: begin
        b_pad_gpio_porta__out = 8'h5A;
        b_pad_gpio_porta__oe = 8'hFF;
      end
    endcase
  end
  assign b_pad_gpio_porta = b_pad_gpio_porta__oe ? b_pad_gpio_porta__out : {8{1'bz}};
  always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      phase <= 2'd0;
      count <= 4'd0;
      pad_sample <= 8'h00;
      shadow0 <= 4'd0;
      shadow1 <= 4'd0;
      shadow2 <= 4'd0;
      shadow3 <= 4'd0;
    end else begin
      phase <= phase + 2'd1;
      count <= count + 4'd1;
      pad_sample <= b_pad_gpio_porta;
      shadow0 <= count;
      shadow1 <= shadow0 + 4'd1;
      shadow2 <= shadow1 + 4'd1;
      shadow3 <= shadow2 + 4'd1;
    end
  end
endmodule
