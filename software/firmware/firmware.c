//#include "stdlib.h"
#include "system.h"
#include "periphs.h"

#include "iob-uart.h"
#include "iob_timer.h"

#include "id.h"

#include "ble.h"

#define N_BYTES 4

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;

  int i;
  int size = N_BYTES; // Payload
  char buffer[N_BYTES+3]; // Payload + CRC

  // Init ID
  id_init(ID_BASE);

  // Init Timer
  timer_init(TIMER_BASE);

  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);
  uart_printf("\n\n\nHello world!\n\n\n");

  // Init BLE
  ble_init();

#if (BLE_MODE == RX)
  if (get_id()) { // Sender
#else // BLE_MODE == TX
  if (!get_id()) { // Sender
#endif
    // Configure ADPLL
    ble_config(FREQ_CHANNEL, ADPLL_OPERATION);

    // Configure BLE for send data
    ble_send_on();

    // Send data
    for (i = 0; i < size; i++) {
      buffer[i] = 2*(i+1);
    }

    ble_send(buffer, size);

    // Wait for transmisstion
    unsigned int start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);

    ble_off();

    // Print buffer
    uart_printf("\nsend:\n");
    for (i = 0; i < size; i++) {
      uart_printf("buffer[%d] = %d\n", i, buffer[i]);
    }

  } else { // Receiver
    // Payload
    ble_payload(size);

    // Configure ADPLL
    ble_config((FREQ_CHANNEL-1.0F), ADPLL_OPERATION);

    // Configure BLE for receive data
    ble_recv_on();

    // Wait for transmission
    unsigned int start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)10000);

    // Receive data
    size = N_BYTES+3;
    for (i = 0; i < size; i++) {
      buffer[i] = 0;
    }
    char nbytes = ble_receive(buffer, size);

    ble_off();

    // Wait for sender
    start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);

    // Print buffer
    uart_printf("\nreceived %d bytes:\n", nbytes);
    for (i = 0; i < size; i++) {
      uart_printf("buffer[%d] = %d\n", i, buffer[i]);
    }
  }

  //read current timer count, compute elapsed time
  elapsed  = timer_get_count();
  elapsedu = timer_time_us();

  uart_printf("\nExecution time: %d clocks in %dus @%dMHz\n\n",
              (unsigned int)elapsed, elapsedu, FREQ/1000000);

  return 0;
}
