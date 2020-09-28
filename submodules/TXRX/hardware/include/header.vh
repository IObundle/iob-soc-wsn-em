`define TXRX_ADDR_W 4

`define TX_DATA      (`TXRX_ADDR_W'd0)  // write
`define TX_START     (`TXRX_ADDR_W'd1)  // write
`define TX_EN        (`TXRX_ADDR_W'd2)  // write
`define TX_READY     (`TXRX_ADDR_W'd3)  // read

`define TXRX_AA      (`TXRX_ADDR_W'd4)  // write
`define TXRX_CH_IDX  (`TXRX_ADDR_W'd5)  // write

`define RX_START     (`TXRX_ADDR_W'd6)  // write
`define RX_EN        (`TXRX_ADDR_W'd7)  // write
`define RX_AA_FOUND  (`TXRX_ADDR_W'd8)  // read
`define RX_CRC_VALID (`TXRX_ADDR_W'd9)  // read
`define RX_EMPTY     (`TXRX_ADDR_W'd10) // read
`define RX_DATA      (`TXRX_ADDR_W'd11) // read
`define DEMOD_EN     (`TXRX_ADDR_W'd12) // write

`define CH_IDX_W 6

`define PREAMBLE_MSB0 8'b01010101
`define PREAMBLE_MSB1 8'b10101010

`define ACCESSADDRESS 32'h8E89BED6 // advertising channel AA
