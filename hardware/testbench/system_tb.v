`timescale 1ns / 1ps

`include "system.vh"

module system_tb;

   parameter realtime clk_per = 1s/`FREQ;

   // clock
   reg clk = 1;
   always #(clk_per/2) clk = ~clk;

   // reset
   reg reset = 1;

   wire antena0;
   wire antena1;
   
    
   /////////////////////////////////////////////
   // TEST PROCEDURE
   //
   initial begin
 
`ifdef VCD
      $dumpfile("system.vcd");
      //$dumpvars(2,soc0, soc1, soc0.uut.txrx0, soc1.uut.txrx0);
      $dumpvars(2, soc0.uut.txrx0, soc0.uut.txrx0.fsk_demod0, soc0.uut.txrx0.rx0, soc0.uut.int_mem0, soc0.uut.cpu);
      //$dumpvars();
`endif

      // deassert rst
      repeat (100) @(posedge clk) #1;
      reset <= 0;

      //wait an arbitray (10) number of cycles 
      repeat (10) @(posedge clk) #1;        
   end

   //
   // INSTANTIATE COMPONENTS
   //

   // cpu trap signals
   wire                    trap0;
   wire                    trap1;

   wire                    finish0;
   wire                    finish1;

   //temperature sensor adc output - temporary setting - to be removed once the ADC is connected
   reg [9:0] adc_out_0;
   reg [9:0] adc_out_1;  
   initial begin   	 
   	adc_out_0 = $urandom % 'h50;
   	adc_out_1 = $urandom % 'h50;
   	$display ("At time=%0t, adc_out_0=0x%0h and adc_out_1=0x%0h", $time, adc_out_0, adc_out_1);  
   end

   //
   // UNITS UNDER TEST
   //
   soc_tb #(
            .ID(0)
            )
   soc0 (
         .clk        (clk),
         .reset      (reset),
	 .adc_out    (adc_out_0),		
         .antena_in  (antena1),
         .antena_out (antena0),
         .trap       (trap0),
         .finish     (finish0)
         );

   soc_tb #(
            .ID(1)
            )
   soc1 (
         .clk        (clk),
         .reset      (reset),	
	 .adc_out    (adc_out_1), 	
         .antena_in  (antena0),
         .antena_out (antena1),
         .trap       (trap1),
         .finish     (finish1)
         );

   // finish simulation
   always @(posedge trap0) begin
      #10 $display("Found CPU trap condition in SoC 0");
      $finish;
   end

   always @(posedge trap1) begin
      #10 $display("Found CPU trap condition in SoC 1");
      $finish;
   end

   always @(posedge (finish0 & finish1)) begin
      #10 $display("End simulation!");
      $finish;
   end

endmodule
