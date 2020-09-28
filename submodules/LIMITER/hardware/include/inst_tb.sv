
   wire limiter_out;
   wire limiter_in = lpf_out;
   assign rx = limiter_out;
   limiter_tb limiter_tb0
     (
      .signal_in  (limiter_in),
      .square_out (limiter_out),
      .limiter_pd (limiter_pd)
      );

