#include "system.h"
#include "periphs.h"

#include "iob-uart.h"
#include "iob_timer.h"
#include "printf.h"

#include "adpll.h"
#include "pa.h"
#include "mixer.h"
#include "lpf.h"
#include "limiter.h"
#include "txrx.h"
#include "iref.h"

#include "ble.h"

//
// Macros
//

// Clock period in us
#define CLK_PER (1e6)/FREQ

// Packet paylod bytes 
#define N_PCKT_BYTE 8

// Packet clock cycles interval
#define PACKET_GAP 32*4

// Interval between packets
#define PACKET_GAP_WAIT ((PACKET_GAP-(3+N_PCKT_BYTE))*CLK_PER)

//
// Global variables
//

char init = 0;
char on = 0;

//
// Functions
//

void ble_init(void) {

  // Init ADPLL
  adpll_init(ADPLL_BASE);

  // Init PA
  pa_init(PA_BASE);

  // Init Mixer
  mixer_init(MIXER_BASE);

  // Init LPF
  lpf_init(LPF_BASE);

  // Init Limiter
  limiter_init(LIMITER_BASE);

  // Init TXRX
  txrx_init(TXRX_BASE);

  // Init Iref
  iref_init(IREF_BASE);

  init = 1;

  return;
}

char ble_config(float channel_freq, int mode) {

  if (init) {
    int fcw = (int)(channel_freq*16384);
    printf_("freq_channel = %fMHz, FCW = %d, adpll_mode = %d\n", channel_freq, fcw, mode);

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
  }

  return init;
}

void ble_payload(char nbytes) {
  payload(nbytes);
  return;
}

//
// Receive
//

//IREF

//mixer
//lpf
//limiter
//PA off

//ADPLL em RX

//wait for locked

//Receive data
char ble_recv_on(void) {

  if (init) {
    ble_off();

    iref_on();

    mixer_on();
    lpf_on();
    limiter_on();

    adpll_set_mode(RX);
    adpll_on();
    while(!adpll_lock());

    rx_on();

    on = RX;
  }

  return on;
}

//
// Send
//

//IREF

//all off
//ADPLL em TX

//when locked, PA On (16 clock cycles, 0.5us)

//Send data
char ble_send_on(void) {

  if (init) {
    ble_off();

    iref_on();

    adpll_set_mode(TX);
    adpll_on();
    while(!adpll_lock());

    pa_on();

    tx_on();

    on = TX;
  }

  return on;
}

char ble_off(void) {
  char ret = -1;

  if (init) {
    iref_off();
    mixer_off();
    lpf_off();
    limiter_off();
    pa_off();
    adpll_off();
    txrx_off();

    on = 0;

    ret = 0;
  }

  return ret;
}

char ble_receive(char buffer[], unsigned char size) {
  char nbytes = -1;

  if (on == RX) {
    nbytes = 0;

    if (rx_crc_valid()) {
      if (nbytes < size) {
        while (!rx_empty()) {
          buffer[nbytes++] = receive();
        }
      }
    }

    rx_start();
  }

  return nbytes;
}

char ble_send(char buffer[], unsigned char size) {
  char nbytes = -1;

  if (on == TX) {
    nbytes = 0;

    if (tx_ready()) {
      while (nbytes != size) {
        send(buffer[nbytes++]);
      }

      tx_start();
    }
  }

  return nbytes;
}
