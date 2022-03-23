#include "system.h"
#include "periphs.h"
#include "iob-uart.h"
#include "iob_timer.h"
#include "id.h"
#include "ble.h"
#include "def.h"
#include "sn_fct.h"

extern sn_s_t sn;  
bs_s_t bs = {.id = BS_ID, .aa = 0x8E89BED6, .nextState = MODE_BS_STANDBY, .isBusy=0};   //By default, BS is in STANDBY mode
 
sn_tx_adv_param_s_t p_stv={0}; sn_rx_cnt_req_param_s_t p_srq={0};
sn_tx_gps_param_s_t p_stgps={0}; sn_rx_llcontrol_param_s_t p_srack_gps={0};
sn_tx_tmp_param_s_t p_sttmp={0}; sn_rx_llcontrol_param_s_t p_srack_tmp={0}; 

int16_t sn_data_tmp=-15;   //temporarily setting for debugging purpose
   
int main() { 
  int8_t nbytes; 			
  uint16_t bs_adv_ch_idx=37;
  uint16_t bs_data_ch_idx=0;		
  uint32_t sn_start_time=0, pdu_size=0, bs_start_time=0, bs_ch_freq=0; 	 
  uint64_t bs_whitelist[MAX_N_SN][3] = {{AA_FREE, 0xA3B9C1E5, 0}, {AA_FREE, 0xF2FFFFFF, 0}, 
  				 	{AA_FREE, 0xF3FFFFFF, 0}, {AA_FREE, 0xF4FFFFFF, 0}, 
				 	{AA_FREE, 0xF5FFFFFF, 0}, {AA_FREE, 0xF6FFFFFF, 0}};
  				       
  adv_ind_pdu_s_t bs_rx_adv_ind_pdu={0};                   //Connectable undirected advertising PDU  
  connect_request_pdu_s_t bs_tx_connect_request_pdu={0};   //Connection request PDU
  pdu_lldata_gps_s_t bs_rx_data_gps_pdu={0};               //GPS Coordinates data PDU  
  pdu_lldata_tmp_s_t bs_rx_lldata_tmp_pdu={0};             //TMP data PDU   
  pdu_llcontrol_s_t  bs_tx_pdu_llcontrol={0};              //ACK/TERMINATE PDU
        
  p_srq.sn_adv_ch_idx=ADV_CH_FIRST;
  
  //Init ID
  id_init(ID_BASE);
  					
  if(get_id()==sn.id) {
   uint32_t js=0;
    while(js<ITER) { //will be replaced by while(1) to run with non-stop
      switch(sn.nextState) {
      	case MODE_SN_STANDBY:  	  
	     sn.nextState=sn_standby(sn.adv);
	     bs.nextState=0; break; 
	      		
	case MODE_SN_TX_ADV_IND:	      
	     p_stv=sn_tx_adv_ind(p_srq.sn_adv_ch_idx);
#ifdef DBUG      
	     sn_tx_adv_ind_print(p_stv, p_srq); 	 
#endif	            	      
	     sn.nextState=p_stv.nextState;	            	       	      
  	     bs.nextState=0; break;
	      
	case MODE_SN_RX_CONNECT_REQ:	      	
	      p_srq=sn_rx_cnt_req(p_srq.sn_adv_ch_idx, p_stv.sn_ch_freq);	
#ifdef DBUG 
  	      //Wait for sender
  	      sn_start_time = timer_time_us();
  	      while ((timer_time_us() - sn_start_time) < (unsigned int)1000);    
	         
	      sn_rx_cnt_req_print(p_srq, p_stv);
#endif  	      
	      sn.nextState=p_srq.nextState;
	      bs.nextState=0; break;				

	case MODE_SN_TX_DATA_GPS:
	     p_stgps=sn_tx_data_gps(p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_AA, p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_ChM);
#ifdef DBUG
	     sn_tx_data_gps_print(p_stgps);
#endif
	     sn.nextState=p_stgps.nextState;	
	     bs.nextState=0; break; 

  	case MODE_SN_RX_ACK_GPS:
	     p_srack_gps=sn_rx_llcontrol(p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq, 1);  	    
#ifdef DBUG
    	     //Wait for sender
             sn_start_time = timer_time_us();
             while ((timer_time_us() - sn_start_time) < (unsigned int)1000);	      
             
	     sn_rx_llcontrol_print(p_srack_gps, p_stgps);
#endif	     
	     sn.nextState=p_srack_gps.nextState;	
	     bs.nextState=0; break; 	
	     	     
	case MODE_SN_TX_DATA_TMP:
	     p_sttmp=sn_tx_data_tmp(p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq, sn_data_tmp);	
#ifdef DBUG
	     sn_tx_data_tmp_print(p_sttmp, p_stgps);
#endif	     
	     sn.nextState=p_sttmp.nextState;
	     bs.nextState=0; break;      

  	case MODE_SN_RX_END_CONNECTION:
	     p_srack_tmp=sn_rx_llcontrol(p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq, 2);	     
#ifdef DBUG
    	     //Wait for sender
             sn_start_time = timer_time_us();
             while ((timer_time_us() - sn_start_time) < (unsigned int)1000);	      
             
	     sn_rx_llcontrol_print(p_srack_tmp, p_stgps);
#endif	 
	     sn.nextState=p_srack_tmp.nextState;
	     bs.nextState=0; break; 	
		     	     
	default: sn.nextState=0; bs.nextState=0; break;	         	     
      }  
      js++;
     }    
  } else if (get_id()==bs.id) {
    uint32_t jb=0;
    while(jb<ITER) {  //will be replaced by while(1) to run with non-stop
      switch(bs.nextState) {
      	case MODE_BS_STANDBY:
  	     //System Init
  	     sys_init();				          	     	 
	     
	     //Turn off BLE
    	     ble_off();
	      
	     //Delay
  	     bs_start_time = timer_time_us();
  	     while ((timer_time_us() - bs_start_time) < (unsigned int)t_standby);	        	     
	     
	     //Go to next state
	     bs.nextState=MODE_BS_RX_ADV_IND; 

	     sn.nextState=0; break; 
     
        case MODE_BS_TX_CONNECT_REQ:		             	  		       
	      //Wait for inter frame space
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)T_IFS);  	      	        
	      
	      //Configure ADPLL
  	      ble_config(bs_ch_freq, 3);

  	      //Configure BLE for data transmission
  	      ble_send_on();

	      //Configure the whitener channel index
	      wp_set_ch_index(bs_adv_ch_idx);  
	      	      
	      //Configure PDU's size
	      pdu_size = ADV_IND_H_LEN + CONNECT_REQ_P_LEN;

	      //Build PDU
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type=CONNECT_REQ; bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1=CONNECT_REQ_H_RFU_1;	    
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd=CONNECT_REQ_H_TxAdd; bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd=CONNECT_REQ_H_RxAdd;	    
	      bs_tx_connect_request_pdu.pdu_adv_ind_h.Length=CONNECT_REQ_P_LEN; bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2=CONNECT_REQ_H_RFU_2;	
	      
	      bs_tx_connect_request_pdu.connect_req_payload.InitA=0x2B53E85C74A8; 
	      bs_tx_connect_request_pdu.connect_req_payload.AdvA=bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA;

	      for(int i=0; i<MAX_N_SN; i++) {
   	       	   if (bs_whitelist[i][0] == AA_FREE) { // Access Address not yet used
   	       		  bs_tx_connect_request_pdu.connect_req_payload.LLData_AA=bs_whitelist[i][1];
	      		  bs_whitelist[i][0]=GPS_WAIT;
			  i=MAX_N_SN;
   	      	   }
	      }	      	      	      
	      
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit=0x555555;	
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize=1; bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset=2;		 
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval=3; bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency=4;  	       
	      bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout=5; bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM=0x400400020;		
              bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop=2; bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA=1;		    
	      	      			    	      	       	   	      	      
	      //Write the PDU to the HW
	      ble_send((unsigned char *)&bs_tx_connect_request_pdu, pdu_size);	

  	      //Wait for transmisstion
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);

  	      //Turn off BLE
	      ble_off();
#ifdef DBUG
	      uart_printf("\nBS TX sent %d bytes/CONNECT_REQ packet on the advertising channel (index,frequency)=(%d,%dMHz)\n", pdu_size, bs_adv_ch_idx, bs_ch_freq);	  	      	      			
  	      printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
	      	    	bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	      		bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd, bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
		 	bs_tx_connect_request_pdu.pdu_adv_ind_h.Length, bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2);				
 	      printf_("InitA=%#llx | AdvA=%#llx | AA=%#x | CRCInit=%#x\n", \
			bs_tx_connect_request_pdu.connect_req_payload.InitA, bs_tx_connect_request_pdu.connect_req_payload.AdvA, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_AA, bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit);			
  	      printf_("WinSize=%d | WinOffset=%d | Interval=%d | Latency=%d | Timeout=%d\n", \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize, bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval, bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout);			
	      printf_("ChM=%#llx | Hop=%d | SCA=%d\n\n", \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM, bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop, \
			bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA);					      	        	 
