`timescale 1ns / 1ps

`include "system.vh"

`include "../../../submodules/ADPLL/hardware/include/adpll_defines.vh"

module adpll_tb;
   // ADPLL CPU interface
   wire                     adpll_valid;
   wire [`ADPLL_ADDR_W-1:0] adpll_address;
   wire [`ADPLL_DATA_W-1:0] adpll_wdata;
   wire                     adpll_wstrb;
   wire [1:0]               adpll_rdata;
   wire                     adpll_ready;

   reg [1:0]                rcv_adpll_word;

`include "adpll_tasks.v"

   /////////////////////////////////////////////
   // TEST PROCEDURE
   //
   initial begin
      // init ADPLL cpu bus signals
      adpll_valid = 0;
      adpll_wstrb = 0;
      
      wait (~reset) #1;

      // configure ADPLL
      adpll_config(`FREQ_CHANNEL);

      // enable ADPLL
      adpll_on();

      // check if ADPLL is lock
      do begin
        adpll_read(rcv_adpll_word);
      end while(rcv_adpll_word != 1);

      $finish;
   end

   //
   // INSTANTIATE COMPONENTS
   //

   // Serial input
   wire                     data_mod = tx;

   // Analog DCO interface
   wire                     dco_pd;
   wire [1:0]               dco_osc_gain;
   wire [4:0]               dco_c_l_rall;
   wire [4:0]               dco_c_l_row;
   wire [4:0]               dco_c_l_col;
   wire [15:0]              dco_c_m_rall;
   wire [15:0]              dco_c_m_row;
   wire [15:0]              dco_c_m_col;
   wire [15:0]              dco_c_s_rall;
   wire [15:0]              dco_c_s_row;
   wire [15:0]              dco_c_s_col;
   wire                     dco_ckv;

   // Analog TDC interface
   wire                     tdc_pd;
   wire                     tdc_pd_inj;
   wire [2:0]               tdc_ctr_freq;
   wire [6:0]               tdc_ripple_count;
   wire [15:0]              tdc_phase;
   adpll_ctr adpll_ctr0
     (
      // Serial
      .data_mod(data_mod),

      // Analog DCO interface
      .dco_pd(dco_pd),
      .dco_osc_gain(dco_osc_gain),
      .dco_c_l_rall(dco_c_l_rall),
      .dco_c_l_row(dco_c_l_row),
      .dco_c_l_col(dco_c_l_col),
      .dco_c_m_rall(dco_c_m_rall),
      .dco_c_m_row(dco_c_m_row),
      .dco_c_m_col(dco_c_m_col),
      .dco_c_s_rall(dco_c_s_rall),
      .dco_c_s_row(dco_c_s_row),
      .dco_c_s_col(dco_c_s_col),

      // Analog TDC interface
      .tdc_pd(tdc_pd),
      .tdc_pd_inj(tdc_pd_inj),
      .tdc_ctr_freq(tdc_ctr_freq),
      .tdc_ripple_count(tdc_ripple_count),
      .tdc_phase(tdc_phase),

      // CPU interface
      .clk       (clk),
      .rst       (reset),
      .valid     (adpll_valid),
      .address   (adpll_address),
      .wdata     (adpll_wdata),
      .wstrb     (adpll_wstrb),
      .rdata     (adpll_rdata),
      .ready     (adpll_ready)
      );

   wire                     channel_lock = adpll_ctr0.adpll_ctr0.channel_lock;
   wire [11:0]              tdc_word = adpll_ctr0.adpll_ctr0.tdc_word;
   wire                     en = adpll_ctr0.en;
   adpll_tb #(
              .ID(0)
              )
   adpll_tb0
     (
      .clk (clk),

      // Analog DCO interface
      .dco_pd(dco_pd),
      .dco_osc_gain(dco_osc_gain),
      .dco_c_l_rall(dco_c_l_rall),
      .dco_c_l_row(dco_c_l_row),
      .dco_c_l_col(dco_c_l_col),
      .dco_c_m_rall(dco_c_m_rall),
      .dco_c_m_row(dco_c_m_row),
      .dco_c_m_col(dco_c_m_col),
      .dco_c_s_rall(dco_c_s_rall),
      .dco_c_s_row(dco_c_s_row),
      .dco_c_s_col(dco_c_s_col),
      .dco_ckv(dco_ckv),

      // Analog TDC interface
      .tdc_pd(tdc_pd),
      .tdc_pd_inj(tdc_pd_inj),
      .tdc_ctr_freq(tdc_ctr_freq),
      .tdc_ripple_count(tdc_ripple_count),
      .tdc_phase(tdc_phase),

      // Simulation
      .channel_lock(channel_lock),
      .tdc_word(tdc_word),
      .en(en)
      );

endmodule
