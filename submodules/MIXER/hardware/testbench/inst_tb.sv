
   mixer_tb mixer_tb0
     (
      .rf_in        (mix_rf_in_tb),
      .osc_in       (mix_osc_in_tb),
      .out          (mix_out_tb),
      .sampling_clk (mix_sampling_clk_tb),
      .ota          (mix_ota_tb),
      .buff         (mix_buff_tb),
      .pd           (mix_pd_tb)
      );

