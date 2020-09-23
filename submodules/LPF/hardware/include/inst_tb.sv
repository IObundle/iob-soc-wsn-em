
   wire lpf_out;
   wire lpf_in = mix_out;
   wire lpf_sampling_clk = mix_sampling_clk;
   lpf_tb lpf_tb0
     (
      .sampling_clk (lpf_sampling_clk),
      .in           (lpf_in),
      .out          (lpf_out),
      .fc           (lpf_fc),
      .pd           (lpf_pd)
      );

