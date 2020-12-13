`timescale 1fs / 1fs

module mixer_tb 
  (
   input       rf_in, 
   input       osc_in, 
   output real out, 
   output reg  sampling_clk,
   input       ota, 
   input [1:0] buff, 
   input       pd 
   );

   always @*
      if( buff != 2'b01 && ~pd)
	$display("$WARNING: Mixer buffer gain is not the default value!");
   always @*
      if( ota != 1'b1 && ~pd)
	$display("$WARNING: Mixer OTA gain is not the default value!");

		  
   parameter real                        Channel_BW = 1e6;
   parameter real 			 Noise_floor_dBm_Hz = -174.0; //same as in the noise_floor.txt
   parameter real 			 Fs = 100.0e6;// Output signal sampling frequency

   parameter real 			 pi = 3.14159;
   parameter real 			 Ts = 1.0e15/Fs; // Clock sampling period in fs
   parameter real 			 SNR = 10**(`SNR_DB/10.0);
   parameter real 			 Power_noise_channel_dBm = Noise_floor_dBm_Hz+10*$log10(Channel_BW);
   parameter real 			 Power_signal_dBm = Power_noise_channel_dBm + `SNR_DB; 			 
   parameter real 			 Power_signal = 10**((Power_signal_dBm-30)/10);
   parameter real 			 Ampl = $sqrt(Power_signal * 2 * 50);

   parameter real 			 offset = 0; // dc output signal
			 
   real     phase_inst, phase_last; 
   real     out_noise;
   real     intermediate_freq;
      
   integer  scan,fp_noise,print;
   real     noise_floor;

   initial sampling_clk = 0;
   
   always sampling_clk = #(Ts/2) ~sampling_clk;
   
   
   //--------------------- Average RF frequency Calculations------------------------------
   real     rf_in_time, rf_in_time_last;
   real     rf_in_period, rf_in_period_accum;
   real     rf_in_cycles, rf_accum_rst;
   real     rf_in_period_avg, rf_in_freq_avg;

   assign rf_in_period = rf_in_time - rf_in_time_last;
   always @(posedge rf_in && ~pd)begin
      rf_in_time <= $time;
      rf_in_time_last <= rf_in_time;
      if(rf_accum_rst)begin
	 rf_in_period_accum <= rf_in_period;
	 rf_in_cycles <= 1;
	 rf_accum_rst <= 0;
      end
      else begin		  
	 rf_in_period_accum <= rf_in_period_accum + rf_in_period;
	 rf_in_cycles <= rf_in_cycles + 1;
      end
   end 
   initial rf_in_period_avg = 408163; // initial rf freq is 2450 MHz
   always @(posedge sampling_clk && ~pd)begin
      rf_in_period_avg <= rf_in_period_accum / rf_in_cycles;
      rf_accum_rst <= 1;
   end
   
   
   assign rf_in_freq_avg = 1e15/rf_in_period_avg;
   
   //--------------------- Average OSC frequency Calculations------------------------------
   real     osc_in_time, osc_in_time_last;
   real     osc_in_period, osc_in_period_accum;
   real     osc_in_cycles, osc_accum_rst;
   real     osc_in_period_avg, osc_in_freq_avg;

 
   assign osc_in_period = osc_in_time - osc_in_time_last;
   always @(posedge osc_in && ~pd)begin
      osc_in_time <= $time;
      osc_in_time_last <= osc_in_time;
      if(osc_accum_rst)begin
	 osc_in_period_accum <= osc_in_period;
	 osc_in_cycles <= 1;
	 osc_accum_rst <= 0;
      end
      else begin		  
	 osc_in_period_accum <= osc_in_period_accum + osc_in_period;
	 osc_in_cycles <= osc_in_cycles + 1;
      end
   end 
   
   //initial osc_in_period_avg = 408330; // initial osc freq is 2449 MHz
   always @(posedge sampling_clk && ~pd)begin
      osc_in_period_avg <= osc_in_period_accum / osc_in_cycles;
      osc_accum_rst <= 1;
   end
   
   
   assign osc_in_freq_avg = 1e15/osc_in_period_avg;
   
   //--------------------- Intermediate Frequency Calculation --------------------------------
   // Intermediate frequency is the difference between rf_in and osc_in frequencies  
   assign intermediate_freq = (~pd && (rf_in_freq_avg-osc_in_freq_avg)>0) ? 
			      rf_in_freq_avg-osc_in_freq_avg : 100e3;
   
   //-------------------------------- FSK Modulation------------------------------------------
   // Angular phase is the integral of the frequency over the sampling clk period,
   //  plus the last angular phase
   assign phase_inst = phase_last + (intermediate_freq/Fs)*2*pi;

   initial phase_last = 0;
   always @(posedge sampling_clk) phase_last <= phase_inst;


   //assign freq = F_carrier + (F_dev * gf_real); 
   

   
    //-------------------------------- Noise Add -----------------------------------------------
   
   always @(posedge sampling_clk) begin
      scan = $fscanf(fp_noise,"%e ",noise_floor); 
      if (scan == -1)begin
         if (!print) begin
            $display("Run out of Nnoise points from \"noise_floor.txt\""); //re-use points?
            $display("$WARNING: The Nnoise points will be re-used!");
            print = 1;
         end
         $rewind(fp_noise);
         scan = $fscanf(fp_noise,"%e ",noise_floor);
      end
		out_noise <=  offset + Ampl * $cos(phase_inst) + noise_floor;
   end

   assign out = out_noise;
      
   
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

      print = 0;
   end
   
endmodule
