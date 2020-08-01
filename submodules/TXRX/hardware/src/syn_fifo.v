//-----------------------------------------------------
// Design Name : syn_fifo
// File Name   : syn_fifo.v
// Function    : Synchronous (single clock) FIFO
// Coder       : Deepak Kumar Tala
// Source      : http://www.asic-world.com/code/hdl_models/syn_fifo.v
//             : No license required
// Modified by : Jose T. de Sousa (jose.t.de.sousa@gmail.com)
//-----------------------------------------------------
`timescale 1ns / 1ps

module syn_fifo (
clk      , // Clock input
rst      , // Active high reset
flush    , // Flush data in FIFO
data_in  , // Data input
rd_en    , // Read enable
wr_en    , // Write Enable
data_out , // Data Output
empty    , // FIFO empty
full       // FIFO full
);    
 
// FIFO constants
parameter DATA_WIDTH = 8;
parameter ADDR_WIDTH = 6;
parameter RAM_DEPTH = 42;
// Port Declarations
input clk ;
input rst ;
input flush ;
input rd_en ;
input wr_en ;
input [DATA_WIDTH-1:0] data_in ;
output full ;
output empty ;
output [DATA_WIDTH-1:0] data_out ;

//-----------Internal variables-------------------
reg [ADDR_WIDTH-1:0] wr_pointer;
reg [ADDR_WIDTH-1:0] rd_pointer;
reg [ADDR_WIDTH :0] status_cnt;
wire [DATA_WIDTH-1:0] data_out ;
wire [DATA_WIDTH-1:0] data_ram ;

//-----------Variable assignments---------------
assign full = (status_cnt == (RAM_DEPTH-1));
assign empty = (status_cnt == 0);

//-----------Code Start---------------------------
always @ (posedge clk or posedge rst)
begin : WRITE_POINTER
  if (rst) begin
    wr_pointer <= 0;
  end else if (flush) begin 
    wr_pointer <= 0;
  end else if (wr_en ) begin
    wr_pointer <= wr_pointer + 1;
  end
end

always @ (posedge clk or posedge rst)
begin : READ_POINTER
  if (rst) begin
    rd_pointer <= 0;
  end else if (flush) begin 
    rd_pointer <= 0;
  end else if (rd_en ) begin
    rd_pointer <= rd_pointer + 1;
  end
end

//always  @ (posedge clk or posedge rst)
//begin : READ_DATA
//  if (rst) begin
//    data_out <= 0;
//  end else if ( rd_en ) begin
   assign  data_out = data_ram;
//  end
//end

always @ (posedge clk or posedge rst)
begin : STATUS_COUNTER
  if (rst) begin
    status_cnt <= 0;
  // Read but no write.
  end else if (flush) begin 
    status_cnt <= 0;
  end else if ((rd_en) && !(wr_en) 
                && (status_cnt != 0)) begin
    status_cnt <= status_cnt - 1;
  // Write but no read.
  end else if ((wr_en) && !(rd_en) 
               && (status_cnt != RAM_DEPTH)) begin
    status_cnt <= status_cnt + 1;
  end
end 
   
regf_dp_d0w_d1r #(DATA_WIDTH,ADDR_WIDTH)
   DP_REGF (
	    .clk(clk),
	    .address_0 (wr_pointer) , // address_0 input 
	    .data_0    (data_in)    , // data_0 input
	    .we_0      (wr_en)      , // write enable
	    .address_1 (rd_pointer) , // address_q input
	    .data_1    (data_ram)   , // data_1 output
	    .rd_1      (rd_en)        // write enable
);     

endmodule
