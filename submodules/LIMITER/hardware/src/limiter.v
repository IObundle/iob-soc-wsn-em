`timescale 1ns / 1ps
`include "header.vh"

module limiter
  (
   input               clk,
   input               rst,

   // CPU interface
   input               valid,
   input               address,
   input [`DATA_W-1:0] wdata,
   input               wstrb,
   output reg          ready,

   // Serial i/f
   output reg          pd
   );

   // Compute ready
   always @(posedge clk, posedge rst) begin
      if (rst)
        ready <= 1'b0;
      else
        ready <= valid;
   end

   // Address decoder
   always @(posedge clk, posedge rst) begin
     if (rst) begin
        pd <= 1'b0;
     end else if (valid) begin
        if (address == `LIMITER_PD) begin
           pd <= wdata[0];
        end
     end
   end

endmodule
