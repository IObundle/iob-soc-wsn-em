`timescale 1ns / 1ps

//filter select
`define F2
`define F3
`define F4

//filter 1
`define I1_W 2
//`define C1_W 2
//`define L1_W 5

//filter 2 from Makefile 

//filter 3 from Makefile 

//threshold T and histeresis H for decision block from Makefile

`define SAMPLE_TIME 15


module fsk_demod_alt (
                      input clk,
                      input rst,
                      input data_in,
                      output data_out,
                      output data_out_valid
                      );


   //input sync
   reg [9:0]                 data_in_reg;
   always@(posedge clk, posedge rst)
     if(rst)
       data_in_reg <= 10'b0;
     else
       data_in_reg <= {data_in_reg[8:0], data_in};

   //xor demodulator
   wire signed [1:0]         x = (data_in_reg[1]^data_in_reg[9])? 2'b01: 2'b11;
   
   //
   //1st filter
   //
   wire signed [`I1_W+`C1_W-1+`L1_W-1:0] f1_out;
   iob_fir #(
             .DATA_IN_W(`I1_W),
             .COEFF_W(`C1_W),
             .COEFF_FILE("fir1.hex"),
             .LENGTH_W(`L1_W)
             )
   fir1 (
         .clk(clk),
         .rst(rst),
         .data_in(x),
         .data_out(f1_out)
         );

`ifdef F2

   //
   //2nd filter
   //

 `define I2_W  (`I1_W+`C1_W-1+`L1_W)
// `define I2_W 2
   wire signed [`I2_W+`C2_W-1+`L2_W-1:0] f2_out;
   iob_fir #(
             .DATA_IN_W(`I2_W),
             .COEFF_W(`C2_W),
             .COEFF_FILE("fir2.hex"),
             .LENGTH_W(`L2_W)
             )
   fir2 (
         .clk(clk),
         .rst(rst),
         .data_in(f1_out),
         .data_out(f2_out)
         );
`endif 


`ifdef F3
   //
   //3rd filter
   //
 `define I3_W  (`I2_W+`C2_W-1+`L2_W)
   wire signed [`I3_W+`C3_W-1+`L3_W-1:0] f3_out;
   iob_fir #(
             .DATA_IN_W(`I3_W),
             .COEFF_W(`C3_W),
             .COEFF_FILE("fir3.hex"),
             .LENGTH_W(`L3_W)
             )
   fir3 (
         .clk(clk),
         .rst(rst),
         .data_in(f2_out),
         .data_out(f3_out)
         );
 
`endif


`ifdef F4
   //
   //3rd filter
   //
 `define I4_W 2
// `define I4_W  (`I3_W+`C3_W-1+`L3_W)
   wire signed [`I4_W+`C4_W-1+`L4_W-1:0] f4_out;
   iob_fir #(
             .DATA_IN_W(`I4_W),
             .COEFF_W(`C4_W),
             .COEFF_FILE("fir4.hex"),
             .LENGTH_W(`L4_W)
             )
   fir4 (
         .clk(clk),
         .rst(rst),
         .data_in(f3_out >= 0? 2'b01: 2'b11),
         .data_out(f4_out)
         );
 
`endif

  /* 
   //demodulated signal and sign and sign register
`ifdef F4
   wire signed [`I4_W+`C4_W-1+`L4_W-1:0] y = f4_out;
   wire                                  y_sign = y[`I4_W+`C4_W-1+`L4_W-1];
   */
`ifdef F3
   wire signed [`I3_W+`C3_W-1+`L3_W-1:0] y = f3_out;
   wire                                  y_sign = y[`I3_W+`C3_W-1+`L3_W-1];
   
`elsif F2
   wire signed [`I2_W+`C2_W-1+`L2_W-1:0] y = f2_out;
   wire                                  y_sign = y[`I2_W+`C2_W-1+`L2_W-1];
`else
   wire signed [`I1_W+`C1_W-1+`L1_W-1:0] y = f1_out;
   wire                                  y_sign = y[`I1_W+`C1_W-1+`L1_W-1];
`endif

/*          
   //compute input DC value
   wire signed [`I3_W+`C3_W-1+`L3_W-1:0]        input_DC = (data_in? 2**(`I3_W+`C3_W-1+`L3_W)-1: -2**(`I3_W+`C3_W-1+`L3_W)+1) - 2**(`I3_W+`C3_W-1+`L3_W-1);
   reg signed [`I3_W+`C3_W-1+`L3_W-1:0]         input_DC_av_reg;
   wire signed [`I3_W+`C3_W-1+`L3_W-1:0]        input_DC_av = (input_DC >>> 17) + input_DC_av_reg - (input_DC_av_reg >>> 17) + 50;
   //wire signed [`I3_W+`C3_W-1+`L3_W-1:0]        input_DC_av = 18'd45;


   reg                                          start_DC_analysis;
   
   always@(posedge clk, posedge rst)
     if(rst) begin
       input_DC_av_reg <= 0;
        start_DC_analysis <= 0;
     end else if(x)
        start_DC_analysis <= 1'b1;
     else if(start_DC_analysis)
       input_DC_av_reg <= input_DC_av;
*/
   
   //determine sampling instant
   reg signed [`I4_W+`C4_W-1+`L4_W-1:0] f4_out_reg;
   always@(posedge clk, posedge rst)
     if(rst)
        f4_out_reg <= 0;
     else
        f4_out_reg <= f4_out;

   



   
   //sample counter
   reg [4:0]           sample_cnt;
   wire sample_cnt_set = (f4_out == 4 || f4_out == -4) & ~valid_en;
   
   
   always@(posedge clk, posedge rst)
     if(rst)
       sample_cnt <= 5'd0;
     else if(sample_cnt_set)
       sample_cnt <= 5'd0;
     else          
       sample_cnt <= sample_cnt+1'b1;

   //sample strobe
   wire                sample_strb = (sample_cnt == 5'd`SAMPLE_TIME);


   //sample and register data bit 
   reg                 sampled_bit, sampled_bit_reg;
   
   always@(posedge clk, posedge rst)
     if(rst) begin
        sampled_bit <= 1'b0;
        sampled_bit_reg <= 1'b0;
     end else if(sample_strb) begin 
       sampled_bit <= (f3_out >= 0);
       sampled_bit_reg <= sampled_bit;
     end

   //find preamble
   reg [2:0]           bit_cnt;
   reg                 valid_en;
   
   always@(posedge clk, posedge rst)
     if(rst) begin
        bit_cnt <= 3'd0;
        valid_en<= 1'b0;
     end else if (sample_strb)
       if(bit_cnt == 3'd4)
         valid_en <= 1;
       else if (sampled_bit != sampled_bit_reg) //next preamble bit found
         bit_cnt <= bit_cnt+1'b1;
       else //failed to detect preamble
         bit_cnt <= 3'b0;
  
   assign data_out = sampled_bit;
   assign data_out_valid = sample_strb & valid_en;
   
endmodule

