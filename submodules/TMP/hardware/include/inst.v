
   tmp tmp0 (
       	   //General interface signals
    	   .clk(clk),
    	   .rst(reset),

    	   //CPU interface signals	    
    	   .valid(slaves_req[`valid(`TMP)]),
    	   .address(slaves_req[`address(`TMP,`TMP_ADDR_W+2)-2]),
    	   .rdata(slaves_resp[`rdata(`TMP)]),
    	   .ready(slaves_resp[`ready(`TMP)]),

    	   //ADC output signal 
    	   .data_in(adc_out)
	   );	    
