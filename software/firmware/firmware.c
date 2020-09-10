//#include "stdlib.h"
#include "system.h"
#include "periphs.h"

#include "iob-uart.h"

#include "adpll.h"

int main() {
  // init uart
  uart_init(UART_BASE,FREQ/BAUD);   
  uart_printf("\n\n\nHello world!\n\n\n");

  //char *a = malloc(10);
  //free(a);

  // init adpll
  adpll_init(ADPLL_BASE);

  int adpll_mode = ADPLL_OPERATION;
  int fcw = (int)(FREQ_CHANNEL*16384);
  uart_printf("freq_channel = %d.%d MHz, FCW = %d, adpll_mode = %d\n", (int)FREQ_CHANNEL,(int)((FREQ_CHANNEL-(int)FREQ_CHANNEL)*1000), fcw, ADPLL_OPERATION);

  // Configure ADPLL
  char alpha_l = 14;
  char alpha_m = 8;
  char alpha_s_rx = 7;
  char alpha_s_tx = 4;
  char beta = 0;
  char lambda_rx = 2;
  char lambda_tx = 2;
  char iir_n_rx = 3;
  char iir_n_tx = 2;
  char FCW_mod = 0b01001;//298kHz
  char dco_c_l_word_test = 0;
  char dco_c_m_word_test = 0;
  char dco_c_s_word_test = 0;
  char dco_pd_test = 1;
  char tdc_pd_test = 1;
  char tdc_pd_inj_test = 1;

  adpll_config(alpha_l, alpha_m, alpha_s_rx, alpha_s_tx,
               beta,
               lambda_rx, lambda_tx,
               iir_n_rx, iir_n_tx,
               FCW_mod,
               dco_c_l_word_test, dco_c_m_word_test, dco_c_s_word_test,
               dco_pd_test, tdc_pd_test, tdc_pd_inj_test);
}
