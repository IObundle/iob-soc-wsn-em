   lna lna0 (
	     .clk(clk),
	     .rst(reset),
             
             //cpu interface 
	     .valid(slaves_req[`valid(`LNA)]),
             .address(slaves_req[`address(`LNA,`LNA_ADDR_W+2)-2]),
	     .wdata(slaves_req[`wdata(`LNA)]),
             .wstrb(|slaves_req[`wstrb(`LNA)]),
	     .rdata(slaves_resp[`rdata(`LNA)]),
	     .ready(slaves_resp[`ready(`LNA)]),
             
             //serial i/f
	     .pd(lna_pd),
	     .mode(lna_mode)
            );

