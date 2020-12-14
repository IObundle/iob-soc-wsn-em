   parameter RAMCODE = "firmware.hex";
   reg [31:0] ram [Words-1:0];
   reg [31:0] tmp;
   integer i, fp;
   initial begin
      //fp = $fopen(RAMCODE, "r");
      $readmemh(RAMCODE, ram, 0, (Words-1));
      for (i = 0; i < Words; i=i+1) begin
         /*if ($fscanf(fp, "%x\n", tmp) == 0) begin
           $finish;
         end*/
         tmp = ram[i];
         Memory_byte0[i] = tmp[7:0];
         Memory_byte1[i] = tmp[15:8];
         Memory_byte2[i] = tmp[23:16];
         Memory_byte3[i] = tmp[31:24];
      end
      //$fclose(fp);
   end
