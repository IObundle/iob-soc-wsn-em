//#include "stdlib.h"
#include "system.h"
#include "periphs.h"

#include "iob-uart.h"
#include "iob_timer.h"

#include "ble.h"

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;

  // Init Timer
  timer_init(TIMER_BASE);

  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);   
  uart_printf("\n\n\nHello world!\n\n\n");

  // Init BLE
  ble_init();

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

  adpll_on();

  // Wait for results
  while(adpll_lock() != 1);

  while(timer_time_us(TIMER_BASE) < SIM_TIME);
  adpll_off();

  // Configure BLE for send data
  //ble_send_on();

  // Send data
  //ble_send(buffer, size);

  // Configure BLE for receive data
  //ble_recv_on();

  // Receive data
  //nbytes = ble_receive(buffer);

  //read current timer count, compute elapsed time
  elapsed  = timer_get_count(TIMER_BASE);
  elapsedu = timer_time_us(TIMER_BASE);

  uart_printf("\nExecution time: %d clocks in %dus @%dMHz\n\n",
              (unsigned int)elapsed, elapsedu, FREQ/1000000);

  return 0;
}
