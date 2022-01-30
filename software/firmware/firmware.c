#include "id.h"
#include "ble.h"

#define N_BYTES 59   // Assumption: Max Payload length = 59

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;

  int sn_tx_connect=0, bs_rx_connect=0;
  int pdu_size_1=PDU_H_LEN+AA_LEN;         	// PDU size - SN Advertising - First time connection
  char buffer[PDU_H_LEN+N_BYTES+CRC_LEN]; 	// PDU + CRC
  unsigned int aa_init=0x8E89BED6;         	// Initial Access Address
   
  //while (1) { // Run with non-stop - to be uncommented, its position might be changed 
  
  // Init ID
  id_init(ID_BASE);

  // Init Timer
  timer_init(TIMER_BASE);

  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);
  uart_printf("\n\n\nHello world!\n\n\n");

  // Init BLE
  ble_init();

  if (get_id()==0) { // SN
	if(!get_status_aa()){ // The initial AA has not been changed
		sn_tx_connect=1;   // Advertising - First time connection		
	} else {
		sn_tx_connect=0;
	}			
  } else if (get_id()==1){ // BS
	if(!get_status_busy()){ // BS is free
		bs_rx_connect=1;   // Scanning
	} else {
		bs_rx_connect=0;
	}	
  }

  // Assumptions
  // ID(SN)=0 and ID(BS)=1  
  // CRC is calculated over the PDU including PDU's header and PDU's payload 
#if (MODE == RX)
  if (get_id()) { // Sender
#else // MODE == TX
  if (!get_id()) { // Sender
#endif
    	switch(sn_tx_connect) {
		case 1: // SN Advertising - First time connection 	
		    // Configure ADPLL
		    ble_config(FREQ_CHANNEL, ADPLL_OPERATION);   // Frequency to be changed

		    // Configure BLE for send data
		    ble_send_on();

		    // Build PDU = Header + Payload
		    // Sending Packet's type only might be enough - to be discussed
		    // PDU's Header
		    buffer[0] = ADV_IND; buffer[1] = AA_LEN; 
		    
		    // PDU's Payload = Initial AA
		    for (int i = pdu_size_1-1 ; i >= 2 ; i--) {  
		        buffer[i] = aa_init & 0xff;
			aa_init >>= 8;
		    }	

		    ble_send(buffer, pdu_size_1);

		    // Wait for transmisstion
		    unsigned int start_time = timer_time_us();
		    while ((timer_time_us() - start_time) < (unsigned int)1000);

		    ble_off();

		    // Print buffer
		    uart_printf("\nsend:\n");
		    for (int i = 0; i < pdu_size_1; i++) {
	            	uart_printf("buffer[%d] = %d\n", i, buffer[i]);
		    }
		    break;
		default: printf("SN default"); // will be changed	    
	}
  } else { // Receiver
    	switch(bs_rx_connect) { 
    		case 1: // BS Scanning - Connect with an SN for the first time
		    // Payload
    		    ble_payload(pdu_size_1);

    		    // Configure ADPLL
       		    ble_config((FREQ_CHANNEL-1.0F), ADPLL_OPERATION);

    		    // Configure BLE for receive data
    		    ble_recv_on();

    		    // Wait for transmission
    		    unsigned int start_time = timer_time_us();
    		    while ((timer_time_us() - start_time) < (unsigned int)10000);

    		    // Receive data
    		    pdu_size_1 += CRC_LEN;
    		    for (int i = 0; i < pdu_size_1; i++) {
      			buffer[i] = 0;
    		    }
    		    char nbytes = ble_receive(buffer, pdu_size_1);

    		    ble_off();

    		    // Wait for sender
    		    start_time = timer_time_us();
    		    while ((timer_time_us() - start_time) < (unsigned int)1000);

    		    // Print buffer
    		    uart_printf("\nreceived %d bytes:\n", nbytes);
    		    for (int i = 0; i < pdu_size_1; i++) {
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

  //} // End of while(1)
  
  return 0;
}
