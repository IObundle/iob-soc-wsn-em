#include "system.h"
#include "periphs.h"

#include "iob-uart.h"

#include "ble.h"

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
void ble_recv_on(void) {

  iref_on();

  mixer_on();
  limiter_on();
  pa_on();

  adpll_set_mode(RX);
  adpll_on();
  while(!adpll_lock());

  return;
}

//
// Send
//

//IREF

//all off
//ADPLL em TX

//when locked, PA On (16 clock cycles, 0.5us)

//Send data
void ble_send_on(void) {

  ble_off();

  iref_on();

  adpll_set_mode(TX);
  adpll_on();
  while(!adpll_lock());

  pa_on();
  
  return;
}

void ble_off(void) {

  iref_off();
  mixer_off();
  limiter_off();
  pa_off();
  adpll_off();

  return;
}
