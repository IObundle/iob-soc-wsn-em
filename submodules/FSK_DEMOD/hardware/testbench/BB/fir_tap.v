`timescale 1fs / 1fs

// Authors: Patrick Gonçalves Pereira, Jose T. de Sousa

module fir_tap(
	       input signed [1:0] x,
	       output reg [1:0]   x_d,
	       input signed [8:0] h_pos,
	       input [8:0] 	  y_p,
	       output [8:0] 	  y,
	       input 		  rst,
	       input 		  clk,
	       input 		  clk_en
	       );

   reg [8:0] 			  prod;

   always @ (posedge rst, posedge clk) begin 
      if(rst == 1'b1)
	x_d <= 2'b 0;							
      else if(clk_en == 1'b1)
	x_d <= x;
   end 

   always @ * begin
      case(x)
	2'b01: prod = h_pos;	
	2'b11: prod = -h_pos;
	default: prod = 9'b000000000;
      endcase
   end
 
   assign y = prod + y_p; 

endmodule
