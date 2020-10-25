
   real lpf_out;
   real lpf_in;
   wire lpf_sampling_clk = mix_sampling_clk;
   assign lpf_in = mix_out;
   lpf_tb lpf_tb0
     (
      .sampling_clk (lpf_sampling_clk),
      .in           (lpf_in),
      .out          (lpf_out),
      .fc           (lpf_fc),
      .pd           (lpf_pd)
      );

