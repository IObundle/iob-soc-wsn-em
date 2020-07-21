`timescale 1fs / 1fs

// Description: Emulates the analog channel filtering of the receiver
//		An RC lpf, followed by two biquad lpf cells, and an RC hpf
//              Filters coef are calculated for a 100 MHz sampling_clk
// 

module lpf_tb
  (
   input       sampling_clk, 
   input real  in, 
   output real out, 
   input       pd, 
   input [7:0] fc
   );

   always @*
     if(fc != 8'h0F && ~pd)
       $display("$WARNING: Low-pass filter cut-off frequency is not the default value!");
   
   
   //real lpf2_in, lpf2_in_m1, lpf2_out, lpf2_out_m1;
   real lpf_in, lpf_in_m1, lpf_out, lpf_out_m1;
   real hpf_in, hpf_in_m1, hpf_out, hpf_out_m1;
   real biq1_in, biq1_in_m1, biq1_in_m2, biq1_out, biq1_out_m1, biq1_out_m2;
   real biq2_in, biq2_in_m1, biq2_in_m2, biq2_out, biq2_out_m1, biq2_out_m2;

   //assign lpf2_in = out_noise;
   //assign lpf2_out  = 0.158600138*lpf2_in + 0.158600138*lpf2_in_m1 + 0.682799725*lpf2_out_m1;
   assign lpf_in = in;
   assign lpf_out  = 0.0861302*lpf_in + 0.0861302*lpf_in_m1 + 0.827739601*lpf_out_m1;
   assign hpf_in = lpf_out;
   assign hpf_out  = 0.996026033*hpf_in - 0.996026033*hpf_in_m1 + 0.99205066*hpf_out_m1;
   assign biq1_in = hpf_out;
   assign biq1_out  = 0.357742221*biq1_in - 0.701412415*biq1_in_m1 + 0.357742221*biq1_in_m2 + 1.90533363*biq1_out_m1 - 0.915994257*biq1_out_m2;
   assign biq2_in = biq1_out;
   assign biq2_out  = 0.254405791*biq2_in - 0.49192037*biq2_in_m1 + 0.254405791*biq2_in_m2 + 1.888927481*biq2_out_m1 - 0.901723854*biq2_out_m2;
   assign out = (pd) ? 0 : biq2_out/2; 
   
      
   always @(posedge sampling_clk) begin
      // RC low pass filter with fc = 6MHz
      // lpf2_out_m1 <= lpf2_out;
      // lpf2_in_m1 <= lpf2_in;
      // RC low pass filter with fc = 3MHz
      // y[n] = 0.08613*x[n] + 0.08613*x[n-1] + 0.8277*y[n-1]
      lpf_out_m1 <= lpf_out;
      lpf_in_m1 <= lpf_in;
      // RC high pass filter with fc = 127kHz
      // y[n] = 0.9922*x[n] - 0.9922*x[n-1] + 0.9844*y[n-1]
      hpf_out_m1 <= hpf_out;
      hpf_in_m1 <= hpf_in;
      // Biquad low pass filter 1
      // y[n] = 0.3577*x[n] - 0.7014*x[n-1] + 0.3577*x[n-2] + 1.905*y[n-1] - 0.916*y[n-2]
      biq1_out_m1 <= biq1_out;
      biq1_out_m2 <= biq1_out_m1;
      biq1_in_m1 <= biq1_in;
      biq1_in_m2 <= biq1_in_m1;
      //$display("biq1_out = %e,  biq1_out_m1 = %e, biq1_out_m2 = %e \n", biq1_out,biq1_out_m1,biq1_out_m2);     
      // Biquad low pass filter 2
      // y[n] = 0.2544*x[n] - 0.4919*x[n-1] + 0.2544*x[n-2] + 1.889*y[n-1] - 0.9017*y[n-2]
      biq2_out_m1 <= biq2_out;
      biq2_out_m2 <= biq2_out_m1;
      biq2_in_m1 <= biq2_in;
      biq2_in_m2 <= biq2_in_m1;
      //$display("biq2_out = %e,  biq2_out_m1 = %e, biq2_out_m2 = %e \n", biq2_out,biq2_out_m1,biq2_out_m2);
   end

endmodule
