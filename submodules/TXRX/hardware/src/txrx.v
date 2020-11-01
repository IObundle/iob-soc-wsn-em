`timescale 1ns/1ps
`include "header.vh"

module txrx (
             input                    clk,
             input                    rst,

             // CPU interface
             input                    valid,
             input [`TXRX_ADDR_W-1:0] address,
             input [31:0]             wdata,
             input                    wstrb,
             output reg [31:0]        rdata,
             output reg               ready,

             // Serial interface
             output                   tx,
             output                   tx_valid,

             input                    rx
             );


   // CPU interface ready signal
   always @(posedge clk, posedge rst)
     if (rst)
       ready <= 1'b0;
     else 
       ready <= valid;

   //
   // address decoder
   //
   wire 			      rx_in;
   wire [7:0]             rx_data;
   wire 			      rx_empty;
   wire 			      rx_aa_found;
   wire 			      rxdata_in_valid;
   wire 			      rx_crc_valid;
   wire 			      tx_ready;

   reg 				      tx_start;
   reg 				      rx_start;
   reg [31:0]             aa;
   reg [`CH_IDX_W-1:0]    ch_idx;
   reg 				      rx_en;
   reg 				      tx_en;
   reg 				      demod_en;
   reg [`NB_PKG_W-1:0]    rx_nb_pkg;

   // Read
   always @*
     case (address)
       `RX_DATA: rdata = {24'b0, rx_data};
       `RX_EMPTY: rdata = {31'b0, rx_empty};
       `RX_AA_FOUND: rdata = {31'b0, rx_aa_found};
       `RX_CRC_VALID: rdata = {31'b0, rx_crc_valid};
       `TX_READY: rdata = {31'b0,tx_ready};
       default: rdata = 32'hFFFFFFFF;
     endcase

   // Write
   always @(posedge clk, posedge rst)
     if (rst) begin
        tx_start <= 1'b0;
        tx_en <= 1'b0;
        aa <= `ACCESSADDRESS;
        ch_idx <= `CH_IDX_W'd5;
        rx_start <= 1'b0;
        rx_en <= 1'b0;
        rx_nb_pkg <= `NB_PKG;
        demod_en <= 1'b0;
     end else if (valid & wstrb)
       case (address)
         `TX_START: tx_start <= wdata[0];
         `TX_EN: tx_en <= wdata[0];
         `TXRX_AA: aa <= wdata;
         `TXRX_CH_IDX: ch_idx <= wdata[`CH_IDX_W-1:0];
         `RX_START: rx_start <= wdata[0];
         `RX_EN: rx_en <= wdata[0];
         `RX_NB_PKG: rx_nb_pkg <= wdata[`NB_PKG_W-1:0];
         `DEMOD_EN: demod_en <= wdata[0];
         default: ;
       endcase

   reg tx_start_reg;
   reg rx_start_reg;
   wire tx_start_int = tx_start & ~tx_start_reg;
   wire rx_start_int = rx_start & ~rx_start_reg;
   always @(posedge clk, posedge rst) begin
      if (rst) begin
         tx_start_reg <= 1'b0;
         rx_start_reg <= 1'b0;
      end else begin
         tx_start_reg <= tx_start;
         rx_start_reg <= rx_start;
      end
   end

   // Transmitter
   tx tx0 (
           .rst             (rst),
           .clk             (clk),
           .en              (tx_en),

           // Data interface
           .txdata_in       (wdata[7:0]),
           .wr_en           (valid & wstrb & ~ready &&
                             (address == `TX_DATA)),

           // Registers
           .txready         (tx_ready),     // read
           .txstart         (tx_start_int), // write
           .aa              (aa),           // write
           .ch_idx          (ch_idx),       // write

           // Serial interface
           .tx              (tx),
           .tx_valid        (tx_valid)
           );

   // FSK demodulator
   fsk_demod fsk_demod0 (
                         .clk            (clk),
                         .rst            (rst | rx_start_int),
                         .en             (rx_en & demod_en), // write
                         .data_in        (rx),               // limiter output // DTEST OUTPUT
                         .data_out       (rx_in),            // DTEST OUTPUT
                         .data_out_valid (rxdata_in_valid)   // DTEST OUTPUT
                         );
   // Receiver
   rx rx0 (
           .rst             (rst),
           .clk             (clk),
           .en              (rx_en),

           // Data interface
           .data_out        (rx_data),
           .rd_en           (valid & ~wstrb & ~ready &&
                             (address == `RX_DATA)),

           // Registers
           .start           (rx_start_int),            // write
           .ch_idx          (ch_idx),                  // write
           .aa              (aa),                      // write
           .nb_pkg          (rx_nb_pkg),               // write
           .aa_found        (rx_aa_found),             // read
           .empty           (rx_empty),                // empty
           .crc_valid       (rx_crc_valid),

           // Serial interface
           .rx              (rx_in),
           .rx_valid        (rxdata_in_valid)
           );

endmodule 
