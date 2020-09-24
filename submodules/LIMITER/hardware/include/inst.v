   limiter limiter0
     (
	  .clk(clk),
	  .rst(reset),

      // CPU interface
	  .valid(slaves_req[`valid(`LIMITER)]),
      .address(slaves_req[`address(`LIMITER,`LIMITER_ADDR_W+2)-2]),
	  .wdata(slaves_req[`wdata(`LIMITER)]),
      .wstrb(|slaves_req[`wstrb(`LIMITER)]),
	  .ready(slaves_resp[`ready(`LIMITER)]),

      // Serial i/f
	  .pd(limiter_pd)
      );

