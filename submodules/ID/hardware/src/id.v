`timescale 1ns/1ps
`include "header.vh"

module id
  # (
     parameter DATA_W = 32,
     parameter ID = 0
     )
   (
	input                   clk,
	input                   rst,

    // CPU interface
	input                   valid,
    input                   address,
    input                   wstrb,
    output reg [DATA_W-1:0] rdata,
	output reg              ready
    );

   always @(posedge clk, posedge rst) begin
      if (rst) begin
         ready <= 1'b0;
      end else begin
         ready <= valid;
      end
   end

   always @* begin
      case (address)
        `ID_VALUE: rdata = ID;
        default: rdata = {DATA_W{1'b0}};
      endcase
   end

endmodule