#endif 	      
	      //Re-initialize pdu_size
	      pdu_size=0;
	      
	      //Go to the next state
	      bs.nextState=MODE_BS_RX_DATA_GPS; sn.nextState=0;  
	      
	      break;
	      
  	case MODE_BS_RX_ADV_IND:  		      
	      //Get the advertising channel frequency 	
	      bs_ch_freq = get_adv_ch_freq(bs_adv_ch_idx);
	      
	      //Configure PDU's size
	      pdu_size = ADV_IND_H_LEN + ADV_IND_P_LEN;	      
              ble_payload(pdu_size);

  	      //Configure ADPLL   
	      ble_config((bs_ch_freq-1.0F), 2);

  	      //Configure BLE for data reception
              ble_recv_on();

	      //Configure the dewhitener channel index
	      wp_set_ch_index(bs_adv_ch_idx);  
	      
  	      //Stay on the advertising channel to listen for a connectable undirected advertising event
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)scanWindow);	
	      
	      //Read the PDU from the HW
	      pdu_size += CRC_LEN;
  	      for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_rx[i] = 0; }
  	      
	      nbytes = ble_receive(bs.buffer_rx, pdu_size);
	      
  	      //Turn off BLE
	      ble_off();
	      
  	      //Wait for sender
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);  	
	          	      
