#include "system.h"
#include "periphs.h"
#include "iob-uart.h"
#include "iob_timer.h"
#include "id.h"
#include "ble.h"
#include "def.h"

int main() { 		
  uint8_t  data_ch[MAX_N_DATA_CHANNELS]={0};		
  uint16_t sn_adv_ch_idx=37, bs_adv_ch_idx=39, next_adv_ch = 0;					
  uint32_t pdu_size=0, sn_start_time=0, bs_start_time=0, elapsedu=0, j=0, k=0, sn_ch_freq=0, bs_ch_freq=0;
  uint64_t elapsed=0, mask_data_ch=0x01;   	 
  uint64_t bs_whitelist[MAX_N_SN][3] = {{AA_FREE, 0xA3B9C1E5, 0}, {AA_FREE, 0xF2FFFFFF, 0}, 
  				 	{AA_FREE, 0xF3FFFFFF, 0}, {AA_FREE, 0xF4FFFFFF, 0}, 
				 	{AA_FREE, 0xF5FFFFFF, 0}, {AA_FREE, 0xF6FFFFFF, 0}};
  				    
  // By default, SN is in STANDBY mode
  sn_s_t sn = {.id = SN_ID, .aa = 0x8E89BED6, .adv_1 = 1, .nextState = MODE_SN_STANDBY};    	
  
  // By default, BS is in STANDBY mode
  bs_s_t bs = {.id = BS_ID, .aa = 0x8E89BED6, .nextState = MODE_BS_STANDBY, .isBusy=0};
  
  // SN TX - Connectable undirected advertising PDU
  adv_ind_pdu_s_t sn_tx_adv_ind_pdu = {0};
  
  // BS RX - Connectable undirected advertising PDU
  adv_ind_pdu_s_t bs_rx_adv_ind_pdu = {0};
  
  // BS TX - Connection request PDU 
  connect_request_pdu_s_t bs_tx_connect_request_pdu = {0}; 
  
  // SN RX - Connection request PDU 
  connect_request_pdu_s_t sn_rx_connect_request_pdu = {0};
       
  // Init ID
  id_init(ID_BASE);
  
  while (j<7) { // will be replaced by while(1) to run with non-stop
  					
  if(get_id()==sn.id) {
      switch(sn.nextState) {
      	case MODE_SN_STANDBY:	  
  	     // Init BLE, init UART, init Timer
  	     ble_init(); uart_init(UART_BASE,FREQ/BAUD); timer_init(TIMER_BASE);	       	     	 	     
	     
	     // Turn off BLE
    	     ble_off();	        	     
	     
	     // Delay
  	     sn_start_time = timer_time_us();
  	     while ((timer_time_us() - sn_start_time) < (unsigned int)t_standby);
	     	     
	     // Go to the advertising state
	     if (sn.adv_1 == 1) {
	     	sn.nextState=MODE_SN_TX_ADV_IND; bs.nextState=0;
	     }		
	     break; 
	      		
	case MODE_SN_TX_ADV_IND:	
	      // Get the advertising channel frequency      	      	      
	      sn_ch_freq = get_adv_ch_freq(sn_adv_ch_idx);			   	      
	      	      
	      // Configure ADPLL
  	      ble_config(sn_ch_freq, ADPLL_OPERATION);	   
	      	      	      
	      // Configure BLE for data transmission
  	      ble_send_on();  

	      // Configure the whitener channel index
	      wp_set_ch_index(sn_adv_ch_idx); 
	      	      
	      // Configure PDU's size 	
  	      pdu_size = ADV_IND_H_LEN + ADV_IND_P_LEN;
	      
	      // Initialize TX buffer  
	      for (int i = 0; i < MAX_N_BYTES; i++) { sn.buffer_tx[i] = 0; }

	      // Build PDU	
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type=ADV_IND; sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1=ADV_IND_H_RFU_1;       
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd=ADV_IND_H_H_TxAdd; sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd=ADV_IND_H_RxAdd;
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length=ADV_IND_P_LEN; sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2=ADV_IND_H_RFU_2;	   
	      sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA=0x8C87BAE5D45B; sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData=0xB2A9;    	
	      
	      for (int i=0; i < pdu_size; i++) { sn.buffer_tx[i] = ((unsigned char *)&sn_tx_adv_ind_pdu)[i]; }    	      
	        	   	      	      
	      // Write the PDU to the HW
	      ble_send(sn.buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      sn_start_time = timer_time_us();
  	      while ((timer_time_us() - sn_start_time) < (unsigned int)1200);

  	      // Turn off BLE
	      ble_off();
      
	      // Print data for debugging purpose
	      uart_printf("\nSN TX sent %d bytes/ADV_IND on advertising channel (index, frequency) = (%d, %dMHz)\n", pdu_size, sn_adv_ch_idx, sn_ch_freq);		  
	      printf_("\nPDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength= %d\nRFU_2 = %d\nAdvA  = %#llx\nAdvData = %#x\n\n", \
			sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData);    			
	      
	      // Re-initialize pdu_size
	      pdu_size=0;
	      
	      // Go to the next state
	      sn.nextState=MODE_SN_RX_CONNECT_REQ; bs.nextState=0;	      	       	      
  	      break;
	      
	case MODE_SN_RX_CONNECT_REQ:
	      // Configure PDU's size
	      pdu_size = ADV_IND_H_LEN + CONNECT_REQ_P_LEN;
              ble_payload(pdu_size);
	      	
  	      // Configure ADPLL
	      ble_config((sn_ch_freq-1.0F), ADPLL_OPERATION);

  	      // Configure BLE for data reception
              ble_recv_on();
	      
	      // Configure the dewhitener channel index
	      wp_set_ch_index(sn_adv_ch_idx);  
	      
	      // Stay on the advertising channel to listen to a connection request 
	      sn_start_time = timer_time_us();
  	      while ((timer_time_us() - sn_start_time) < (unsigned int)advDelay);

  	      // Read the PDU from the HW
	      pdu_size += CRC_LEN;
  	      for (int i = 0; i < MAX_N_BYTES; i++) { sn.buffer_rx[i] = 0; }
  	      char nbytes = ble_receive(sn.buffer_rx, pdu_size);

  	      // Turn off BLE
	      ble_off();

  	      // Wait for sender
  	      sn_start_time = timer_time_us();
  	      while ((timer_time_us() - sn_start_time) < (unsigned int)1200);
    
	      // Print data for debugging purpose
	      uart_printf("\nSN RX received %d bytes/CONNECT_REQ on advertising channel (index, frequency) = (%d, %dMHz)\n\n", nbytes, sn_adv_ch_idx, sn_ch_freq);	

	      if (nbytes == pdu_size) { // There is data in the RX buffer
	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
	       	  	((unsigned char *)&sn_rx_connect_request_pdu)[i] = sn.buffer_rx[i];
  	      	   } 
		    
		    // Print data for debugging purpose  	      
  	      	    printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\n", \
	      	    	sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		sn_rx_connect_request_pdu.pdu_adv_ind_h.TxAdd, sn_rx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	sn_rx_connect_request_pdu.pdu_adv_ind_h.Length, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_2);
 	      	    printf_("InitA = %#llx\nAdvA = %#llx\nLLData_AA = %#x\nLLData_CRCInit = %#x\n", \
			sn_rx_connect_request_pdu.connect_req_payload.InitA, sn_rx_connect_request_pdu.connect_req_payload.AdvA, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData.AA, sn_rx_connect_request_pdu.connect_req_payload.LLData.CRCInit);			
  	      	    printf_("LLData_WinSize = %d\nLLData_WinOffset = %d\nLLData_Interval = %d\nLLData_Latency = %d\nLLData_Timeout = %d\nLLData_ChM = %#llx\n", \
			sn_rx_connect_request_pdu.connect_req_payload.LLData.WinSize, sn_rx_connect_request_pdu.connect_req_payload.LLData.WinOffset, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData.Interval, sn_rx_connect_request_pdu.connect_req_payload.LLData.Latency, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData.Timeout, sn_rx_connect_request_pdu.connect_req_payload.LLData.ChM);				
  	      	    printf_("LLData_Hop = %d\nLLData_SCA = %d\n\n", \
			sn_rx_connect_request_pdu.connect_req_payload.LLData.Hop, sn_rx_connect_request_pdu.connect_req_payload.LLData.SCA);	
	      	   		   				   
		   if (sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type == CONNECT_REQ) {  // If a connection request is received
		   	printf_("SN received a CONNECT_REQ packet\n\n");
			next_adv_ch = 0;  k=0;			
			for (int i=0; i<MAX_N_DATA_CHANNELS; i++) {
			   if (sn_rx_connect_request_pdu.connect_req_payload.LLData.ChM & mask_data_ch) {
				data_ch[k++]=i;
			    }
			    mask_data_ch <<= 1; 					
			}
			for (int i=k; i<MAX_N_DATA_CHANNELS; i++) {data_ch[i]=0;}
			sn.nextState=0; bs.nextState=0;   // temporarily settings
	            } else {
		        printf_("SN did not receive a CONNECT_REQ packet\n\n");
		        next_adv_ch = 1;
		    }		
	        } else {
		        printf_("SN received %d bytes instead of %d bytes!\n\n", nbytes, pdu_size);
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
	      
	      	// Re-initialize pdu_size 
	        pdu_size=0;	
  	      
	        break;	
		 
	default: sn.nextState=0; bs.nextState=0; break;	         	     
      }     
    } else if (get_id()==bs.id) {
     switch(bs.nextState) {
      	case MODE_BS_STANDBY:
  	     // Init BLE, init UART, init Timer
  	     ble_init(); uart_init(UART_BASE,FREQ/BAUD); timer_init(TIMER_BASE);				          	     	 
	     
	     // Turn off BLE
    	     ble_off();
	      
	     // Delay
  	     bs_start_time = timer_time_us();
  	     while ((timer_time_us() - bs_start_time) < (unsigned int)t_standby);	        	     
	     
	     // Go to the initiating state
	     bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;

	     break; 
     
        case MODE_BS_TX_CONNECT_REQ:	      	  		       
	      // Wait for inter frame space
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)T_IFS);  	      	        
	      
	      // Configure ADPLL
  	      ble_config(bs_ch_freq, ADPLL_OPERATION);

  	      // Configure BLE for data transmission
  	      ble_send_on();

	      // Configure the whitener channel index
	      wp_set_ch_index(bs_adv_ch_idx);  
	      	      
	      // Configure PDU's size
	      pdu_size = ADV_IND_H_LEN + CONNECT_REQ_P_LEN;

	      // Initialize TX buffer      
	      for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_tx[i] = 0; }		      
	      
	      // Build PDU
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type=CONNECT_REQ; bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1=CONNECT_REQ_H_RFU_1;	    
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd=CONNECT_REQ_H_TxAdd; bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd=CONNECT_REQ_H_RxAdd;	    
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.Length=CONNECT_REQ_P_LEN; bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2=CONNECT_REQ_H_RFU_2;	
	      
	      bs_tx_connect_request_pdu.connect_req_payload.InitA=0x2B53E85C74A8; bs_tx_connect_request_pdu.connect_req_payload.AdvA=0x8C87BAE5D45B;

	      for(int i=0; i<MAX_N_SN; i++) {
   	       	   if (bs_whitelist[i][0] == AA_FREE) { // Access Address not yet used
		   	  printf_("There is a free address!\n"); 
   	       		  bs_tx_connect_request_pdu.connect_req_payload.LLData.AA=bs_whitelist[i][1];
	      		  bs_whitelist[i][0]=GPS_WAIT;
			  i=MAX_N_SN;
   	      	   }
	      }	      	      	      
	      
	      bs_tx_connect_request_pdu.connect_req_payload.LLData.CRCInit=0x555555;	
	      bs_tx_connect_request_pdu.connect_req_payload.LLData.WinSize=1; bs_tx_connect_request_pdu.connect_req_payload.LLData.WinOffset=2;		 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData.Interval=3; bs_tx_connect_request_pdu.connect_req_payload.LLData.Latency=4;  	       
	      bs_tx_connect_request_pdu.connect_req_payload.LLData.Timeout=5; bs_tx_connect_request_pdu.connect_req_payload.LLData.ChM=0x400400020;		
              bs_tx_connect_request_pdu.connect_req_payload.LLData.Hop=2; bs_tx_connect_request_pdu.connect_req_payload.LLData.SCA=1;		    
	      	      			    	      
	      for (int i=0; i < pdu_size; i++) { bs.buffer_tx[i] = ((unsigned char *)&bs_tx_connect_request_pdu)[i]; }    
	       	   	      	      
	      // Write the PDU to the HW
	      ble_send(bs.buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1200);

  	      // Turn off BLE
	      ble_off();

  	      // Print data for debugging purpose	      	      	
	      uart_printf("\nBS TX sent %d bytes/CONNECT_REQ on advertising channel (index, frequency) = (%d, %dMHz)\n", pdu_size, bs_adv_ch_idx, bs_ch_freq);	  	      			
  	      printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\n", \
	      	    	bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd, bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	bs_tx_connect_request_pdu.pdu_adv_ind_h.Length, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2);				
 	      printf_("InitA = %#llx\nAdvA = %#llx\nLLData_AA = %#x\nLLData_CRCInit = %#x\n", \
			bs_tx_connect_request_pdu.connect_req_payload.InitA, bs_tx_connect_request_pdu.connect_req_payload.AdvA, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData.AA, bs_tx_connect_request_pdu.connect_req_payload.LLData.CRCInit);			
  	      printf_("LLData_WinSize = %d\nLLData_WinOffset = %d\nLLData_Interval = %d\nLLData_Latency = %d\nLLData_Timeout = %d\nLLData_ChM = %#llx\n", \
			bs_tx_connect_request_pdu.connect_req_payload.LLData.WinSize, bs_tx_connect_request_pdu.connect_req_payload.LLData.WinOffset, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData.Interval, bs_tx_connect_request_pdu.connect_req_payload.LLData.Latency, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData.Timeout, bs_tx_connect_request_pdu.connect_req_payload.LLData.ChM);			
	      printf_("LLData_Hop = %d\nLLData_SCA = %d\n\n", \
			bs_tx_connect_request_pdu.connect_req_payload.LLData.Hop, bs_tx_connect_request_pdu.connect_req_payload.LLData.SCA);			 
		
	      // Re-initialize pdu_size
	      pdu_size=0;
	      
	      // Go to the next state
	      bs.nextState=0; sn.nextState=0;   //temporarily for debugging purpose
	      break;
	      
  	case MODE_BS_RX_ADV_IND:  	      
	      bs_ch_freq = get_adv_ch_freq(bs_adv_ch_idx);
	      
	      // Configure PDU's size
	      pdu_size = ADV_IND_H_LEN + ADV_IND_P_LEN;	      
              ble_payload(pdu_size);

  	      // Configure ADPLL   
	      ble_config((bs_ch_freq-1.0F), ADPLL_OPERATION);

  	      // Configure BLE for data reception
              ble_recv_on();

	      // Configure the dewhitener channel index
	      wp_set_ch_index(bs_adv_ch_idx);  
	      
  	      // Stay on the advertising channel to listen for a connectable undirected advertising event
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)scanWindow);

  	      // Read the PDU from the HW
	      pdu_size += CRC_LEN;
  	      for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_rx[i] = 0; }
  	      char nbytes = ble_receive(bs.buffer_rx, pdu_size);

  	      // Turn off BLE
	      ble_off();

  	      // Wait for sender
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1200);  	        	      

  	      // Print data for debugging purpose	      	      
	      uart_printf("\nBS RX received %d bytes/ADV_IND on advertising channel (index, frequency) = (%d, %dMHz)\n\n", nbytes, bs_adv_ch_idx, bs_ch_freq);	
	      
	      if (nbytes == pdu_size) { // There is data in the RX buffer
  	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
		  	((unsigned char *)&bs_rx_adv_ind_pdu)[i] = bs.buffer_rx[i];
  	      	   }           	           

  	           // Print data for debugging purpose	      	      				
	           printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength= %d\nRFU_2 = %d\nAdvA  = %#llx\nAdvData = %#x\n\n", \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	bs_rx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.Length, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvData); 			
	   		   	    		   		         	      
		   if (bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type == ADV_IND) {
		   	printf_("BS received an ADV_IND packet\n\n");
			bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0;
              	   } else {
		   	printf_("BS did not receive an ADV_IND packet\n\n");
		        bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
		   }
	      }	else {
	           printf_("BS received %d bytes instead of %d bytes!\n\n", nbytes, pdu_size);
	           bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
	      }	      	       
  	      
	      // Re-initialize
	      pdu_size=0;	
	            
	      break;
	      
	 default: sn.nextState=0; bs.nextState=0; break; 	               
     } 
  }		  
  
  j++;
  
  }
  
  return 0;
}
