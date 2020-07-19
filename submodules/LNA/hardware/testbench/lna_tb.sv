`timescale 1ns / 1ps

module lna_tb
(
 input       lna_in, 
 output      lna_out, 
 input       lna_pd,
 input [1:0] lna_mode 
 );

   assign signal_out = lna_pd ? 0 : signal_in);
   
   always @*
      if( (lna_mode!=2'b01) && ~lna_pd)
	$display("$WARNING: LNA gain mode is not the default value!");
   
endmodule