#ifdef DBUG
	      uart_printf("\nBS RX received %d bytes/ADV_IND expected on the advertising channel (index,frequency)=(%d,%dMHz)\n", nbytes, bs_adv_ch_idx, (bs_ch_freq-1));	
#endif	      
	      if (nbytes == pdu_size) { // There is data in the RX buffer
  	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
		  	((unsigned char *)&bs_rx_adv_ind_pdu)[i] = bs.buffer_rx[i];
  	      	   }           	           
#ifdef DBUG      	      				
	           printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\nAdvA=%#llx | AdvData=%#x\n", \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
		   	bs_rx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_h.Length, bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
			bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvData); 			
#endif	   		   	    		   		         	      
		   if (bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type == ADV_IND) {
#ifdef DBUG		   
		   	printf_("BS received an ADV_IND packet\n\n");
#endif			
			bs.nextState=MODE_BS_TX_CONNECT_REQ; sn.nextState=0;
			//bs.nextState=0; sn.nextState=0;
              	   } else {
#ifdef DBUG		   
		   	printf_("BS did not receive an ADV_IND packet\n\n");
#endif			
		        bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
		   }
	      }	else {
#ifdef DBUG	      
	           printf_("BS received %d bytes instead of %d bytes!\n\n", nbytes, pdu_size);
#endif		   
	           bs.nextState=MODE_BS_RX_ADV_IND; sn.nextState=0;
	      }	      	       
	      
	      // Re-initialize
	      pdu_size=0;	
	            
	      break;	      			      

  	case MODE_BS_RX_DATA_GPS:  	
	      //Set data channel index
	      bs_data_ch_idx=5;  //temporarily for debugging purpose
	
	      //Get the data channel frequency	      
	      bs_ch_freq = get_data_ch_freq(bs_data_ch_idx);
	      
	      //Configure PDU's size
  	      pdu_size = LL_DATA_H_LEN + LL_DATA_GPS_P_LEN;    
              ble_payload(pdu_size);

  	      //Configure ADPLL   
	      ble_config((bs_ch_freq-1.0F), 2);

  	      //Configure BLE for data reception
              ble_recv_on();

	      //Configure the dewhitener channel index
	      wp_set_ch_index(bs_data_ch_idx);  
	      
	      //Set BS RX Access Address
	      wp_set_aa(bs_tx_connect_request_pdu.connect_req_payload.LLData_AA);  
	      
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)scanWindow);   //delay to be changed

  	      // Read the PDU from the HW
	      pdu_size += CRC_LEN;
  	      for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_rx[i] = 0; }
  	      nbytes = ble_receive(bs.buffer_rx, pdu_size);

  	      //Turn off BLE
	      ble_off();

  	      //Wait for sender
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);  	        	      
#ifdef DBUG
	      uart_printf("\nBS RX received %d bytes/DATA_GPS expected on the data channel (index,frequency)=(%d,%dMHz)\n", nbytes, bs_data_ch_idx, (bs_ch_freq - 1));	
#endif	      
	      if (nbytes == pdu_size) { // There is data in the RX buffer
  	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
		  	((unsigned char *)&bs_rx_data_gps_pdu)[i] = bs.buffer_rx[i];
  	      	   }           	           
