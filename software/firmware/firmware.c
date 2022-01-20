#include "id.h"
#include "ble.h"

#define N_BYTES 59   // Assumption: Max Payload length = 59

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;

  int i, d=1, sn_connect=0, bs_connect=0;
  int pdu_size=0; // PDU size
  char buffer[PDU_H_LEN+N_BYTES+CRC_LEN]; // PDU + CRC

  // Init ID
  id_init(ID_BASE);

  // Init Timer
  timer_init(TIMER_BASE);

  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);
  uart_printf("\n\n\nHello world!\n\n\n");

  // Init BLE
  ble_init();

  if(d==1) { // will be replaced by another condition
 	sn_connect=1;   // SN Advertising - First time connection
	bs_connect=1;   // BS Scanning - Connect with an SN for the first time
	pdu_size=PDU_H_LEN+AA_LEN; 
  } else { 
  	sn_connect=0;	
	bs_connect=0;
  }

  // Assumptions
  // ID(SN)=0 and ID(BS)=1  
  // CRC is calculated over the PDU including PDU's header and PDU's payload 
#if (MODE == RX)
  if (get_id()) { // Sender
#else // MODE == TX
  if (!get_id()) { // Sender
#endif
    	switch(sn_connect) {
		case 1: // SN Advertising - First time connection 	
		    // Configure ADPLL
		    ble_config(FREQ_CHANNEL, ADPLL_OPERATION);   // Frequency to be changed

		    // Configure BLE for send data
		    ble_send_on();

		    // Build PDU = Header + Payload
		    // PDU's Header
		    buffer[0] = ADV_IND;
		    buffer[1] = AA_LEN; 
		    
		    // PDU's Payload
	            for (i = 2; i < pdu_size+2 ; i++) {  // will be replaced by AA
	    	    	buffer[i] = 2*(i+1);
		    }

		    ble_send(buffer, pdu_size);

		    // Wait for transmisstion
		    unsigned int start_time = timer_time_us();
		    while ((timer_time_us() - start_time) < (unsigned int)1000);

		    ble_off();

		    // Print buffer
		    uart_printf("\nsend:\n");
		    for (i = 0; i < pdu_size; i++) {
	            	uart_printf("buffer[%d] = %d\n", i, buffer[i]);
		    }
		    break;
		default: printf("SN default"); // will be changed	    
	}
  } else { // Receiver
    	switch(bs_connect) { 
    		case 1: // BS Scanning - Connect with an SN for the first time
		    // Payload
    		    ble_payload(pdu_size);

    		    // Configure ADPLL
       		    ble_config((FREQ_CHANNEL-1.0F), ADPLL_OPERATION);

    		    // Configure BLE for receive data
    		    ble_recv_on();

    		    // Wait for transmission
    		    unsigned int start_time = timer_time_us();
    		    while ((timer_time_us() - start_time) < (unsigned int)10000);

    		    // Receive data
    		    pdu_size += CRC_LEN;
    		    for (i = 0; i < pdu_size; i++) {
      			buffer[i] = 0;
    		    }
    		    char nbytes = ble_receive(buffer, pdu_size);

    		    ble_off();

    		    // Wait for sender
    		    start_time = timer_time_us();
    		    while ((timer_time_us() - start_time) < (unsigned int)1000);

    		    // Print buffer
    		    uart_printf("\nreceived %d bytes:\n", nbytes);
    		    for (i = 0; i < pdu_size; i++) {
      			uart_printf("buffer[%d] = %d\n", i, buffer[i]);
    		    }
		    break;
		default: printf("BS default"); // will be changed    
	}	
  }

  //read current timer count, compute elapsed time
  elapsed  = timer_get_count();
  elapsedu = timer_time_us();

  printf_("\nExecution time: %d clocks in %dus @%dMHz\n\n",
          (unsigned int)elapsed, elapsedu, FREQ/1000000);

  return 0;
}
