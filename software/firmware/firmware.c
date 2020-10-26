//#include "stdlib.h"
#include "system.h"
#include "periphs.h"

#include "iob-uart.h"
#include "iob_timer.h"

#include "id.h"

#include "ble.h"

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;

  int i, size = 8;
  char buffer[8];

  // Init ID
  id_init(ID_BASE);

  // Init Timer
  timer_init(TIMER_BASE);

  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);   
  uart_printf("\n\n\nHello world!\n\n\n");

  // Init BLE
  ble_init();

  // Configure ADPLL
  ble_config(FREQ_CHANNEL, ADPLL_OPERATION);

  if (!get_id()) {
    // Configure BLE for send data
    ble_send_on();

    // Send data
    for (i = 0; i < size; i++) {
      buffer[i] = 2*(i+1);
    }

    // Print buffer
    uart_printf("\nsend:\n");
    for (i = 0; i < size; i++) {
      uart_printf("buffer[%d] = %d\n", i, buffer[i]);
    }

    ble_send(buffer, size);

    // Wait for transmisstion

  } else {
    // Configure BLE for receive data
    ble_recv_on();

    // Receive data
    for (i = 0; i < size; i++) {
      buffer[i] = 0;
    }
    char nbytes = ble_receive(buffer);

    // Print buffer
    uart_printf("\nreceived %d bytes:\n", nbytes);
    for (i = 0; i < size; i++) {
      uart_printf("buffer[%d] = %d\n", i, buffer[i]);
    }
  }

  ble_off();

  //read current timer count, compute elapsed time
  elapsed  = timer_get_count();
  elapsedu = timer_time_us();

  uart_printf("\nExecution time: %d clocks in %dus @%dMHz\n\n",
              (unsigned int)elapsed, elapsedu, FREQ/1000000);

  return 0;
}
