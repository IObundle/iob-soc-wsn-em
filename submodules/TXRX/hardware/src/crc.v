`timescale 1ns / 1ps
//
// CRC module for BLE
// Polynomial: x^24 + x^10 + x^9 + x^6 + x^4 + x^3 + x^1 + 1
//
//
module crc(
	   input             rst,
	   input             clk,

	   input             data_in,
	   input             crc_en,
	   output reg [23:0] lfsr,
           output            data_out
           );

   // NOTE: BLE advertising packets employ INIT_VAL = 24'h555555
   //parameter [23:0] INIT_VAL = 24'h0;
   parameter [23:0] INIT_VAL = 24'h555555;
   
   //reg [23:0] 	  lfsr;

   assign data_out = data_in ^ lfsr[23];
   always @(posedge clk, posedge rst) begin

   if(rst)
     lfsr <= INIT_VAL;
   else if (crc_en) begin 
      lfsr[23:11] <= lfsr[22:10];
      lfsr[10] <= lfsr[9] ^ data_out;
      lfsr[9] <= lfsr[8] ^ data_out;
      lfsr[8:7] <= lfsr[7:6];
      lfsr[6] <= lfsr[5] ^ data_out;
      lfsr[5] <= lfsr[4];
      lfsr[4] <= lfsr[3] ^ data_out;
      lfsr[3] <= lfsr[2] ^ data_out;
      lfsr[2] <= lfsr[1];
      lfsr[1] <= lfsr[0] ^ data_out;
      lfsr[0] <= data_out;
   end 
end // always

endmodule // crc
