`timescale 1fs / 1fs
`include "rcntlr_defines.v"

//
// Authors: Patrick Gonçalves Pereira, Jose T. de Sousa
//


module gf(
	  input 		   rst,
	  input 		   clk,
	  input 		   clk_en,
	  input [1:0] 		   x,
	  output reg [`Q1_8SZ-1:0] y
		);

   parameter SIZE = 19;			// Filter Order

   wire [1:0] 			   x_l [SIZE-1:0]; 
   wire signed [`Q1_8SZ-1:0] 	   y_l [SIZE-1:0];

   wire signed [`Q1_8SZ-1:0] 	   h_pos [SIZE-1:0];

   // Coefficients for x=1
   assign h_pos[0] = 9'h 003;
   assign h_pos[1] = 9'h 004;
   assign h_pos[2] = 9'h 006;
   assign h_pos[3] = 9'h 009;
   assign h_pos[4] = 9'h 00C;
   assign h_pos[5] = 9'h 010;
   assign h_pos[6] = 9'h 013;
   assign h_pos[7] = 9'h 016;
   assign h_pos[8] = 9'h 018;
   assign h_pos[9] = 9'h 019;
   assign h_pos[10] = 9'h 018;
   assign h_pos[11] = 9'h 016;
   assign h_pos[12] = 9'h 013;
   assign h_pos[13] = 9'h 010;
   assign h_pos[14] = 9'h 00C;
   assign h_pos[15] = 9'h 009;
   assign h_pos[16] = 9'h 006;
   assign h_pos[17] = 9'h 004;
   assign h_pos[18] = 9'h 003;

   assign x_l[0] = x;
   assign y_l[0] = 9'h0;

   // Generate SIZE-1 sub-modules. 
   genvar 		     i; 
   generate
      for (i=0; i<(SIZE-1); i=i+1) begin: fir_path
	 fir_tap U (
		    .x(x_l[i]),
		    .x_d(x_l[i+1]),
		    .h_pos(h_pos[i]),
		    .y_p(y_l[i]),
		    .y(y_l[i+1]),
		    .rst(rst),
		    .clk(clk),
		    .clk_en(clk_en)
		    );
      end
   endgenerate

   //last submodule
   always @ (x_l[SIZE-1], y_l[SIZE-1],  h_pos[SIZE-1]) begin
      case(x_l[SIZE-1])
	2'b01: y = y_l[SIZE-1] + h_pos[SIZE-1];	
	2'b11: y = y_l[SIZE-1] - h_pos[SIZE-1];
	default: y = 9'b000000000;
      endcase
   end

endmodule
