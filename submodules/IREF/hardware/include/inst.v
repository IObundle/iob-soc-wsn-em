   iref iref0 (
	     .clk(clk),
	     .rst(reset),
             
             //cpu interface 
	     .valid(slaves_req[`valid(`IREF)]),
             .address(slaves_req[`address(`IREF,`IREF_ADDR_W+2)-2]),
	     .wdata(slaves_req[`wdata(`IREF)]),
             .wstrb(|slaves_req[`wstrb(`IREF)]),
	     .ready(slaves_resp[`ready(`IREF)]),
             
             //serial i/f
	     .pd(iref_pd),
	     .charge(iref_charge)
            );

