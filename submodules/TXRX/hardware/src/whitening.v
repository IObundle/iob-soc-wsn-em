//
// BLE Whitening module 
//

`timescale 1ns / 1ps

module whitening(
	   input                 data_in,
	   input                 en,
	   input                 init,
	   input [`CH_IDX_W-1:0] ch_idx,
	   output                data_out,
	   input                 rst,
	   input                 clk);

   reg [6:0] 	  lfsr;
   integer 	  i;
   wire data_out1;

`ifdef WHITE_DIS
   assign data_out = data_in;
`else
   assign data_out1 = data_in ^ lfsr[6];
   assign data_out = (init == 1'b1)? data_in :  data_out1;
`endif
   
     
   always @(posedge clk, posedge rst) 
      begin
      if(rst)
	lfsr <= 7'b0000000;
      else if (init) begin
	 lfsr[0] <= 1'b1; //must be 1 as per BLE
	 for(i=1; i < 7; i=i+1)
	   lfsr[i] <= ch_idx[5-i+1];
      end else if (en) begin 
	 lfsr[6:5] <= lfsr[5:4];
	 lfsr[4] <= lfsr[6] ^ lfsr[3];
	 lfsr[3:1] <= lfsr[2:0];
	 lfsr[0] <= lfsr[6];
      end 
   end // always

endmodule //
