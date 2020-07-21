   lpf lpf0 (
	     .clk(clk),
	     .rst(reset),
             
             //cpu interface 
	     .valid(slaves_req[`valid(`LPF)]),
             .address(slaves_req[`address(`LPF,`LPF_ADDR_W+2)-2]),
	     .wdata(slaves_req[`wdata(`LPF)]),
             .wstrb(|slaves_req[`wstrb(`LPF)]),
	     .ready(slaves_resp[`ready(`LPF)]),
             
             //serial i/f
	     .pd(lpf_pd),
	     .fc(lpf_fc)
            );

