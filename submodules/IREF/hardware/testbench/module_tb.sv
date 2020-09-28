`timescale 1ns / 1ps

///////////////////////////////////////////////////////////////////////////////
// Date: 29/02/2020
// Module: iref.sv
// Project: WSN
// Description: emulates the current refernece generator
//				 


module iref_tb ( 
             input pd, 
             input charge
             );

// iref_charge default value is high, but shoud be low a little bit after iref_pd
// goes low
   always @*
      if (pd)
	    $display("$WARNING: RF IREF power mode is on!");
      else
        $display("$WARNING: RF IREF power mode is off!");

   always @*
      if (charge)
	    $display("$WARNING: RF IREF is in charged!");
      else
        $display("$WARNING: RF IREF is out of charge!");
   
endmodule
