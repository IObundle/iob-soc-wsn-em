
   wire mix_out;
   wire mix_sampling_clk;
   wire mix_osc_in = dco_ckv;
   wire mix_rf_in = antena;
   mixer_tb mixer_tb0
     (
      .rf_in        (mix_rf_in),
      .osc_in       (mix_osc_in),
      .out          (mix_out),
      .sampling_clk (mix_sampling_clk),
      .ota          (mix_ota),
      .buff         (mix_buff),
      .pd           (mix_pd)
      );

