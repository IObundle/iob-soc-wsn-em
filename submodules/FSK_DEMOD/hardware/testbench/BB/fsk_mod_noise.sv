`timescale 1ns / 1ps

`include "rcntlr_defines.vh"

///////////////////////////////////////////////////////////////////////////////
// Date: 28/02/2020
// Module: fsk_mod_noise.sv
// Project: WSN
// Description: FSK modulator (input signal is signed Q1.8) followed by a
//              noise floor adder. SNR control is  available
//				 

module fsk_mod_noise(input [8:0] gf_out, output real signal_out, output sampling_clk);

   ///// converts input Q1.8 to real (double-precision in C)
   bit [7:0] gf_unsigned;
   real     gf_real;
   assign gf_unsigned = gf_out[8] ? (~gf_out)+1'b1: gf_out;
   assign gf_real = gf_out[8] ? -1*((1/256.0)*gf_unsigned) : (1/256.0)*gf_unsigned;

		  
   parameter real                        Channel_BW = 1e6;
   parameter real 			 Noise_floor_dBm_Hz = -174.0; //same as in the noise_floor.txt
   parameter real 			 Fs = 100.0e6;// Output signal sampling frequency

   parameter real 			 F_dev = 350e3; //Frequency deviation in Hz
   parameter real 			 F_carrier = 1e6; // Carrier frequency
   parameter real 			 pi = 3.14159;
   parameter real 			 Ts = 1.0e9/Fs; // Clock sampling period in ns
   parameter real 			 SNR = 10**(`SNR_DB/10.0);
   parameter real 			 Power_noise_channel_dBm = Noise_floor_dBm_Hz+10*$log10(Channel_BW);
   parameter real 			 Power_signal_dBm = Power_noise_channel_dBm + `SNR_DB; 			 
   parameter real 			 Power_signal = 10**((Power_signal_dBm-30)/10);
   parameter real 			 Ampl = $sqrt(Power_signal * 2 * 50);

   parameter real 			 offset = 0; // dc output signal
			 
   reg 	    sampling_clk;
   real     freq;
   real     phase_inst, phase_last; 
   real     signal_out_noise;
   integer  scan,fp_noise;
   real     noise_floor;

   initial sampling_clk = 0;
   always sampling_clk = #(Ts/2) ~sampling_clk;
   
   
   //-------------------------------- FSK Modulation------------------------------------------
   // angular phase is the integral of the frequency over the sampling clk period, plus the last angular phase
   assign phase_inst = phase_last + (freq/Fs)*2*pi;
   
   always @(posedge sampling_clk) phase_last <= phase_inst;

   assign freq = F_carrier + (F_dev * gf_real); 
   
   //assign signal_out_no_noise = (gf_out == 9'b0) ? 0 : offset + (Ampl * $cos(phase_inst)); // output sine is zero during packet interval
   //assign signal_out_no_noise =offset + (Ampl * $cos(phase_inst));
   
    //-------------------------------- Noise Add -----------------------------------------------
   ////// DEBUG SIGNAL
   /* integer fp;
   initial begin
      fp = $fopen("sine_IF","w");
   end
   always @(posedge sampling_clk) $fwrite(fp, "%e \n ", signal_out);
   */
   
   always @(posedge sampling_clk) begin
      scan = $fscanf(fp_noise,"%e ",noise_floor); 
      if (scan == -1)begin
         $display("Run out of Nnoise points from \"noise_floor.txt\""); //re-use points?
	 $fclose(fp_noise);
	 $finish;
      end
		signal_out_noise <=  offset + Ampl * $cos(phase_inst) + noise_floor;
   end

   assign signal_out = signal_out_noise;
      
   
   initial begin
      // opens noise data source
     // $display("Power_noise_channel_dBm = %d",Power_noise_channel_dBm);
      $display("SNR_dB = %.2f",`SNR_DB);
     // $display("Power_signal_dBm = %f",Power_signal_dBm);
     // $display("Ampl = %f",Ampl);
      
      fp_noise = $fopen("noise_floor.txt","r");
      if (!fp_noise)begin
	 $display("Could not open \"noise_floor.txt\"");
	 $fclose(fp_noise);
	 $finish;
      end
   end
   
endmodule
