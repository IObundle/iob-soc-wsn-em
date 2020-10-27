`timescale 1ns / 1ps

`include "system.vh"

module system_tb;

   parameter realtime clk_per = 1s/`FREQ;

   // clock
   reg clk = 1;
   always #(clk_per/2) clk = ~clk;

   // reset
   reg reset = 1;

   wire antena;

   /////////////////////////////////////////////
   // TEST PROCEDURE
   //
   initial begin

`ifdef VCD
      $dumpfile("system.vcd");
      $dumpvars(3,system_tb);
`endif

      // deassert rst
      repeat (100) @(posedge clk);
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

   //
   // UNITS UNDER TEST
   //
   soc_tb #(
            .ID(0)
            )
   soc0 (
         .clk    (clk),
         .reset  (reset),
         .antena (antena),
         .trap   (trap0)
         );

   soc_tb #(
            .ID(1)
            )
   soc1 (
         .clk    (clk),
         .reset  (reset),
         .antena (antena),
         .trap   (trap1)
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

endmodule
