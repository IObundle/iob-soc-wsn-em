/*
 * Description: constant definitions
 * 
 */

//uncomment the following line to disable whitening for verification
//`define WHITE_DIS //disable whitening for testing
`define CH_IDX_W 6 // channel id width
`define CHIDXW 6 // channel id width
`define LOG_FREQ 5 //log of frequency in megahertz - 2**5=32MHz
`define PREAMBLE_MSB0 8'b01010101
`define PREAMBLE_MSB1 8'b10101010
`define ACCESSADDRESS 32'h8E89BED6 //advertising channel AA 


`define Q1_8SZ 9

`define RX_GAIN 1 // if 1 high-gain mode rx
