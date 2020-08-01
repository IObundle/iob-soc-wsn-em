   txrx txrx0 (
	     .clk(clk),
	     .rst(reset),
             
             //cpu interface 
	     .valid(slaves_req[`valid(`TXRX)]),
             .address(slaves_req[`address(`TXRX,`TXRX_ADDR_W+2)-2]),
	     .wdata(slaves_req[`wdata(`TXRX)-24]),
             .wstrb(|slaves_req[`wstrb(`TXRX)]),
	     .ready(slaves_resp[`ready(`TXRX)]),
             
             //serial i/f
	     .tx_valid(tx_valid),
	     .tx(tx),
	     .rx(rx)
            );

