/*
 * RX module
 * 
 * Description: after a "start" pulse, waits for rx_valid=1; after that, 
 * reads rx to find the access address; after that, parallelizes 
 * the data and stores bytes in a FIFO. Users start the module, 
 * wait until FIFO is not empty and read the amount of bytes desired. A reset 
 * pulse should be issued to receive a new packet.
 * 
 */
 

`timescale 1ns / 1ps
`include "header.vh"

module rx (
           input                 clk, 
           input                 rst,
	       input                 en,
           
           //data interface 
           input                 rd_en, 
           output [7:0]          data_out, 
   
           //registers
           input                 start, 
           input [31:0]          aa, //access address to be found
           input [`CH_IDX_W-1:0] ch_idx, //channel id to init the whitening register
           input [`NB_PKG_W-1:0] nb_pkg,
           output                aa_found, //access address found
           output                empty,
	       output reg            crc_valid,

           //serial interface
           input                 rx, 
           input                 rx_valid
           );
   
 

   //serial to parallel converter input
   wire                         wht_out;

   //fifo input interface 
   wire [7:0]                   fifodata_in;
   wire                         wr_en;

   wire                         full;		       
   wire [23:0]			crc_out;
   

   wire                 rst_aa;
   
   aa_find aa_find0 (
		     .clk               (clk),
		     .rst               (rst | start | rst_aa),

		     .aa                (aa),
		     .data_in           (rx),
		     .en                (rx_valid & en),

		     .aa_found        (aa_found)
		     );
 
   whitening dewhitener (
			 .clk			(clk),
			 .rst			(rst),
			 
			 .init			(~aa_found),
			 .ch_idx		(ch_idx),

			 .data_in		(rx),
			 .en			(rx_valid & en),

			 .data_out		(wht_out)
			 );
   
   byte_s2p s2p (
		 .clk			(clk),
		 .start                 (aa_found),

		 .serial_en		(rx_valid & en),
		 .serial_in		(wht_out),

		 .data_out		(fifodata_in),
		 .data_out_en		(wr_en)
		 );

   syn_fifo ififo (
		   .clk			(clk),
		   .rst			(rst),
		   .flush               (start),

		   .full		(full),
		   .empty		(empty),
		   .data_out		(data_out),

		   .rd_en		(rd_en),
		   .wr_en		(wr_en),
		   .data_in		(fifodata_in));
      crc crc (
	    // Inputs
	    .rst (rst | start),
	    .clk	(clk),
	    .data_in	(wht_out),
	    .crc_en	(rx_valid & aa_found & en),			   
	    // Outputs
	    .lfsr		(crc_out),
	    .data_out ()
	    );

   //assign crc_valid = (crc_out == 24'd0 && aa_found) ? 1'b1: 1'b0;

   always @*
     if(rst|start)
       crc_valid = 1'b0;
     else if (crc_out == 24'd0 && aa_found)
       crc_valid = 1'b1;

   reg [`NB_PKG_W-1:0] counter;
   wire en_cnt = (counter == 0)? 1'b0 : 1'b1;
   always @(posedge clk, posedge rst) begin
      if (rst) begin
         counter <= {`NB_PKG_W{1'b0}};
      end else if (start) begin
         counter <= nb_pkg;
      end else if (en_cnt & wr_en) begin
         counter <= counter - 1'b1;
      end
   end

   reg en_cnt_reg;
   assign rst_aa = ~en_cnt & en_cnt_reg;
   always @(posedge clk, posedge rst) begin
      if (rst) begin
         en_cnt_reg <= 1'b0;
      end else begin
         en_cnt_reg <= en_cnt;
      end
   end

endmodule // rx

