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
  ble_config(FREQ_CHANNEL, TX);

  adpll_on();

  // Wait for results
  while(adpll_lock() != 1);

  while(timer_time_us() < SIM_TIME);
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
  elapsed  = timer_get_count();
  elapsedu = timer_time_us();

  uart_printf("\nExecution time: %d clocks in %dus @%dMHz\n\n",
              (unsigned int)elapsed, elapsedu, FREQ/1000000);

  return 0;
}
