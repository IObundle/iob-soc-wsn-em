
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
