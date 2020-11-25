`timescale 1ns/1ps
`include "mixer.vh"

module mixer (
	    input                     clk,
	    input                     rst,

            //cpu interface 
	    input                     valid,
            input [`MIXER_ADDR_W-1:0] address,
	    input [`DATA_W-1:0]       wdata,
            input                     wstrb,
	    output reg                ready,

            //serial i/f
	    output reg                pd,
	    output reg                ota,
	    output reg [1:0]          buff
            );


   always @(posedge clk, posedge rst)
     if(rst) begin          
        pd <= 1'b0;
        ota <= 1'b0;
        buff <= 2'b0;
     end else if(valid) begin
        if(address == `MIXER_PD)
          pd <= wdata[0];
        else if(address == `MIXER_OTA)
          ota <= wdata[0];
        else
          buff <= wdata[1:0];
        ready <= 1'b1;
     end else 
       ready <= 1'b0;

endmodule
