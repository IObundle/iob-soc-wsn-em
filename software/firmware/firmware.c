#include "system.h"
#include "periphs.h"
#include "iob-uart.h"
#include "iob_timer.h"
#include "id.h"
#include "ble.h"
#include "def.h"

int main() {
  unsigned long long elapsed;
  unsigned int elapsedu;	   			
  char     *buffer_tx=NULL, *buffer_rx=NULL; 					
  uint32_t pdu_size=0, start_time, j=0, k=0;					  
  
  // By default, SN is in TX mode and ready to send a connectable advertising packet
  sn_s_t sn = {.id = SN_ID, .aa_init = 0x8E89BED6, .nextState = MODE_SN_TX_ADV_IND};    	
  
  // By default, BS is in RX mode, free and can receive a connectable advertising packet
  bs_s_t bs = {.id = BS_ID, .aa_init = 0x8E89BED6, .nextState = MODE_BS_RX_ADV_IND, .busyState=0};
  
  pdu_adv_h_s_t pdu_adv_ind_h = {
       .PDU_Type = ADV_IND,        
       .RFU_1=0, 	           
       .TxAdd=0,	           
       .RxAdd=0,	           
       .Length=4,	           
       .RFU_2=0  	           
  };

  adv_ind_payload_s_t adv_ind_payload = {
       .AdvA=0x8C87BAE5D45B,       	//value temporarily assigned for debug purpose      	           
       .AdvData=7	           	//value temporarily assigned for debug purpose
  };
  
  pdu_adv_h_s_t pdu_connect_req_h = {
       .PDU_Type = CONNECT_REQ,	   
       .RFU_1=0, 	      	   
       .TxAdd=0,	           
       .RxAdd=0,	           
       .Length=4,	           
       .RFU_2=0  	           
  };
 
  connect_req_payload_s_t connect_req_payload = {
       .InitA=0,		 
       .AdvA=0,
       .LLData={ 		   
       		 .AA=0,   
		 .CRCInit=0, 
		 .WinSize=0, 
		 .WinOffset=0,
		 .Interval=0, 
		 .Latency=0, 
		 .Timeout=0,
		 .ChM=0,  
		 .Hop=0,  
		 .SCA=0  
       }		     
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
  					
  if(get_id()==sn.id) {
      switch(sn.nextState) {
	case MODE_SN_TX_ADV_IND:	   	      
	      // Configure ADPLL and BLE for the current data transmission	  
  	      config_tx(FREQ_CHANNEL);   // Frequency to be changed	      

  	      pdu_size = PDU_H_LEN + ADV_IND_L_LEN;
	      buffer_tx = (char*)malloc(pdu_size * sizeof(char));
	      
	      // Initialize buffer_tx
	      for (int i = 0; i < pdu_size; i++) {
	       	     buffer_tx[i] = 0;
  	      }
	      	      
   	      // Build PDU	     	      
	      buffer_tx[0] = (pdu_adv_ind_h.PDU_Type << 4) | (pdu_adv_ind_h.RFU_1 << 2) | (pdu_adv_ind_h.TxAdd << 1) | pdu_adv_ind_h.RxAdd;
	      buffer_tx[1] = (pdu_adv_ind_h.Length << 2) | pdu_adv_ind_h.RFU_2; 
	      for (int i=PDU_H_LEN + get_size(1,1) - 1; i >= PDU_H_LEN; i--) {
	      		buffer_tx[i] = ((adv_ind_payload.AdvA >> 8*k++) & 0xff);
	      }   
	      k=0;    	     
	      for (int i=pdu_size-1; i >= PDU_H_LEN + get_size(1,1); i--) {
	      		buffer_tx[i] = ((adv_ind_payload.AdvData >> 8*k++) & 0xff);
	      }  
	      k=0;     	   
	      	      
	      // Write PDU to HW
  	      ble_send(buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      ble_off();

  	      // Print SN TX buffer for debugging purpose
  	      uart_printf("\n%d - SN TX sent:\n", j);
  	      for (int i = 0; i < pdu_size; i++) {
  		  uart_printf("buffer[%d] = %x\n", i, buffer_tx[i]);
  	      }	          	     
	      
	      free(buffer_tx); pdu_size=0;
	      sn.nextState=MODE_SN_RX_CONNECT_REQ; bs.nextState=0;  	      	       	      
  	      break;
	      
	case MODE_SN_RX_CONNECT_REQ:
	      pdu_size = PDU_H_LEN;
	      // Configure payload size, configure ADPLL and configure BLE for data reception
	      config_rx(pdu_size, FREQ_CHANNEL);

  	      // Wait for transmission
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)13000);

  	      // Receive data
	      pdu_size += CRC_LEN;
	      buffer_rx = (char*)malloc(pdu_size * sizeof(char));
  	      for (int i = 0; i < pdu_size; i++) {
	             buffer_rx[i] = 0;
  	      }
  	      char nbytes = ble_receive(buffer_rx, pdu_size);

  	      ble_off();

  	      // Wait for sender
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      // Print SN RX buffer for debugging purpose
  	      uart_printf("\n%d - SN RX received %d bytes:\n", j, nbytes);
  	      for (int i = 0; i < pdu_size; i++) {
		  uart_printf("buffer[%d] = %x\n", i, buffer_rx[i]);
  	      } 
	      	      
	      free(buffer_rx); pdu_size=0;
	      sn.nextState=0; bs.nextState=0;	
  	      break;	
 	
	default: sn.nextState=0; bs.nextState=0; break;	         	     
      }     
    } else if (get_id()==bs.id) {
     switch(bs.nextState) {
        case MODE_BS_TX_CONNECT_REQ:	   
	      // Configure ADPLL and BLE for data transmission	  
  	      config_tx(FREQ_CHANNEL);   // Frequency to be changed	      	        
	      
	      pdu_size = PDU_H_LEN;  //temporarily setting
	      buffer_tx = (char*)malloc(pdu_size * sizeof(char));
  	      // Initialize buffer_tx 
	      for (int i = 0; i < pdu_size; i++) {
	             buffer_tx[i] = 0;
  	      }

	      // Build PDU  
	      buffer_tx[0] = (pdu_connect_req_h.PDU_Type << 4) | (pdu_connect_req_h.RFU_1 << 2) | (pdu_connect_req_h.TxAdd << 1) | pdu_connect_req_h.RxAdd;
	      buffer_tx[1] = (pdu_connect_req_h.Length << 2) | pdu_connect_req_h.RFU_2; 
	      	      
	      ble_send(buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      ble_off();

  	      // Print BS TX buffer for debugging purpose
  	      uart_printf("\n%d - BS TX sent:\n", j);
  	      for (int i = 0; i < pdu_size; i++) {
  		  uart_printf("buffer[%d] = %x\n", i, buffer_tx[i]);
  	      }
  	      
	      free(buffer_tx); pdu_size=0;
	      bs.nextState=0; sn.nextState=0;
	      break;
	      
  	case MODE_BS_RX_ADV_IND:  
	      // Configure payload size, configure ADPLL and configure BLE for data reception
	      pdu_size = PDU_H_LEN + ADV_IND_L_LEN;
	      config_rx(pdu_size, FREQ_CHANNEL);

  	      // Wait for transmission
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)10000);

  	      // Receive data
	      pdu_size += CRC_LEN;
	      buffer_rx = (char*)malloc(pdu_size * sizeof(char));
  	      for (int i = 0; i < pdu_size; i++) {
	      	     buffer_rx[i] = 0;
  	      }
  	      char nbytes = ble_receive(buffer_rx, pdu_size);

  	      ble_off();

  	      // Wait for sender
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);  

  	      // Print BS RX buffer for debugging purpose
  	      uart_printf("\n%d - BS RX received %d bytes:\n", j, nbytes);
  	      for (int i = 0; i < pdu_size; i++) {
		  uart_printf("buffer[%d] = %x\n", i, buffer_rx[i]);
  	      } 
	      
	      if (nbytes) { // There is data in the rx buffer
    	      	  if ((buffer_rx[0] >> 4)== 0) {
			bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0; 
              	  }
	      }		      	       
  	      free(buffer_rx); pdu_size=0;
	      break;
	      
	 default: sn.nextState=0; bs.nextState=0; break; 	               
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
