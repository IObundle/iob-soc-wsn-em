   mixer mixer0 (
	     .clk(clk),
	     .rst(reset),
             
             //cpu interface 
	     .valid(slaves_req[`valid(`MIXER)]),
             .address(slaves_req[`address(`MIXER,`MIXER_ADDR_W+2)-2]),
	     .wdata(slaves_req[`wdata(`MIXER)]),
             .wstrb(|slaves_req[`wstrb(`MIXER)]),
	     .ready(slaves_resp[`ready(`MIXER)]),
             
             //serial i/f
	     .pd(mix_pd),
	     .ota(mix_ota),
	     .buff(mix_buff)
            );

