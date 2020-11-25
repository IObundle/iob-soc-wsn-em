`timescale 1ns/1ps
`include "pa.vh"

module pa (
	    input               clk,
	    input               rst,

            //cpu interface 
	    input               valid,
            input               address,
	    input [`DATA_W-1:0] wdata,
            input               wstrb,
	    output reg          ready,

            //serial i/f
	    output reg          pd,
	    output reg [1:0]    mode
            );

   always @(posedge clk, posedge rst)
     if(rst) begin          
        pd <= 1'b0;
        mode <= 1'b0;
     end else 
       if(valid) begin
          if(address == `PA_PD)
            pd <= wdata[0];
          else
            mode <= wdata[0];
          ready <= 1'b1;
       end else 
         ready <= 1'b0;

endmodule
