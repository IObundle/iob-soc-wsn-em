//#include "stdlib.h"
#include "system.h"
#include "periphs.h"

#include "iob-uart.h"

#include "adpll.h"

int main() {
  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);   
  uart_printf("\n\n\nHello world!\n\n\n");

  // Init ADPLL
  adpll_init(ADPLL_BASE);

  // Configure ADPLL
  int mode = ADPLL_OPERATION;
  int fcw = (int)(FREQ_CHANNEL*16384);
  uart_printf("freq_channel = %fMHz, FCW = %d, adpll_mode = %d\n", FREQ_CHANNEL, fcw, ADPLL_OPERATION);

  char alpha_l = 14;
  char alpha_m = 8;
  char alpha_s_rx = 7;
  char alpha_s_tx = 4;
  char beta = 0;
  char lambda_rx = 2;
  char lambda_tx = 2;
  char iir_n_rx = 3;
  char iir_n_tx = 2;
  char FCW_mod = 0b01001; // 288kHz
  char dco_c_l_word_test = 0;
  char dco_c_m_word_test = 0;
  char dco_c_s_word_test = 0;
  char dco_pd_test = 1;
  char tdc_pd_test = 1;
  char tdc_pd_inj_test = 1;
  char tdc_ctr_freq = 0b100;
  char dco_osc_gain = 0b10;

  adpll_config(fcw, mode,
               alpha_l, alpha_m, alpha_s_rx, alpha_s_tx,
               beta,
               lambda_rx, lambda_tx,
               iir_n_rx, iir_n_tx,
               FCW_mod,
               dco_c_l_word_test, dco_c_m_word_test, dco_c_s_word_test,
               dco_pd_test, dco_osc_gain,
               tdc_pd_test, tdc_pd_inj_test, tdc_ctr_freq);

  adpll_soft_rst();
  adpll_enable();

  // Wait for results
  unsigned int i;
  unsigned int k = 0;
  unsigned int end = SIM_TIME;
  for (i = 0; i < end; i++) {
    if (i == end*k/100) {
      uart_printf("Loading progress: %d percent\n", k);
      k += 10;
    }
  }
  uart_printf("Loading progress: %d percent\n", 100);

  return 0;
}
