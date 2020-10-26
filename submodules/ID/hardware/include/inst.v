   id #(
        .ID(ID)
        )
   id0 (
	    .clk(clk),
	    .rst(reset),

        // CPU interface 
	    .valid(slaves_req[`valid(`ID)]),
        .address(slaves_req[`address(`ID,`ID_ADDR_W+2)-2]),
        .wstrb(|slaves_req[`wstrb(`ID)]),
        .rdata(slaves_resp[`rdata(`ID)]),
	    .ready(slaves_resp[`ready(`ID)])
        );
