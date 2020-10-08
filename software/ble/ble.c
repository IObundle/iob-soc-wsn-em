#include "system.h"
#include "periphs.h"

#include "iob-uart.h"

#include "ble.h"

//
// Macros
//

// Clock period in us
#define CLK_PER (1e6)/(32.0e6)

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
    iref_on();

    mixer_on();
    limiter_on();
    pa_on();

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

char ble_receive(char *buffer) {
  char nbytes = -1;

  if (on == RX) {
    nbytes = 0;

    if (rx_crc_valid()) {
      while (!rx_empty()) {
        buffer[nbytes++] = receive();
      }
    }

    rx_start();
  }

  return nbytes;
}

char ble_send(char *buffer, char size) {
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
