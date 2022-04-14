`timescale 1ns/1ps
`include "tmp.vh"

module tmp 
	#(
	  parameter ADC_RES = `ADC_RESOLUTION,	
	  parameter RDATA_W = `RDATA_W
	 )
	 (
	  //General interface signals
	  input 			clk,
	  input				rst,
	
	  //CPU interface signals
	  input				valid,
	  input				address,
	  output reg [RDATA_W - 1:0]	rdata,
	  output reg 			ready,
	
	  //ADC output signal
	  input [ADC_RES - 1:0] 	data_in
	 );
	 
	 
	 //CPU interface ready signal
	 always @(posedge clk, posedge rst)
	    if(rst)
	        ready <= 1'b0;
	    else
	         ready <= valid;
	 
	 //Read The output of the ADC
	 always @* begin
	    case(address)
	       `TMP_VALUE: rdata = {22'b0, data_in};
	       default: rdata = `RDATA_RVAL;    
	    endcase 
	 end       

endmodule
