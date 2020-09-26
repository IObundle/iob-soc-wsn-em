`timescale 1fs / 1fs

// Description: Emulates the limiter behaviour
// 

module limiter_tb
  (
   input real signal_in,
   output bit square_out,
   input      limiter_pd
   );

   parameter offset = 0;

   //-------------------------------- Signal Limiting---- -------------------------------
   assign square_out = limiter_pd ? 0 : ((signal_in > offset)? 1 : 0);

endmodule