#ifdef DBUG      	      				
	      	   printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n", \
			bs_rx_data_gps_pdu.pdu_lldata_h.LLID, bs_rx_data_gps_pdu.pdu_lldata_h.NESN, \
		   	bs_rx_data_gps_pdu.pdu_lldata_h.SN, bs_rx_data_gps_pdu.pdu_lldata_h.MD, \
			bs_rx_data_gps_pdu.pdu_lldata_h.RFU, bs_rx_data_gps_pdu.pdu_lldata_h.Length, \
			bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_D, bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_M, \
	      		bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_S, bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_C, \
	      		bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_D, bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_M, \
	      		bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_S, bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_C); 
		    printf("BS received a DATA_GPS packet\n\n");				
#endif 		  
		  bs.nextState=MODE_BS_TX_ACK_GPS; 
		  sn.nextState=0;    
	      }	else {
#ifdef DBUG	      
	           printf_("BS received %d bytes instead of %d bytes!\n\n", nbytes, pdu_size);
#endif		   
	           bs.nextState=0; sn.nextState=0;
	      }	      	       
  	      
	      //Re-initialize
	      pdu_size=0;	
	            
	      break;	

	case MODE_BS_TX_ACK_GPS:	
	      //Configure ADPLL
  	      ble_config(bs_ch_freq, 3);	    
	      	      	      
	      //Configure BLE for data transmission
  	      ble_send_on();  
	      
	      //Configure the whitener channel index
	      wp_set_ch_index(bs_data_ch_idx);   
	      
	      //Configure PDU's size 	
  	      pdu_size = LL_DATA_H_LEN + LL_CONTROL_P_LEN;
	      	
	      //Build PDU
	      bs_tx_pdu_llcontrol.pdu_lldata_h.LLID=LL_CONTROL_PDU; bs_tx_pdu_llcontrol.pdu_lldata_h.RFU=0;
	      bs_tx_pdu_llcontrol.pdu_lldata_h.NESN=0; bs_tx_pdu_llcontrol.pdu_lldata_h.SN=0;
	      bs_tx_pdu_llcontrol.pdu_lldata_h.MD=0; bs_tx_pdu_llcontrol.pdu_lldata_h.Length=LL_CONTROL_P_LEN;		 
	      bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode=LL_CONNECTION_ACK;
  	      bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData=POSITIVE_ACK;			  
	        	   	      	      
	      //Write the PDU to the HW
	      ble_send((unsigned char *)&bs_tx_pdu_llcontrol, pdu_size);

  	      // Wait for transmisstion
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);

  	      //Turn off BLE
	      ble_off();     
#ifdef DBUG
	      uart_printf("\nBS TX sent %d bytes/ACK_GPS packet on the data channel (index,frequency)=(%d,%dMHz)\n", pdu_size, bs_data_ch_idx, bs_ch_freq);		  
	      printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nOpcode=%d | CtrData=%d\n\n",
	      	   bs_tx_pdu_llcontrol.pdu_lldata_h.LLID, bs_tx_pdu_llcontrol.pdu_lldata_h.NESN, \
	      	   bs_tx_pdu_llcontrol.pdu_lldata_h.SN, bs_tx_pdu_llcontrol.pdu_lldata_h.MD, \
	      	   bs_tx_pdu_llcontrol.pdu_lldata_h.RFU, bs_tx_pdu_llcontrol.pdu_lldata_h.Length, \
	      	   bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode, \
	      	   bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData);  			    
#endif	      
	      //Re-initialize pdu_size
	      pdu_size=0;
	      
	      //Go to the next state
	      bs.nextState=MODE_BS_RX_DATA_TMP; 
	      sn.nextState=0; break;		      
	      
  	case MODE_BS_RX_DATA_TMP:  	      
	      //Configure PDU's size
  	      pdu_size = LL_DATA_H_LEN + LL_DATA_TMP_P_LEN;    
              ble_payload(pdu_size);

  	      //Configure ADPLL   
	      ble_config((bs_ch_freq-1.0F), 2);

  	      // Configure BLE for data reception
              ble_recv_on();

	      //Configure the dewhitener channel index
	      wp_set_ch_index(bs_data_ch_idx);  
	      
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)scanWindow);   //delay to be changed

  	      //Read the PDU from the HW
	      pdu_size += CRC_LEN;
  	      for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_rx[i] = 0; }
  	      nbytes = ble_receive(bs.buffer_rx, pdu_size);

  	      //Turn off BLE
	      ble_off();

  	      //Wait for sender
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);  	        	      
#ifdef DBUG	      	      
	      uart_printf("\nBS RX received %d bytes/DATA_TMP expected on the data channel (index,frequency)=(%d,%dMHz)\n", nbytes, bs_data_ch_idx, (bs_ch_freq - 1));	
