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
  uint32_t pdu_size=0, start_time=0, j=0, sn_ch_freq=0, bs_ch_freq=0;
  uint16_t sn_adv_ch_idx=37, bs_adv_ch_idx=39, next_adv_ch = 0;		 				  
  
  // By default, SN is in STANDBY mode
  sn_s_t sn = {.id = SN_ID, .aa = 0x8E89BED6, .adv_1 = 1, .nextState = MODE_SN_STANDBY};    	
  
  // By default, BS is in STANDBY mode
  bs_s_t bs = {.id = BS_ID, .aa = 0x8E89BED6, .nextState = MODE_BS_STANDBY, .busyState=0};
  
  // SN TX - Connectable undirected advertising PDU
  adv_ind_pdu_s_t sn_tx_adv_ind_pdu = {
  	.pdu_adv_ind_h = {
       		.PDU_Type = ADV_IND,        
       		.RFU_1  = 0, 	           
       		.TxAdd  = 1,		//temporarily for debugging purpose	           
       		.RxAdd  = 0,	           
       		.Length = 7,		//temporarily for debugging purpose		   
       		.RFU_2  = 1 		//temporarily for debugging purpose		   
        },
	.pdu_adv_ind_payload = {
       		.AdvA = 0,      	          
       		.AdvData = 0
  	}  
  }; 
  
  // BS RX - Connectable undirected advertising PDU
  adv_ind_pdu_s_t bs_rx_adv_ind_pdu = {
  	.pdu_adv_ind_h = {
       		.PDU_Type = 1,		//temporarily for debugging purpose        
       		.RFU_1  = 0, 	           
       		.TxAdd  = 0,	           
       		.RxAdd  = 0,	           
       		.Length = 0,	           
       		.RFU_2  = 0  	           
        },
	.pdu_adv_ind_payload = {
       		.AdvA = 0,           	          
       		.AdvData = 0
	}		           			 
  }; 
  
  // BS TX - Connect request PDU 
  connect_request_pdu_s_t bs_tx_connect_request_pdu = {
  	.pdu_adv_ind_h = { // TxAdd, RxAdd and RFU_2: values assigned temporarily for debugging purpose
       		.PDU_Type = CONNECT_REQ, .RFU_1  = 0, .TxAdd  = 1, .RxAdd  = 1, .Length = 0, .RFU_2  = 1			      
        }, 
	.connect_req_payload = {
       		.InitA=0x2B53E85C74A8,		//temporarily for debugging purpose   
       		.AdvA=0x8C87BAE5D45B,		//temporarily for debugging purpose
       		.LLData={ 		   
       		 	.AA=0,   
		 	.CRCInit=0x555555,
		 	.WinSize=0, 
		 	.WinOffset=0,
		 	.Interval=0, 
		 	.Latency=0, 
		 	.Timeout=0,
		 	.ChM=0,  
		 	.Hop=0,  
		 	.SCA=0  
       		}		     
  	}     
  }; 
  
  // SN RX - Connect request PDU 
  connect_request_pdu_s_t sn_rx_connect_request_pdu = {
  	.pdu_adv_ind_h = { 
       		.PDU_Type = 0, .RFU_1  = 0, .TxAdd  = 0, .RxAdd  = 0, .Length = 0, .RFU_2  = 0			      
        }, 
	.connect_req_payload = {
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
  	}     
  }; 
       
  // Init ID
  id_init(ID_BASE);
  
  // Init BLE
  ble_init();
  
  // Init UART
  uart_init(UART_BASE,FREQ/BAUD);
  uart_printf("\n\n\nWelcome to WSN-EM!\n\n\n");
  
  while (j<7) { // will be replaced by while(1) to run with non-stop
  
  // Init Timer
  timer_init(TIMER_BASE);
  					
  if(get_id()==sn.id) {
      switch(sn.nextState) {
      	case MODE_SN_STANDBY:	     	     	 	     
	     // Turn off BLE
    	     ble_off();	        	     
	     
	     // Delay
  	     start_time = timer_time_us();
  	     while ((timer_time_us() - start_time) < (unsigned int)t_standby);
	     	     
	     // Go to advertising state
	     if (sn.adv_1 == 1) {
	     	sn.nextState=MODE_SN_TX_ADV_IND; bs.nextState=0;
	     }		
	     break; 
	      		
	case MODE_SN_TX_ADV_IND:		      	      	      
	      sn_ch_freq = get_adv_ch_freq(sn_adv_ch_idx);
	      		   	      
	      // Configure ADPLL and BLE for the current data transmission 
  	      config_tx(sn_ch_freq);   	      

  	      pdu_size = PDU_H_LEN + ADV_IND_L_LEN;
	      buffer_tx = (char*)malloc(pdu_size * sizeof(char));
	      
	      // Initialize buffer_tx
	      for (int i = 0; i < pdu_size; i++) {
	       	     buffer_tx[i] = 0;
  	      }
	      		      
	      sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA=0x8C87BAE5D45B,   //temporarily for debugging purpose	
	      sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData=0xA9;	   //temporarily for debugging purpose
	      	      
	      // Build PDU		  	     	      	      	      
	      for (int i=0; i < pdu_size; i++) {	
		   buffer_tx[i] = ((unsigned char *)&sn_tx_adv_ind_pdu)[i];	
	      }
	        	   	      	      
	      // Write PDU to HW
  	      ble_send(buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      ble_off();
      
	      uart_printf("\nSN TX sent: %d bytes/ADV_IND on advertising channel (index, frequency) =(%d, %dMHz)\n", pdu_size, sn_adv_ch_idx, sn_ch_freq);		
	      printf_("\nPDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength= %d\nRFU_2 = %d\nAdvA  = %#"PRIx64"\nAdvData = %#x\n", \
			sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData);      
	      
	      free(buffer_tx); pdu_size=0;
	      sn.nextState=MODE_SN_RX_CONNECT_REQ; bs.nextState=0;  	      	       	      
  	      break;
	      
	case MODE_SN_RX_CONNECT_REQ:
	      pdu_size = PDU_H_LEN;   //temporarily for debugging purpose
	      // Configure payload size, configure ADPLL and configure BLE for data reception
	      config_rx(pdu_size, sn_ch_freq);

  	      // Stay on the advertising channel to listen to a connection request 
	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)advDelay);

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

	      uart_printf("\nSN RX received %d bytes/CONNECT_REQ on advertising channel (index, frequency) =(%d, %dMHz)\n", nbytes, sn_adv_ch_idx, sn_ch_freq);	      
	      if (nbytes == pdu_size) { // There is data in the rx buffer
	      	   for (int i = 0; i < pdu_size - CRC_LEN; i++) {
	       	  	((unsigned char *)&sn_rx_connect_request_pdu)[i] = buffer_rx[i];
  	      	   } 
	      
  	      	   printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\n", \
	      	    	sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		sn_rx_connect_request_pdu.pdu_adv_ind_h.TxAdd, sn_rx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	sn_rx_connect_request_pdu.pdu_adv_ind_h.Length, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_2);
	      	   		   		
		   //to be added: check if it is a connection request
		   next_adv_ch = 0;
		   sn.nextState=0; bs.nextState=0;   // temporarily settings
	        } else {
			next_adv_ch = 1;
	        }
		 		 
		if (next_adv_ch == 1) {
		     sn_adv_ch_idx++;
		     if (sn_adv_ch_idx > 39 ) { 
			sn_adv_ch_idx = 37;	
		 	sn.nextState=0; bs.nextState=0;   // temporarily settings
		     } else {
			sn.nextState=MODE_SN_TX_ADV_IND; bs.nextState=0; 
		     }		  		  		      
		     next_adv_ch = 0;		 
	         }
	       
	      free(buffer_rx); pdu_size=0;	
  	      break;	
 	
	default: sn.nextState=0; bs.nextState=0; break;	         	     
      }     
    } else if (get_id()==bs.id) {
     switch(bs.nextState) {
      	case MODE_BS_STANDBY:		          	     	 
	     // Turn off BLE
    	     ble_off();
	      
	     // Delay
  	     start_time = timer_time_us();
  	     while ((timer_time_us() - start_time) < (unsigned int)t_standby);	        	     
	     
	     // Go to initiating state
	     bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
	     break; 
     
        case MODE_BS_TX_CONNECT_REQ:	      	  		       
	      // Wait for inter frame space
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)T_IFS);
	      
	      // Configure ADPLL and BLE for data transmission	  
  	      config_tx(bs_ch_freq);   	      	        
	      
	      pdu_size = PDU_H_LEN;
	      //pdu_size = PDU_H_LEN + CONNECT_REQ_L_LEN;
	      buffer_tx = (char*)malloc(pdu_size * sizeof(char));
  	      // Initialize buffer_tx 
	      for (int i = 0; i < pdu_size; i++) {
	             buffer_tx[i] = 0;
  	      }
	      
	      // Build PDU  
	      for (int i=0; i < pdu_size; i++) {
	           buffer_tx[i] = ((unsigned char *)&bs_tx_connect_request_pdu)[i];
	      }
	      	      
	      ble_send(buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      ble_off();

  	      uart_printf("\nBS TX sent: %d bytes/CONNECT_REQ on advertising channel (index, frequency) =(%d, %dMHz)\n", pdu_size, bs_adv_ch_idx, bs_ch_freq);
  	      printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\n", \
	      	    	bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd, bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	bs_tx_connect_request_pdu.pdu_adv_ind_h.Length, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2);

	      free(buffer_tx); pdu_size=0;
	      bs.nextState=0; sn.nextState=0;   //temporarily for debugging purpose
	      break;
	      
  	case MODE_BS_RX_ADV_IND:  	      
	      bs_ch_freq = get_adv_ch_freq(bs_adv_ch_idx);
	      
	      // Configure payload size, configure ADPLL and configure BLE for data reception
	      pdu_size = PDU_H_LEN + ADV_IND_L_LEN;
	      config_rx(pdu_size, bs_ch_freq);

  	      // Stay on the advertising channel to listen to a connectable undirected advertising event
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)scanWindow);

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
    	      
	      uart_printf("\nBS RX received %d bytes/ADV_IND on advertising channel (index, frequency) =(%d, %dMHz)\n", nbytes, bs_adv_ch_idx, bs_ch_freq);	

	      if (nbytes == pdu_size) { // There is data in the rx buffer
  	      	   for (int i = 0; i < pdu_size - CRC_LEN; i++) {
		  	((unsigned char *)&bs_rx_adv_ind_pdu)[i] = buffer_rx[i];
  	      	   }           
	           
	           printf_("\nPDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength= %d\nRFU_2 = %d\nAdvA  = %#"PRIx64"\nAdvData = %#x\n", \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	bs_rx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.Length, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvData);   
	   		   	    		   		         	      
		   if ((unsigned short)bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type == ADV_IND) {
			bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0; 
              	   } else {
		        bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
		   }
	      }	else {
	           bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
	      }	      	       
  	      
	      free(buffer_rx); pdu_size=0;	      
	      break;
	      
	 default: sn.nextState=0; bs.nextState=0; break; 	               
     } 
  }		  
  
  j++;
  
  }
  
  return 0;
}
