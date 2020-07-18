`timescale 1ns / 1ps

`include "rcntlr_defines.v"

///////////////////////////////////////////////////////////////////////////////
// Date: 29/02/2020
// Module: lna.sv
// Project: WSN
// Description: emulates the LNA (it is a simple signal switch)
//				 


module lna(input signal_in, output signal_out, input lna_pd,
		    input [1:0] lna_mode );


   
   //-------------------------------- Signal Limiting---- -------------------------------
   assign signal_out = lna_pd ? 0 : signal_in);

   always @*
      if( (lna_mode!=2'b01) && ~lna_pd)
	$display("$WARNING: LNA gain mode is not the default value!");
   
endmodule
