`timescale 1ns/1ps
`include "header.vh"

module lpf (
	    input                     clk,
	    input                     rst,

            //cpu interface 
	    input                     valid,
            input [`LPF_ADDR_W-1:0] address,
	    input [`DATA_W-1:0]       wdata,
            input                     wstrb,
	    output reg                ready,

            //serial i/f
	    output reg                pd,
	    output reg [7:0]          fc
            );


 always @(posedge clk, posedge rst)
     if(rst) begin          
        pd <= 1'b0;
        fc <= 8'b0;
     end else if(valid) begin
        if(address == `LPF_PD)
          pd <= wdata[0];
        else
          fc <= wdata[7:0];
        ready <= 1'b1;
     end else 
       ready <= 1'b0;

   
endmodule
