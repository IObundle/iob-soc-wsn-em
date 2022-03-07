#include "system.h"
#include "periphs.h"
#include "iob-uart.h"
#include "iob_timer.h"
#include "txrx.h"
#include "id.h"
#include "ble.h"
#include "def.h"

int main() { 			
  int *sn_buffer_tx=(int*)0x400;
  int *bs_buffer_tx=(int*)0x400;    
  volatile int *sn_buffer_rx=NULL;
  volatile int *bs_buffer_rx=NULL; 	
  uint16_t sn_adv_ch_idx=37, bs_adv_ch_idx=37, next_adv_ch = 0;					
  uint32_t pdu_size=0, start_time=0, elapsedu=0, j=0, sn_ch_freq=0, bs_ch_freq=0, op=21;
  uint64_t elapsed=0;   	 
  uint64_t bs_whitelist[MAX_NUM_SN][3] = {{AA_FREE, 0xA3B9C1E5, 0}, {AA_FREE, 0xF2FFFFFF, 0}, 
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
  
  // BS TX - Connect request PDU 
  connect_request_pdu_s_t bs_tx_connect_request_pdu = {0}; 
  
  // SN RX - Connect request PDU 
  connect_request_pdu_s_t sn_rx_connect_request_pdu = {0};
       
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
		//sn.nextState=MODE_SN_RX_CONNECT_REQ; bs.nextState=0;
	     }		
	     break; 
	      		
	case MODE_SN_TX_ADV_IND:	
	      // Get the the advertising channel frequency      	      	      
	      sn_ch_freq = get_adv_ch_freq(sn_adv_ch_idx);	
	      //txrx_set_ch_idx(sn_adv_ch_idx);      		   	      
	      	      
	      // Configure ADPLL
	      ble_config(FREQ_CHANNEL, ADPLL_OPERATION);
  	      //ble_config(sn_ch_freq, ADPLL_OPERATION);	   
	      	      	      
	      // Configure BLE for data transmission
  	      ble_send_on();  
	      
	      // Configure the PDU's size 	
  	      pdu_size = ADV_IND_H_LEN + ADV_IND_P_LEN;
	      
	      // Allocate memory and initialize buffer_tx
	      sn_buffer_tx = (int*)malloc(pdu_size * sizeof(char));	
	      //sn_buffer_tx = (char*)malloc(pdu_size * sizeof(char));      
	      //for (int i = 0; i < pdu_size; i++) { sn_buffer_tx[i] = 0; }

	      // Allocate memory, check and initialize 
	      //sn_tx_adv_ind_pdu = malloc(sizeof *sn_tx_adv_ind_pdu);
	      //if (sn_tx_adv_ind_pdu == NULL) {uart_printf("Memory Allocation Error: sn_tx_adv_ind_pdu!\n");}
	      //memset(&sn_tx_adv_ind_pdu, 0, pdu_size);

	      // Build PDU	
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type=ADV_IND; 
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1=ADV_IND_H_RFU_1;       
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd=ADV_IND_H_H_TxAdd; 
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd=ADV_IND_H_RxAdd;
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length=ADV_IND_P_LEN; 
	      sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2=ADV_IND_H_RFU_2;	     	     	      //temporarily for debugging purpose
	      sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA=0x8C87BAE5D45B; 
	      sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData=0xB2A9;    	
	      
	      for (int i=0; i < pdu_size; i++) { *((char*)sn_buffer_tx+i) = ((unsigned char *)&sn_tx_adv_ind_pdu)[i]; }    	      
	        	   	      	      
	      // Write PDU to HW
	      ble_send_tempo((const int *)sn_buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      // Turn off BLE
	      ble_off();
      
	      // Print data for debugging purpose
	      //printf_("pdu_size = %d bytes\nsize(sn_tx_adv_ind_pdu) = %d bytes\n", pdu_size, sizeof(*sn_tx_adv_ind_pdu));
	      //printf_("pdu_size = %d bytes\nsize(sn_tx_adv_ind_pdu) = %d bytes\n", pdu_size, sizeof(sn_tx_adv_ind_pdu));
	      uart_printf("\nSN TX sent %d bytes/ADV_IND on advertising channel (index, frequency) =(%d, %dMHz)\n", pdu_size, sn_adv_ch_idx, sn_ch_freq);		  

	      printf_("\nPDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength= %d\nRFU_2 = %d\nAdvA  = %#llx\nAdvData = %#x\n", \
			sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length, sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData);    			
	      
	      // Free memory and re-initialize
	      //free(sn_tx_adv_ind_pdu); 
	      free((void*)sn_buffer_tx); pdu_size=0;
	      
	      // Go to the next state
	      sn.nextState=MODE_SN_RX_CONNECT_REQ; bs.nextState=0;
	      //sn.nextState=0; bs.nextState=0;  	      	       	      
  	      break;
	      
	case MODE_SN_RX_CONNECT_REQ:
	     // Configure PDU's size
	      pdu_size = ADV_IND_H_LEN + CONNECT_REQ_P_LEN;
              ble_payload(pdu_size);

	      //txrx_set_ch_idx(sn_adv_ch_idx);
	      	
  	      // Configure ADPLL
  	      ble_config((FREQ_CHANNEL-1.0F), ADPLL_OPERATION);
	      //ble_config((sn_ch_freq-1.0F), ADPLL_OPERATION);

  	      // Configure BLE for receive data
              ble_recv_on();
	      
	      // Stay on the advertising channel to listen to a connection request 
	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)advDelay);

  	      // Receive data
	      pdu_size += CRC_LEN;
	      //sn_buffer_rx = (char*)malloc(pdu_size * sizeof(char));
	      sn_buffer_rx = (int*)malloc(pdu_size * sizeof(char));
  	      for (int i = 0; i < pdu_size; i++) { sn_buffer_rx[i] = 0; }
  	      //char nbytes = ble_receive(sn_buffer_rx, pdu_size);
	      char nbytes = ble_receive_tempo(sn_buffer_rx, pdu_size);

  	      ble_off();

  	      // Wait for sender
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

	      uart_printf("\nSN RX received %d bytes/CONNECT_REQ on advertising channel (index, frequency) =(%d, %dMHz)\n", nbytes, sn_adv_ch_idx, sn_ch_freq);	      
	      if (nbytes == pdu_size) { // There is data in the rx buffer
	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
	       	  	((unsigned char *)&sn_rx_connect_request_pdu)[i] = sn_buffer_rx[i];
  	      	   } 

  	      	    //printf_("PDU_Type = %d\nLength = %d\n", sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, sn_rx_connect_request_pdu.pdu_adv_ind_h.Length);
		    
		    //printf_("LLData_AA = %#x\nLLData_CRCInit = %#x\n", \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_AA, sn_rx_connect_request_pdu.connect_req_payload.LLData_CRCInit);	
	      
  	      	    printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\n", \
	      	    	sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		sn_rx_connect_request_pdu.pdu_adv_ind_h.TxAdd, sn_rx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	sn_rx_connect_request_pdu.pdu_adv_ind_h.Length, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_2);

 	      	    printf_("InitA = %#llx\nAdvA = %#llx\nLLData_AA = %#x\nLLData_CRCInit = %#x\n", \
			sn_rx_connect_request_pdu.connect_req_payload.InitA, sn_rx_connect_request_pdu.connect_req_payload.AdvA, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_AA, sn_rx_connect_request_pdu.connect_req_payload.LLData_CRCInit);
			
  	      	    printf_("LLData_WinSize = %d\nLLData_WinOffset = %d\nLLData_Interval = %d\nLLData_Latency = %d\nLLData_Timeout = %d\nLLData_ChM = %d\n", \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_WinSize, sn_rx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_Interval, sn_rx_connect_request_pdu.connect_req_payload.LLData_Latency, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_Timeout, sn_rx_connect_request_pdu.connect_req_payload.LLData_ChM);	
			
  	      	    printf_("LLData_Hop = %d\nLLData_SCA = %d\n", \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_Hop, sn_rx_connect_request_pdu.connect_req_payload.LLData_SCA);	

  	      	    /*printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\nInitA = %#llx\nAdvA = %#llx\nLLData_AA = %#x\nLLData_CRCInit = %#x\n \
	                LLData_WinSize = %d\nLLData_WinOffset = %d\nLLData_Interval = %d\nLLData_Latency = %d\nLLData_Timeout = %d\nLLData_ChM = %d\nLLData_Hop = %d\nLLData_SCA = d\n", \
	      	    	sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		sn_rx_connect_request_pdu.pdu_adv_ind_h.TxAdd, sn_rx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	sn_rx_connect_request_pdu.pdu_adv_ind_h.Length, sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_2, \
			sn_rx_connect_request_pdu.connect_req_payload.InitA, sn_rx_connect_request_pdu.connect_req_payload.AdvA, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_AA, sn_rx_connect_request_pdu.connect_req_payload.LLData_CRCInit, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_WinSize, sn_rx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_Interval, sn_rx_connect_request_pdu.connect_req_payload.LLData_Latency, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_Timeout, sn_rx_connect_request_pdu.connect_req_payload.LLData_ChM, \
			sn_rx_connect_request_pdu.connect_req_payload.LLData_Hop, sn_rx_connect_request_pdu.connect_req_payload.LLData_SCA);*/
	      	   		   		
		    //for (int i = 1; i < op; i++) { printf("buffer_rx[%d] = %d\n", i, buffer_rx[i]); }
		   
		   if (sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type == CONNECT_REQ) {  // If a connection request is received
		   	printf_("SN received a CONNECT_REQ packet\n");
			next_adv_ch = 0;
		   	sn.nextState=0; bs.nextState=0;   // temporarily settings
	            } else {
		        printf_("SN did not receive a CONNECT_REQ packet\n");
		        next_adv_ch = 1;
		    }		
	        } else {
		        printf_("The number of received bytes is different from the expected one!\n");
			next_adv_ch = 1;
	        }
		 		 
		if (next_adv_ch == 1) {
		     sn_adv_ch_idx++;
		     if (sn_adv_ch_idx > 39 ) { 
			sn_adv_ch_idx = 37;	
		 	sn.nextState=0; bs.nextState=0;   // temporarily settings
		     } else {
			sn.nextState=MODE_SN_TX_ADV_IND; bs.nextState=0; 
			//sn.nextState=0; bs.nextState=0; 
		     }		  		  		      
		     next_adv_ch = 0;		 
	         }
	       
	      free((void*)sn_buffer_rx); pdu_size=0;	
  	      break;	
 	
	case MODE_SN_TX_DATA:
		sn.nextState=0; bs.nextState=0; break;
		 
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
	     //bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0;
	     break; 
     
        case MODE_BS_TX_CONNECT_REQ:	      	  		       
	      // Wait for inter frame space
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)T_IFS);  	      	        

	     //txrx_set_ch_idx(bs_adv_ch_idx);	
	      
	      // Configure ADPLL
	      ble_config(FREQ_CHANNEL, ADPLL_OPERATION);
  	      //ble_config(bs_ch_freq, ADPLL_OPERATION);

  	      // Configure BLE for data transmission
  	      ble_send_on();
	      
	      // Configure the PDU's size
	      pdu_size = ADV_IND_H_LEN + CONNECT_REQ_P_LEN;

	      // Allocate memory and initialize buffer_tx
	      //bs_buffer_tx = (char*)malloc(pdu_size * sizeof(char));
	      bs_buffer_tx = (int*)malloc(pdu_size * sizeof(char));	      
	      //for (int i = 0; i < pdu_size; i++) { bs_buffer_tx[i] = 0; }	
	      
	      //for (int i = 1; i < op; i++) { buffer_tx[i] = i; }	       
	      
	      // Allocate memory, check and initialize 
	      //bs_tx_connect_request_pdu = malloc(sizeof *bs_tx_connect_request_pdu);	
	      //bs_tx_connect_request_pdu = malloc(pdu_size);      
	      //if (bs_tx_connect_request_pdu == NULL) {uart_printf("Memory Allocation Error: bs_tx_connect_request_pdu!\n");}
	      //memset(&bs_tx_connect_request_pdu, 0, pdu_size);	      
	      
	      // Build PDU
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type=CONNECT_REQ;
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1=CONNECT_REQ_H_RFU_1;	    
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd=CONNECT_REQ_H_TxAdd;
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd=CONNECT_REQ_H_RxAdd;	    
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.Length=CONNECT_REQ_P_LEN;
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2=CONNECT_REQ_H_RFU_2;	
	      
	      bs_tx_connect_request_pdu.connect_req_payload.InitA=0x2B53E85C74A8;
	      bs_tx_connect_request_pdu.connect_req_payload.AdvA=0x8C87BAE5D45B;
	      
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_AA=0xA3B9C1E5;
	      
	      /*for(int i=0; i<MAX_NUM_SN; i++) {
   	       	   if (bs_whitelist[i][0] == AA_FREE) { // Access Address not yet used
		   	  printf_("There is a free address!\n"); 
   	       		  //bs_tx_connect_request_pdu.connect_req_payload.LLData_AA=bs_whitelist[i][1];
	      		  //bs_whitelist[i][0]=GPS_WAIT;
			  i=MAX_NUM_SN;
   	      	   }
	      }*/	      	      	      
	      
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit=0x555555;	
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize=1; 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset=2;		 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval=3; 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency=4;  	       
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout=5; 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM=6;		
              bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop=2; 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA=1;		    
	      	      			    	      
	      //for (int i=0; i < pdu_size; i++) { bs_buffer_tx[i] = ((unsigned char *)&bs_tx_connect_request_pdu)[i]; }  
	      for (int i=0; i < pdu_size; i++) { *((char*)bs_buffer_tx+i) = ((unsigned char *)&bs_tx_connect_request_pdu)[i]; }   	   	      
	      
	      // Write PDU to HW
	      ble_send_tempo((const int *)bs_buffer_tx, pdu_size);

  	      // Wait for transmisstion
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1000);

  	      // Turn off BLE
	      ble_off();

  	      // Print data for debugging purpose
	      //printf_("pdu_size = %d bytes\nsize(bs_tx_connect_request_pdu) = %d bytes\n", pdu_size, sizeof(*bs_tx_connect_request_pdu));
	      //printf_("pdu_size = %d bytes\nsize(bs_tx_connect_request_pdu) = %d bytes\n", pdu_size, sizeof(bs_tx_connect_request_pdu));
	      uart_printf("\nBS TX sent %d bytes/CONNECT_REQ on advertising channel (index, frequency) =(%d, %dMHz)\n", pdu_size, bs_adv_ch_idx, bs_ch_freq);	      	      
			
  	      printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\n", \
	      	    	bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd, bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	bs_tx_connect_request_pdu.pdu_adv_ind_h.Length, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2);	
			
 	      printf_("InitA = %#llx\nAdvA = %#llx\nLLData_AA = %#x\nLLData_CRCInit = %#x\n", \
			bs_tx_connect_request_pdu.connect_req_payload.InitA, bs_tx_connect_request_pdu.connect_req_payload.AdvA, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_AA, bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit);
			
  	      printf_("LLData_WinSize = %d\nLLData_WinOffset = %d\nLLData_Interval = %d\nLLData_Latency = %d\nLLData_Timeout = %d\nLLData_ChM = %d\n", \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize, bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval, bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout, bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM);
			
	      printf_("LLData_Hop = %d\nLLData_SCA = %d\n", \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop, bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA);			
							
  	      /*printf_("PDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength = %d\nRFU_2 = %d\nInitA = %#llx\nAdvA = %#llx\nLLData_AA = %#x\nLLData_CRCInit = %#x\n \
	                LLData_WinSize = %d\nLLData_WinOffset = %d\nLLData_Interval = %d\nLLData_Latency = %d\nLLData_Timeout = %d\nLLData_ChM = %d\nLLData_Hop = %d\nLLData_SCA = d\n", \
	      	    	bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd, bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	bs_tx_connect_request_pdu.pdu_adv_ind_h.Length, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2, \
			bs_tx_connect_request_pdu.connect_req_payload.InitA, bs_tx_connect_request_pdu.connect_req_payload.AdvA, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_AA, bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize, bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval, bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout, bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop, bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA);*/	
		
	      //for (int i = 1; i < op; i++) { printf("buffer_tx[%d] = %d\n", i, buffer_tx[i]); }	 
		
	      // Free memory and re-initialize
	      //free(bs_tx_connect_request_pdu); 
	      free((void*)bs_buffer_tx); pdu_size=0;
	      
	      // Go to the next state
	      bs.nextState=0; sn.nextState=0;   //temporarily for debugging purpose
	      break;
	      
  	case MODE_BS_RX_ADV_IND:  	      
	      bs_ch_freq = get_adv_ch_freq(bs_adv_ch_idx);
	      //txrx_set_ch_idx(bs_adv_ch_idx);
	      
	      // Configure payload's size
	      pdu_size = ADV_IND_H_LEN + ADV_IND_P_LEN;	      
              ble_payload(pdu_size);

  	      // Configure ADPLL   
  	      ble_config((FREQ_CHANNEL-1.0F), ADPLL_OPERATION);
	      //ble_config((bs_ch_freq-1.0F), ADPLL_OPERATION);

  	      // Configure BLE for receive data
              ble_recv_on();

  	      // Stay on the advertising channel to listen to a connectable undirected advertising event
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)scanWindow);

  	      // Receive data
	      pdu_size += CRC_LEN;
	      bs_buffer_rx = (int*)malloc(pdu_size * sizeof(char));
  	      for (int i = 0; i < pdu_size; i++) { bs_buffer_rx[i] = 0; }
  	      char nbytes = ble_receive_tempo(bs_buffer_rx, pdu_size);

  	      ble_off();

  	      // Wait for sender
  	      start_time = timer_time_us();
  	      while ((timer_time_us() - start_time) < (unsigned int)1100);   //initial 1000  	    
    	      
	      uart_printf("\nBS RX received %d bytes/ADV_IND on advertising channel (index, frequency) =(%d, %dMHz)\n", nbytes, bs_adv_ch_idx, bs_ch_freq);	

	      if (nbytes == pdu_size) { // There is data in the rx buffer
  	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
		  	((unsigned char *)&bs_rx_adv_ind_pdu)[i] = bs_buffer_rx[i];
  	      	   }           	           
			
	           printf_("\nPDU_Type = %d\nRFU_1 = %d\nTxAdd = %d\nRxAdd = %d\nLength= %d\nRFU_2 = %d\nAdvA  = %#llx\nAdvData = %#x\n", \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	bs_rx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.Length, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvData); 			
	   		   	    		   		         	      
		   if (bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type == ADV_IND) {
		   	printf_("BS received an ADV_IND packet\n");
			bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0;
			//bs.nextState=0; sn.nextState=0;  
              	   } else {
		   	printf_("BS did not receive an ADV_IND packet\n");
		        bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
			//bs.nextState=0; sn.nextState=0;  
		   }
	      }	else {
	           printf_("BS: the number of received bytes if different from the expected one!\n");
	           bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
		   //bs.nextState=0; sn.nextState=0;  
	      }	      	       
  	      
	      free((void*)bs_buffer_rx); pdu_size=0;	      
	      break;
	      
	 default: sn.nextState=0; bs.nextState=0; break; 	               
     } 
  }		  
  
  j++;
  
  }
  
  return 0;
}
