/*
 * TX module
 * 
 * 
 * Description: if txready=1, write bytes to txdata_in using wr_en,
 * up to 39 bytes, issue txstart and wait until txready=1 again to 
 * send next packet; ch_idx is used in the whitening polynomial and 
 * aa is the address code to send; tx_valid qualifies tx bits.
 * 
 */

`timescale 1ns / 1ps
`include "txrx.vh"

module tx (

	   input                rst, clk, en,

           //data interface
	   input [7:0]          txdata_in,//FIFO parallel data input
	   input                wr_en,//write enable for txdata_in

           //registers
	   input                txstart,//start transmission
	   output reg           txready,//ready for transmission or last transmission done
	   input [`CH_IDX_W-1:0] ch_idx, //channel id used for whitening
	   input [31:0]         aa, //access address


           //serial output 
	   output               tx,//serial output 
	   output               tx_valid//serial output qualifier
	   

   );
   
   //state encoding 
   parameter wait_start_st=4'b0000, send_preamble_st=4'b0001,
     send_aa0_st=4'b0010, send_aa1_st=4'b0011, send_aa2_st=4'b0100,
     send_aa3_st=4'b0101, send_data_st=4'b0110, send_last_st=4'b0111,
     send_crc0_st=4'b1000,send_crc1_st=4'b1001,send_crc2_st=4'b1010;   
 
	
   reg txstart_sync;
   wire [7:0]                   preamble;
   
   reg [3:0]                    state_nxt, state;

   reg [7:0]                    data_to_send;

   wire                         byte_p2s_ready; // From dut of byte_p2s.v
   reg                          byte_p2s_start;
   wire                         serial_dat;
   wire                         serial_en;
   
   wire                         whitener_init;
   
   wire [7:0]                   fifodata_out;		// From ififo of syn_fifo.v
   wire [7:0]                   fifodata_in;
   wire                         empty;			// From ififo of syn_fifo.v
   wire                         full;			// From ififo of syn_fifo.v

   reg                          txready_nxt;
   reg                          rd_en;
   reg 				crc_en;
   wire [23:0] 			crc_out;
   wire 			serial_en_middle;
 			

   assign fifodata_in = txdata_in;
      
   //input FIFO
   syn_fifo ififo (/*AUTOINST*/
		   // Outputs
		   .full		(full),
		   .empty		(empty),
		   .data_out		(fifodata_out),
		   // Inputs
		   .clk			(clk),
		   .rst			(rst),
		   .rd_en		(rd_en),
		   .wr_en		(wr_en),
		   .flush               (1'b0),
		   .data_in		(fifodata_in));
   
   //parallel to serial converter 
   byte_p2s byte_p2s0 (/*AUTOINST*/
		       // Outputs
		       .serial_out	(serial_dat),
		       .serial_en       (serial_en),
		       .serial_en_middle       (serial_en_middle),
		       .ready		(byte_p2s_ready),
		       .bit_valid       (tx_valid),
		       // Inputs
		       .clk		(clk),
		       .rst		(rst),
			   .en      (en),	
		       .start		(byte_p2s_start),
		       .data_in		(data_to_send));
   

   whitening whitener (/*AUTOINST*/
		  // Outputs
		  .data_out		(tx),
		  // Inputs
		  .data_in		(serial_dat),
		  .en			(serial_en & en),
		  .init			(whitener_init),
		  .ch_idx		(ch_idx),
		  .rst			(rst),
		  .clk			(clk));

   crc crc (
	    // Inputs
	    .rst (rst | txstart),
	    .clk	(clk),
	    .data_in	(serial_dat),
	    .crc_en	(serial_en_middle & crc_en & en),			   
	    // Outputs
	    .lfsr		(crc_out),
	    .data_out ()
	    );


   //chose preamble according to BLE
   assign preamble = aa[31] ? `PREAMBLE_MSB1 : `PREAMBLE_MSB0;
   
   //init whitener
   assign whitener_init = (state == send_data_st || state == send_last_st 
			   || state == send_crc0_st || state == send_crc1_st 
			   || state == send_crc2_st)? 1'b0 : 1'b1;

   //tx control state machine
   always @ *  begin
      
      data_to_send = fifodata_out;
      state_nxt = state;
      rd_en = 1'b0;
      txready_nxt = txready;
      byte_p2s_start = 1'b1;
      crc_en = 1'b0;
	  
      case(state)
	wait_start_st: begin
	   byte_p2s_start = 1'b0;
	   if(txstart_sync) begin
	      byte_p2s_start = 1'b1;
	      state_nxt = send_preamble_st;
	      txready_nxt = 1'b0;
	      data_to_send = preamble;
		  rd_en = 1'b1; //draws first data item from fifo and holds it
	      
	   end 
	end // case: wait_start_st
	
	send_preamble_st: begin
	   data_to_send = preamble;
	   if(byte_p2s_ready) begin
	      data_to_send = aa[31:24]; 
	      state_nxt = send_aa0_st;
	   end
	end

	send_aa0_st: begin
	   if(byte_p2s_ready) begin
	      data_to_send = aa[3*8-1:2*8];
	      state_nxt = send_aa1_st;
	   end 
	end

	send_aa1_st: begin
	   if(byte_p2s_ready) begin
	      data_to_send = aa[2*8-1:8];
	      state_nxt = send_aa2_st;
	   end 
	end

	send_aa2_st: begin
	   if(byte_p2s_ready) begin
	      data_to_send = aa[7:0];
	      state_nxt = send_aa3_st;
	   end 
	end
	
	send_aa3_st: begin
	   if(byte_p2s_ready) begin
	      rd_en = 1'b1;
	      state_nxt = send_data_st;
	   end
	end
	   
	send_data_st: begin
	   crc_en = 1'b1;
	   if(byte_p2s_ready) begin
	      if(empty) //all bytes sent
		state_nxt = send_last_st;
	      else 
		 rd_en = 1'b1;
	   end
	end
	send_last_st: begin
	   crc_en = 1'b1;
	   //byte_p2s_start = 1'b0;
	   if(byte_p2s_ready) begin
	      //state_nxt = wait_start_st;
	      //txready_nxt = 1'b1;
	      state_nxt = send_crc0_st;
	      data_to_send = crc_out[23:16];
	   end    
	end
	send_crc0_st: begin
	   if(byte_p2s_ready) begin
	      state_nxt = send_crc1_st;
	      data_to_send = crc_out[15:8];
	   end    
	end
	send_crc1_st: begin
	   if(byte_p2s_ready) begin
	      state_nxt = send_crc2_st;
	      data_to_send = crc_out[7:0];
	   end    
	end
	send_crc2_st: begin
	   byte_p2s_start = 1'b0;
	   if(byte_p2s_ready) begin
	      state_nxt = wait_start_st;
	      txready_nxt = 1'b1;
	   end    
	end

      endcase // case (state)
   end // always @ *
   
   
   always @(posedge rst, posedge clk) begin
      if(rst)begin
	 state <= wait_start_st;
	 txready <= 1'b1;
     txstart_sync <= 1'b0;		 
      end else if(en) begin
	 state <= state_nxt;
	 txready <= txready_nxt;
	 txstart_sync <= txstart;
      end 
   end // always @ (posedge rst, posedge clk)
   	    
endmodule 
