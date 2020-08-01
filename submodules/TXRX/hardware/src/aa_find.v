/*
 * Access address finder and data qualifier
 * 
 * Description: after reset the module is waiting for data.
 * Every input bit in "data_in" qualified by "en" is stored in a shift 
 * register of 31 bits. The contents of the shift register is compared 
 * with the 31 msbs of the access address. Before it matches 
 * aa_found=0, after it matches data aa_found=1. Reset the module 
 * to find the access address again.
 * 
 * Author: Jose T. de Sousa
 */

`timescale 1ns / 1ps

module aa_find (
		input 	     clk,
		input 	     rst,
		
		input [31:0] aa,//access address to find 
		input 	     data_in, //input bitstream
		input 	     en,//input bit enable
		
		output reg   aa_found //goes high after aa is found
		); 

   //state encoding
   parameter aa_st=1'b0, data_st=1'b1;

   //reg [1:0] state, state_nxt;
   reg 			     state, state_nxt;

   reg [31:0] 		     aa_reg, aa_reg_nxt;

   //state machine
   always @ * begin
      state_nxt = state;                
      aa_found = 1'b0;
      
      aa_reg_nxt = aa_reg << 1;
      aa_reg_nxt[0] = data_in;

      if (state == aa_st) begin
	 if(aa_reg == aa) begin
	    state_nxt = data_st;
	    aa_found = 1'b1;
	 end 
      end else  begin //data_st
	 aa_found = 1'b1;
      end 
   end // always @ *
   
   always @ (posedge rst, posedge clk) begin
      if(rst) begin
	 state <= aa_st;
	 aa_reg <= 0;
      end else if(en) begin 
	 state <= state_nxt;
	 aa_reg <= aa_reg_nxt;
      end    
   end 

endmodule 
