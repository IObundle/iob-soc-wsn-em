/*
 * Description: TX byte deserializer of an input bitstream where each
 * input bit in "serial_in" is validated by the "serial_en" signal and
 * each output byte is validated by the "data_out_en" signal. The conversion 
 * starts one clock cycle after the "start" signal is asserted.
 * 
 * Author: Jose T. de Sousa
 */

`timescale 1ns/1ps

module byte_s2p (

		 input 	      clk, 
   
		 input 	      start, //start conversion
		 input 	      serial_in, //serialized input data wire
		 input 	      serial_en, //input data qualifier

		 output [7:0] data_out, //output deserialized byte 
		 output       data_out_en //output data qualifier
		 );
 
   reg [2:0] bitcnt, bitcnt_nxt; //bit counter 
   reg [7:0] data_reg, data_reg_nxt; //data register
   reg 	     start_reg, start_nxt;//delayed start used internally
   reg 	     serial_en_reg;//delayed serial enable
   
   
   
   assign data_out = data_reg; //deserialized data output
   assign data_out_en =  start_reg & serial_en_reg & (bitcnt == 3'b0)? 1'b1: 1'b0;
		      
   always @ * begin
      data_reg_nxt = data_reg << 1;//shift reg
      data_reg_nxt[0] = serial_in;
      bitcnt_nxt = bitcnt+1;
      start_nxt = start_reg;
      if (bitcnt == 4) start_nxt = start;
   end
   
   always @ (posedge clk) begin
	if (~start) begin
	   data_reg <= 0;
	   bitcnt <= 0;
	   start_reg <= 0;
	end else if (serial_en ) begin
	   data_reg <= data_reg_nxt;
	   bitcnt <= bitcnt_nxt;
	   start_reg <= start_nxt;
	end
   end // always @ (posedge clk)

   always @ (posedge clk) 
     if (~start) 
       serial_en_reg <= 0;
     else
       serial_en_reg <= serial_en;
       
endmodule
