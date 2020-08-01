/*
 * TX byte serializer @1Mb/s using a 32MHz clock signal
 */
`timescale 1ns/1ps

module byte_p2s (/*AUTOARG*/
		 // Outputs
		 serial_out, serial_en, serial_en_middle, ready, bit_valid,
		 // Inputs
		 clk, rst, start, data_in, en
		 );

   input clk, rst; //clock and reset 
   input en;
   input start; //start byte serialization 
   input [7:0] data_in; //input byte to be serialized 
   
   output serial_out; //serialized output
   output reg ready; //ready for next serialization
   output reg serial_en; //serial data valid bit
   output reg serial_en_middle; //serial data valid bit
   output     bit_valid; //valid throughout the whole bit duration
    

   parameter rest_st=1'b0, serialize_st=1'b1; //state encoding

   reg [0:0] state, state_nxt; //state and next state (1 bit so far) 
   reg 	     ready_nxt;
   reg [2:0] bitcnt, bitcnt_nxt; //bit counter 
   reg [4:0] cyccnt, cyccnt_nxt; //cycle counter (1 bit = 4 cycles at 16MHz)
   reg [7:0] data_reg, data_reg_nxt; //data register 

   assign bit_valid = state;
   
   assign serial_out = data_reg[7]; //serialized output LSB first
   
		      
   always @ *
     begin
	state_nxt = state; //default next state
	data_reg_nxt = data_reg;
	ready_nxt = ready;
	serial_en = 1'b0;
	serial_en_middle = 1'b0;
	bitcnt_nxt = bitcnt;
	
	case (state)
	  rest_st : begin //waiting for start 
	     bitcnt_nxt = 3'b000;
	     cyccnt_nxt = 4'b0000;
	     if(start)begin
		data_reg_nxt = data_in;
		ready_nxt = 1'b0; 
		state_nxt = serialize_st;
	     end
	  end
	  serialize_st : begin //serializing data
	     cyccnt_nxt = cyccnt+1'b1;
	     if(cyccnt == 5'b11110 && bitcnt == 3'b111) //must activate ready
	       ready_nxt = 1'b1;
		 if(cyccnt == 5'b01111)	serial_en_middle = 1'b1;
	     if(cyccnt == 5'b11111) begin //if cycle counter wraps, do next bit
		serial_en = 1'b1;
		bitcnt_nxt = bitcnt+1'b1;
		data_reg_nxt = data_reg<<1;
		cyccnt_nxt = 4'b0;
		if(bitcnt == 3'b111) begin //if bit counter wraps, byte done
		   if(~start)
		     state_nxt =rest_st;
		   else
		     ready_nxt = 1'b0;
		   data_reg_nxt = data_in;
		end 
	     end 
	  end	    
	endcase
     end 
   
   always @ (posedge rst, posedge clk) begin
      if (rst) begin
	 state <= rest_st;
	 data_reg <= 8'h00;
	 bitcnt <= 3'b000;
	 cyccnt <= 5'b00;
	 ready <= 1'b1;
      end else if(en) begin
	 state <= state_nxt;
	 data_reg <= data_reg_nxt;
	 bitcnt <= bitcnt_nxt;	   
	 cyccnt <= cyccnt_nxt;
	 ready <= ready_nxt;
      end
   end // always @ (posedge rst, posedge clk)
   
endmodule // byte_p2s
