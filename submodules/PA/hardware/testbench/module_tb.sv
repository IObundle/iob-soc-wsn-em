`timescale 1ns / 1ps

module pa_tb 
  (
   input       in, 
   output      out, 
   input       pd,
   input [1:0] mode 
   );

   assign out = pd ? 0 : in;

   always @*
      if( mode && ~pd)
	$display("$WARNING: RF PA power mode is not the default value!");
   
endmodule
