
`timescale 1ns/1ps

module lna (
	    input             clk,
	    input             rst,

            //cpu interface 
	    input             valid,
            input [2:0]       address,
	    input [31:0]      wdata,
            input             wstrb,
	    output reg [31:0] rdata,
	    output reg        ready,

            //serial i/f
	    output reg        pd,
	    input reg [2:0]   mode
            );


   always @(posedge clk, posedge rst)
     if(rst) begin          
        pd <= 1'b0;
        mode <= 3'b0;
     end else if(s_valid[`LNA]) begin
        if(m_addr[0] == `LNA_PD)
          pd <= m_wdata[0];
        else
          mode <= m_wdata[2:0];
        ready <= 1'b1;
     end else 
       ready <= 1'b0;
 
endmodule
