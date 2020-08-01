//-----------------------------------------------------
// Function    : Single read and write ports regfile
// Coder       : Jose T. de Sousa
//-----------------------------------------------------
`timescale 1ns / 1ps

module regf_dp_d0w_d1r (
		     clk,
		     address_0 , // address_0 Input
		     data_0    , // data_0 bi-directional
		     we_0      , // Write Enable/Read Enable
		     address_1 , // address_1 Input
		     rd_1      ,     // data_1 bi-directional
		     data_1     // data_1 bi-directional
); 

   parameter DATA_WIDTH = 8 ;
   parameter ADDR_WIDTH = 3 ;
   parameter REGF_DEPTH = 1 << ADDR_WIDTH;

   input clk ;
   
   input [ADDR_WIDTH-1:0] address_0 ;
   input [DATA_WIDTH-1:0] data_0 ; 
   input 		  we_0 ;

   input [ADDR_WIDTH-1:0] address_1 ;
   output reg [DATA_WIDTH-1:0] data_1 ;
   input 		       rd_1 ;

   reg [DATA_WIDTH-1:0]        regf [0:REGF_DEPTH-1];

   // Write Operation
   always @ (posedge clk)
     begin 
	if ( we_0 ) begin
	   regf[address_0] <= data_0;
	end
     end

   // Read Operation
   always @ (posedge clk)
     begin 
	if (rd_1) begin
	   data_1 <= regf[address_1]; 
	end
     end
   
endmodule 
