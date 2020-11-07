   //
   // ADPLL TASKS
   //

   // 1-cycle write
   task adpll_write;
      input [3:0]  adpllAddress;
      input [31:0] adpllDataIn;

      # 1 adpll_address = adpllAddress;
      adpll_valid = 1;
      adpll_wstrb = 1;
      adpll_wdata = adpllDataIn;
      @ (posedge clk) #1 adpll_wstrb = 0;
      adpll_valid = 0;
   endtask //adpll_write

   // 2-cycle read
   task adpll_read;
      input [3:0]   adpllAddress;
      output [31:0] adpllDataOut;

      # 1 adpll_address = adpllAddress;
      adpll_valid = 1;
      @ (posedge clk) #1 adpllDataOut = adpll_rdata;
      @ (posedge clk) #1 adpll_valid = 0;
   endtask //adpll_read
