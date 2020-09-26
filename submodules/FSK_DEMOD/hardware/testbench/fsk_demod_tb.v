/*
 * Unitary test bench for TX  and RX with fsk demodulator and baseband blocks
 */

`timescale 1ns / 1ps

module fsk_demod_tb;
   //blocks enable and rst
   reg demod_en;
   reg rx_en;
   reg tx_en;
   reg txrx_rst;
   reg en; 
   
   integer n_packet_valid;
   
   wire txdata_out;	
   wire [7:0] 		rxdata_out;

   reg 			clk;
   reg 			rst;
   reg 			wr_en;
   reg 			rd_en;
   reg [`CHIDXW-1:0] 	ch_idx;
   
   reg 			rxstart;
   reg 			txstart;
   reg [7:0] 		txdata_in;
   wire 		txready;
   wire 		tx_valid;
   wire 		empty;
   reg [31:0] 		aa;
   wire 	 crc_valid;
  
   parameter clk_per= (10**9)/(32.0*10**6);

   parameter PACKET_N = 1<<`N_PCKTS_W;
   parameter BYTE_N = `N_PCKT_BYTE;
   parameter PACKET_GAP = 32*4; //packet clk cycles interval
   parameter real packet_gap_wait = (PACKET_GAP-(3+BYTE_N))*clk_per;
   
  
   integer 		i, j;

   
   tx tx (
	  // Outputs
	  .tx_valid                    (tx_valid),
	  .tx		        	(txdata_out),
	  .txready			(txready),
	  // Inputs
	  .rst				(rst | txrx_rst),
	  .clk				(clk),
	  .en                           (en & tx_en),
	  .wr_en			(wr_en),
	  .ch_idx			(ch_idx),
	  .txdata_in			(txdata_in),
	  .txstart			(txstart),
	  .aa(aa));

   wire [8:0] 		gf_out; //signed Q1.8 signal		
   parameter  SIZE = 19;
   // instantiate GF module
   gf #(.SIZE(SIZE)) GF0 (
			  .rst(rst), 
			  .clk(clk),
			  .clk_en(en),
			  .x({~txdata_out,1'b1}), //signed input //bit sequence represented as signed 1->01 and 0->11
			  .y(gf_out) //signed output Q1.8
			  );
   
   real 		fsk_mod_noise_out;
   wire 		sampling_clk; // 100 MHz sampling clk	  
   // instantiate fsk modulation module
   fsk_mod_noise fsk_mod_noise(	  
				  // Inputs
				  .gf_out(gf_out),
				  //Outputs
				  .signal_out(fsk_mod_noise_out),
				  .sampling_clk(sampling_clk));

   reg 			lpf_pd;
   reg [7:0] 		lpf_fc;
   real 		lpf_out;
   // instantiate the rx channel filtering
   lpf_tb lpf(
		      // Inputs
		      .sampling_clk(sampling_clk),
		      .pd(lpf_pd),
		      .fc(lpf_fc),
		      .in(fsk_mod_noise_out),
		      //Outputs
		      .out(lpf_out));

   
   reg 			limiter_pd;
   wire 		limiter_out;   
   // instantiate the limiter analog block
   limiter_tb limiter(
				      // Inputs
				      .signal_in(lpf_out),
				      .limiter_pd(limiter_pd),
				      //Outputs
				      .square_out(limiter_out));
   


   wire 		rxdata_in;
   wire 		rxdata_in_valid;
   // instantiate device under test
   fsk_demod fsk_demod ( 
                       .clk(clk),
                       .rst(rst | rxstart),
		       .en(en & demod_en),
                       .data_in(limiter_out),
                       .data_out(rxdata_in),
                       .data_out_valid(rxdata_in_valid)
                       );

   rx rx (
	  .rst				(rst | txrx_rst),
	  .clk				(clk),
	  .en                           (en & rx_en),
	  .start		(rxstart),
	  // fifo interface 
	  .data_out			(rxdata_out),
	  .empty 		        (empty),
	  .rd_en			(rd_en),
	  // Inputs
	  .rx			(rxdata_in),
	  .rx_valid		(rxdata_in_valid),
	  .ch_idx                      (ch_idx),
	  .aa                          (aa),
	  .crc_valid                          (crc_valid)
	  );
   

   //just issue clock cyles and a start pulse with fixed data at input
   initial begin
      $dumpfile("fsk_demod_tb.vcd");
      $dumpvars();

      clk = 1;
      rst = 1;
      en = 1;

      /// Analog blocks digital control ///
      lpf_pd = 0;
      lpf_fc = 8'h0F;
      limiter_pd = 0;

      tx_en = 1;
      rx_en = 1;
      demod_en = 1;
      txrx_rst = 0;
      wr_en = 0;
      rd_en = 0;
      rxstart = 0;
      txstart = 0;
      txdata_in = 8'h22;
      ch_idx = `CHIDXW'd3;
      aa = 32'hEEDDCCBB;
      n_packet_valid = 0;
      

      #(clk_per) rst = 0;
      #(clk_per/2);
      
      for(i=0; i < PACKET_N; i=i+1) begin
	 #(clk_per) wr_en = 1; //write tx fifo
	 for(j=0; j < BYTE_N-1; j=j+1)
	    #(clk_per) txdata_in = $urandom%256;
	 #(clk_per) wr_en = 0;
	 rd_en = 1; //read rx fifo
	 #packet_gap_wait; rd_en = 0;
	 txstart=1;rxstart=1;
	 #(clk_per) txstart=0; rxstart=0;
	 
	 #(100*clk_per) // not needed
	 wait(txready);
	 #(clk_per/2);
	 $display("Packet %d of %d sent", i+1, PACKET_N);
      end 
     #(5*32*clk_per) begin
	$display("PER = %.2f \%", 100 - n_packet_valid *100.0 / PACKET_N);
	$finish;
     end
    end 
      
   always
     #(clk_per/2) clk = ~clk;

   always @(posedge crc_valid)
     n_packet_valid = n_packet_valid + 1;
   
 
endmodule 
