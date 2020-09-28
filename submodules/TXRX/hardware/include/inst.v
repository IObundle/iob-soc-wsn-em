   txrx txrx0 (
               .clk(clk),
               .rst(reset),

               // CPU interface
               .valid(slaves_req[`valid(`TXRX)]),
               .address(slaves_req[`address(`TXRX,`TXRX_ADDR_W+2)-2]),
               .wdata(slaves_req[`wdata(`TXRX)]),
               .wstrb(|slaves_req[`wstrb(`TXRX)]),
               .rdata(slaves_resp[`rdata(`TXRX)]),
               .ready(slaves_resp[`ready(`TXRX)]),

               // Serial i/f
               .tx_valid(tx_valid),
               .tx(tx),
               .rx(rx)
               );

