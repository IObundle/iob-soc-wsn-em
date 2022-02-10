#include "iob-uart.h"
#include "iob_timer.h"

#include "id.h"
#include "ble.h"

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;
  uint32_t pdu_size_min=PDU_H_LEN;
  uint32_t pdu_size_min_tmp=PDU_H_LEN+CRC_LEN;		   			
  uint32_t pdu_size_max=PDU_H_LEN+MAX_PAYLOAD_LEN; 	// Max PDU size
  char     *buffer_tx=NULL, *buffer_rx=NULL; 					
  uint32_t j=0;					  
  uint32_t start_time; 
  
  // By default, SN is in TX mode and ready to send a connectable advertising packet
  sn_t sn = {.id = SN_ID, .aa_init = 0x8E89BED6, .nextState = MODE_SN_TX_ADV_IND};    	
  
  // By default, BS is in RX mode, free and can receive a connectable advertising packet
  bs_t bs = {.id = BS_ID, .aa_init = 0x8E89BED6, .nextState = MODE_BS_RX_ADV_IND};
  
  pdu_adv_h_t pdu_adv_ind_h = {
       .pdu_type=0,	      //4 bits
       .rfu_1=0, 	      //2 bits
       .tx_addr= 0,	      //1 bit
       .rx_addr= 0,	      //1 bit
       .length=4,	      //6 bits
       .rfu_2=0  	      //2 bits
  };
   
  // Init ID
  id_init(ID_BASE);
  
  // Init BLE
  ble_init();
  
  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);
  uart_printf("\n\n\nWelcome to WSN-EM!\n\n\n");
  
  while (j<2) { // will be replaced by while(1) to run with non-stop - the position of this loop might be changed 
  
  // Init Timer
  timer_init(TIMER_BASE);
  				
  if(get_id()==sn.id) { // SN
      switch(sn.nextState) {
	case MODE_SN_TX_ADV_IND:	   	      
	      // Configure ADPLL and BLE for the current data transmission	  
  	      config_tx(FREQ_CHANNEL);   // Frequency to be changed	      

  	      buffer_tx = (char*)malloc(pdu_size_min * sizeof(char));
	      
	      //Initialize buffer_tx
	      for (int i = 0; i < pdu_size_min; i++) {
	       	     buffer_tx[i] = 0;
  	      }
	      	      
   	      // Build PDU	      	      
	       buffer_tx[0] = (pdu_adv_ind_h.pdu_type << 4) | (pdu_adv_ind_h.rfu_1 << 2) | (pdu_adv_ind_h.tx_addr << 1) | pdu_adv_ind_h.rx_addr;
	       buffer_tx[1] = (pdu_adv_ind_h.length << 2) | pdu_adv_ind_h.rfu_2; 
	      
	      // Write PDU to HW	
  	      ble_send(buffer_tx, pdu_size_min);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      ble_off();

  	      // Print SN TX buffer for debugging purpose
  	      uart_printf("\n%d - SN TX sent:\n", j);
  	      for (int i = 0; i < pdu_size_min; i++) {
  		  uart_printf("buffer[%d] = %x\n", i, buffer_tx[i]);
  	      }	          	     
	      
	      free(buffer_tx);
	      sn.nextState=MODE_SN_RX_CONNECT_REQ; bs.nextState=0;  	      	       	      
  	      break;
	      
	case MODE_SN_RX_CONNECT_REQ:
	      // Configure payload size, configure ADPLL and configure BLE for data reception
	      config_rx(pdu_size_min, FREQ_CHANNEL);

  	      // Wait for transmission
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)13000); 

  	      // Receive data
	      buffer_rx = (char*)malloc(pdu_size_min_tmp * sizeof(char));
  	      for (int i = 0; i < pdu_size_min_tmp; i++) {
	             buffer_rx[i] = 0;
  	      }
  	      char nbytes = ble_receive(buffer_rx, pdu_size_min_tmp);

  	      ble_off();

  	      // Wait for sender
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      // Print SN RX buffer for debugging purpose
  	      uart_printf("\n%d - SN RX received %d bytes:\n", j, nbytes);
  	      for (int i = 0; i < pdu_size_min_tmp; i++) {
		  uart_printf("buffer[%d] = %x\n", i, buffer_rx[i]);
  	      } 
	      	      
	      free(buffer_rx);
	      sn.nextState=0; bs.nextState=0;	
  	      break;	
 	
	default: sn.nextState=0; bs.nextState=0;
	         break;	     
      }     
    } else if (get_id()==bs.id) {  // BS
     switch(bs.nextState) {
        case MODE_BS_TX_CONNECT_REQ:	   
	      // Configure ADPLL and BLE for data transmission	  
  	      config_tx(FREQ_CHANNEL);   // Frequency to be changed	      	        
	      
	      buffer_tx = (char*)malloc(pdu_size_min * sizeof(char));
  	      // Initialize buffer_tx 
	      for (int i = 0; i < pdu_size_min; i++) {
	             buffer_tx[i] = 0;
  	      }

	      // Build PDU 
  	      buffer_tx[0] = CONNECT_REQ; buffer_tx[1] = ZERO_PAYLOAD;   // ZERO_PAYLOAD used temporarily  

  	      ble_send(buffer_tx, pdu_size_min);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      ble_off();

  	      // Print BS TX buffer for debugging purpose
  	      uart_printf("\n%d - BS TX sent:\n", j);
  	      for (int i = 0; i < pdu_size_min; i++) {
  		  uart_printf("buffer[%d] = %x\n", i, buffer_tx[i]);
  	      }
  	      
	      free(buffer_tx);
	      bs.nextState=0; sn.nextState=0;
	      break;
	      
  	case MODE_BS_RX_ADV_IND:  
	      // Configure payload size, configure ADPLL and configure BLE for data reception
	      config_rx(pdu_size_min, FREQ_CHANNEL);

  	      // Wait for transmission
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)10000);

  	      // Receive data
	      buffer_rx = (char*)malloc(pdu_size_min_tmp * sizeof(char));
  	      for (int i = 0; i < pdu_size_min_tmp; i++) {
	      	     buffer_rx[i] = 0;
  	      }
  	      char nbytes = ble_receive(buffer_rx, pdu_size_min_tmp);

  	      ble_off();

  	      // Wait for sender
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);  

  	      // Print BS RX buffer for debugging purpose
  	      uart_printf("\n%d - BS RX received %d bytes:\n", j, nbytes);
  	      for (int i = 0; i < pdu_size_min_tmp; i++) {
		  uart_printf("buffer[%d] = %x\n", i, buffer_rx[i]);
  	      } 
	      
	      if (nbytes) { // There is data in the rx buffer
    	      	  if ((buffer_rx[0] >> 4)== 0) {
			bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0; 
              	  }
	      }		      	       
  	      free(buffer_rx);
	      break;
	      
	 default: sn.nextState=0; bs.nextState=0;
	          break;      
     } 
  }		  
  
  j++;
  
  //read current timer count, compute elapsed time
  elapsed  = timer_get_count();
  elapsedu = timer_time_us();

  printf_("\nExecution time: %d clocks in %dus @%dMHz\n\n",
          (unsigned int)elapsed, elapsedu, FREQ/1000000);
  
  }
  
  return 0;
}
