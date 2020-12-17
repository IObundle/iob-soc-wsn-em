`timescale 1ns / 1ps

module fsk_demod (
                      input  clk,
                      input  rst,
                      input  en,
                      input  data_in,
                      output data_out,
                      output data_out_valid
                      );


   //input sync
   reg [9:0]                 data_in_reg;
   always@(posedge clk, posedge rst)
     if(rst)
       data_in_reg <= 10'b0;
     else if(en)
       data_in_reg <= {data_in_reg[8:0], data_in};


   wire signed [1:0]         x = (data_in_reg[1]^data_in_reg[9])? 2'b01: 2'b11;
//   wire signed [1:0]         x = (data_in_reg[1]? 2'b01: 2'b11;
   
   //
   //1st filter
   //

   wire signed [`O1_W-1:0]   f1_out;
   iob_fir #(
             .DATA_IN_W(`I1_W),
             .DATA_OUT_W(`O1_W),
             .COEFF_W(`C1_W),
             .COEFF_FILE("fir1coeffs.v"),
             .LENGTH(`L1),
             .LENGTH_W(`L1_W)
             )
   fir1 (
         .clk(clk),
         .rst(rst),
         .data_in(x),
         .data_out(f1_out)
         );


   //
   //2nd filter
   //


   wire signed [`O2_W-1:0] f2_out;
   iob_fir #(
             .DATA_IN_W(`O1_W),
             .DATA_OUT_W(`O2_W),
             .COEFF_W(`C2_W),
             .COEFF_FILE("fir2coeffs.v"),
             .LENGTH(`L2),
             .LENGTH_W(`L2_W)
             )
   fir2 (
         .clk(clk),
         .rst(rst),
         .data_in(f1_out),
         .data_out(f2_out)
         );

   //
   //3rd filter
   //

   wire signed [`O3_W-1:0] f3_out;
   iob_fir #(
             .DATA_IN_W(`O2_W),
             .DATA_OUT_W(`O3_W),
             .COEFF_W(`C3_W),
             .COEFF_FILE("fir3coeffs.v"),
             .LENGTH(`L3),
             .LENGTH_W(`L3_W)
             )
   fir3 (
         .clk(clk),
         .rst(rst),
         .data_in(f2_out),
         .data_out(f3_out)
         );
 

   
   //demodulated signal
   wire signed [`O3_W-1:0] y = f3_out;

   //
   //slicer
   //
   reg                     demod_bit;
   
   always@(posedge clk, posedge rst)
     if(rst)
       demod_bit <= 1'b0;
     else if(en)
       if(y > `T+`H)
         demod_bit <= 1'b1;
       else if(y < `T-`H)
         demod_bit <= 1'b0;


   
   //
   // demodulated signal edge detector
   //
   
   reg                 demod_bit_reg;
   wire                demod_edge_detected = demod_bit_reg ^ demod_bit;
   always@(posedge clk, posedge rst)
     if(rst)
       demod_bit_reg <= 1'b0;
     else if (en)
       demod_bit_reg <= demod_bit;
   
   //sample strobe
   reg [4:0]           sample_cnt;
   wire                sample_strb = (sample_cnt == 5'd14);
   reg                 restart;

   //sample counter 
   always@(posedge clk, posedge rst)
     if(rst)
       sample_cnt <= 5'd0;
     else if(en)
       if(demod_edge_detected)
         sample_cnt <= 5'd0;
       else          
         sample_cnt <= sample_cnt+1'b1;


   //sample and register data bit 
   reg                 sampled_bit, sampled_bit_reg;
   
   always@(posedge clk, posedge rst)
     if(rst) begin
        sampled_bit <= 1'b0;
        sampled_bit_reg <= 1'b0;
     end else if(en)
       if(sample_strb) begin 
          sampled_bit <= demod_bit;
          sampled_bit_reg <= sampled_bit;
       end

   //find preamble
   reg [2:0]           bit_cnt;
   reg                 valid_en;
   
   always@(posedge clk, posedge rst)
     if(rst) begin
        bit_cnt <= 3'd0;
        valid_en<= 1'b0;
     end else if (en) 
       if (sample_strb)
         if(bit_cnt == 3'd4)
           valid_en <= 1;
         else if (sampled_bit != sampled_bit_reg) //next preamble bit found
           bit_cnt <= bit_cnt+1'b1;
         else //failed to detect preamble
           bit_cnt <= 3'b0;
     
   assign data_out = demod_bit;
   assign data_out_valid = sample_strb;
   
endmodule

