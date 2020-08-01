`timescale 1ns/1ps
`include "header.vh"

module txrx (
             input                    clk,
             input                    rst,

             //cpu interface 
             input                    valid,
             input [`TXRX_ADDR_W-1:0] address,
             input [7:0]              wdata,
             input                    wstrb,
             output reg [31:0]        rdata,
             output reg               ready,

             //serial interface
             output                   tx,
             output                   tx_valid,

             input                    rx
             );


   // cpu interface ready signal
   always @(posedge clk, posedge rst)
     if(rst)
       ready <= 1'b0;
     else 
       ready <= valid;
   //
   //address decoder
   //
   wire 			      rx_in;
   wire [7:0] 			      rx_data;
   wire 			      rx_empty;
   wire 			      rx_aa_found;
   wire 			      rxdata_in_valid;
   wire 			      rx_crc_valid;
   wire 			      tx_ready;

   reg 				      tx_start;
   reg 				      rx_start;
   reg [31:0] 			      aa;
   reg [`CH_IDX_W-1:0] 		      ch_idx;
   reg 				      rx_en;
   reg 				      tx_en;
   reg 				      demod_en;
   
   //read
   always @*
	case (address)
	  `RX_DATA: rdata = {24'b0, rx_data};
	  `RX_EMPTY: rdata = {31'b0, rx_empty};
	  `RX_AA_FOUND: rdata = {31'b0, rx_aa_found};
	  `RX_CRC_VALID: rdata = {31'b0, rx_crc_valid};
	  `TX_READY: rdata = {31'b0,tx_ready};
	  default: rdata = 32'hFFFFFFFF;
    endcase

   //write
   always @(posedge clk, posedge rst)
     if(rst) begin
        tx_start <= 1'b0;
	tx_en <= 1'b0;
        aa <= `ACCESSADDRESS;
        ch_idx <= `CH_IDX_W'd5;
        rx_start <= 1'b0;
	rx_en <= 1'b0;
	demod_en <= 1'b0;
     end else if(valid && wstrb)
       case (address)
         `TX_START: tx_start <= wdata[0];
	 `TX_EN: tx_en <= wdata[0];
         `TXRX_AA: aa <= wdata;
         `TXRX_CH_IDX: ch_idx <= wdata[`CH_IDX_W-1:0];
         `RX_START: rx_start <= wdata[0];
	 `RX_EN: rx_en <= wdata[0];
	 `DEMOD_EN: demod_en <= wdata[0];
         default: ;
       endcase
   
   //transmitter
   tx tx0 (
	   .rst				(rst),
	   .clk				(clk),
	   .en                          (tx_en),
      
           //data interface
	   .txdata_in			(wdata[7:0]),
	   .wr_en                       (valid & wstrb & ~ready &&  
                                         (address == `TX_DATA)),

           //registers
	   .txready			(tx_ready), //read
	   .txstart			(tx_start), //write
	   .aa                          (aa), //write
	   .ch_idx			(ch_idx), //write
           
           //serial interface
	   .tx			        (tx),
	   .tx_valid                    (tx_valid)
           );

   //fsk demodulator
   fsk_demod fsk_demod ( 
                       .clk(clk),
                       .rst(rst | rx_start),
		       .en(rx_en & demod_en), //write
                       .data_in(rx), // limiter output // DTEST OUTPUT
                       .data_out(rx_in),// DTEST OUTPUT
                       .data_out_valid(rxdata_in_valid) // DTEST OUTPUT
                       );
   //receiver
   rx rx0 (
	   .rst				(rst),
	   .clk				(clk),
	   .en (rx_en),

	   //data interface 
	   .data_out			(rx_data),
	   .rd_en			(valid & ~wstrb & ~ready),//read is same as (valid & ~wstrb)

	   //registers
	   .start                       (rx_start), //write
	   .ch_idx                      (ch_idx), //write
           .aa                          (aa), //write
	   .aa_found                    (rx_aa_found), //read
	   .empty			(rx_empty), //empty
	   .crc_valid (rx_crc_valid),

	   // serial interface
	   .rx   			(rx_in),
	   .rx_valid		        (rxdata_in_valid)
	   );
endmodule 
