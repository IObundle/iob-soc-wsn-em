`define TXRX_ADDR_W 4

`define TX_DATA 0 //write
`define TX_START 1 //write
`define TX_EN 2 //write
`define TX_READY 3 //read

`define TXRX_AA 4 //write
`define TXRX_CH_IDX 5 //write

`define RX_START 6 //write
`define RX_EN 7 //write
`define RX_AA_FOUND 8 //read
`define RX_CRC_VALID 9 //read
`define RX_EMPTY 10 //read
`define RX_DATA 11 //read
`define DEMOD_EN 12 //write

`define CH_IDX_W 6

`define PREAMBLE_MSB0 8'b01010101
`define PREAMBLE_MSB1 8'b10101010

`define ACCESSADDRESS 32'h8E89BED6 //advertising channel AA 

// firs

`define C_W 2


`define I1_W  2
`define O1_W  6
`define L1  32 
`define L1_W  5

`define O2_W  11
`define L2  16
`define L2_W  4

`define O3_W  15
`define L3  16
`define L3_W  4

// slicer 
`define T  45  //threshhold
`define H  10  //histeresis

