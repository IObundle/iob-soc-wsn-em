   pa pa0 (
	     .clk(clk),
	     .rst(reset),
             
             //cpu interface 
	     .valid(slaves_req[`valid(`PA)]),
             .address(slaves_req[`address(`PA,`PA_ADDR_W+2)-2]),
	     .wdata(slaves_req[`wdata(`PA)]),
             .wstrb(|slaves_req[`wstrb(`PA)]),
	     .ready(slaves_resp[`ready(`PA)]),
             
             //serial i/f
	     .pd(pa_pd),
	     .mode(pa_mode)
            );