#endif	      
	      if (nbytes == pdu_size) { // There is data in the RX buffer
  	      	   for (int i = 0; i < (pdu_size - CRC_LEN); i++) {
		  	((unsigned char *)&bs_rx_lldata_tmp_pdu)[i] = bs.buffer_rx[i];
  	      	   }           	           
#ifdef DBUG	      	      				
	      	   printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nTemperature=%d\n", \
			bs_rx_lldata_tmp_pdu.pdu_lldata_h.LLID, bs_rx_lldata_tmp_pdu.pdu_lldata_h.NESN, \
		   	bs_rx_lldata_tmp_pdu.pdu_lldata_h.SN, bs_rx_lldata_tmp_pdu.pdu_lldata_h.MD, \
			bs_rx_lldata_tmp_pdu.pdu_lldata_h.RFU, bs_rx_lldata_tmp_pdu.pdu_lldata_h.Length, \
			bs_rx_lldata_tmp_pdu.payload_tmp);
		    printf("BS received a DATA_TMP packet\n\n");				
#endif 		  
		  bs.nextState=MODE_BS_TX_END_CONNECTION; 
		  sn.nextState=0;   
	      }	else {
#ifdef BG	      
	           printf_("BS received %d bytes instead of %d bytes!\n\n", nbytes, pdu_size);
#endif		   
	           bs.nextState=0; sn.nextState=0;
	      }	      	       
  	      
	      //Re-initialize
	      pdu_size=0;	
	            
	      break;
	      	      	                  	 	 	 	      
	case MODE_BS_TX_END_CONNECTION:	    	   	   		   	      	      	      	      
	      //Configure ADPLL
  	      ble_config(bs_ch_freq, 3);	    
	      	      	      
	      //Configure BLE for data transmission
  	      ble_send_on();  
	      
	      //Configure the whitener channel index
	      wp_set_ch_index(bs_data_ch_idx);   
	      
	      //Configure PDU's size 	
  	      pdu_size = LL_DATA_H_LEN + LL_CONTROL_P_LEN;
	      	
	      //Build PDU
	      bs_tx_pdu_llcontrol.pdu_lldata_h.LLID=LL_CONTROL_PDU; bs_tx_pdu_llcontrol.pdu_lldata_h.RFU=0;
	      bs_tx_pdu_llcontrol.pdu_lldata_h.NESN=0; bs_tx_pdu_llcontrol.pdu_lldata_h.SN=0;
	      bs_tx_pdu_llcontrol.pdu_lldata_h.MD=0; bs_tx_pdu_llcontrol.pdu_lldata_h.Length=LL_CONTROL_P_LEN;		 
	      bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode=LL_TERMINATE_IND;
  	      bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData=END_CONNECTION;     
	      	        	   	      	      
	      //Write the PDU to the HW
	      ble_send((unsigned char *)&bs_tx_pdu_llcontrol, pdu_size);

  	      //Wait for transmisstion
  	      bs_start_time = timer_time_us();
  	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);

  	      //Turn off BLE
	      ble_off();     
#ifdef DBUG
	      uart_printf("\nBS TX sent %d bytes/END_CONNECTION packet on the data channel (index,frequency)=(%d,%dMHz)\n", pdu_size, bs_data_ch_idx, bs_ch_freq);		  
	      printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nOpcode=%d | CtrData=%d\n",
	      	   bs_tx_pdu_llcontrol.pdu_lldata_h.LLID, bs_tx_pdu_llcontrol.pdu_lldata_h.NESN, \
	      	   bs_tx_pdu_llcontrol.pdu_lldata_h.SN, bs_tx_pdu_llcontrol.pdu_lldata_h.MD, \
	      	   bs_tx_pdu_llcontrol.pdu_lldata_h.RFU, bs_tx_pdu_llcontrol.pdu_lldata_h.Length, \
	      	   bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode, \
	      	   bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData);  			    
#endif	      
	      //Re-initialize pdu_size
	      pdu_size=0;
	      
	      //Go to the next state
	      bs.nextState=0; sn.nextState=0;	      	       	      
  	      break;		      
	
	default: sn.nextState=0; bs.nextState=0; break; 	               
     } 
     jb++; 
    }		      
  }  
  return 0;
}
