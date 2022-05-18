#include "iob_timer.h"
#include "cm_def.h"
#include "sn_def.h"
#include "ble.h"
#include "tmp.h"

#define CST 1
#include "sn_config.h"

sn_s_t sn = {.id = SN_ID, .aa = 0x8E89BED6, .nextState = SN_STANDBY, .nSend=1, .transmitSeqNum=0, .nextExpectedSeqNum=0};
   
//------------------------------------------------------------------------------------------------------------------------------------------
int32_t sn_advertiser_filter(sn_rx_cnt_req_param_s_t p){
    int32_t error=-1;    
    if(p.nbytes == (p.pdu_size + CRC_LEN)) { //There is data in the RX buffer (pdu + crc)
        if(p.sn_rx_connect_request_pdu.h.Length == CONNECT_REQ_P_LEN) {
	   if(p.sn_rx_connect_request_pdu.payload.AdvA == SN_DEVICE_ADDR) { //This packet is addressed to this SN's device address
	      if(p.sn_rx_connect_request_pdu.payload.InitA == BS_DEVICE_ADDR_OP) { //The received packet has been sent by the BS	
	         if(p.sn_rx_connect_request_pdu.h.PDU_Type == CONNECT_REQ) { //If a connection request is received
	             error=0;	             			    
	          } else {
	             error=5;
	          }
	      } else {error=4;}	  	
	   } else {error=3;}   	
        } else {error=2;}
     } else {
	error=1;
    }     
    return error;
}

int32_t sn_rx_ack_check(sn_rx_data_ack_param_s_t p){
    int32_t error=-1;
    if(p.nbytes == (p.pdu_size + CRC_LEN)) {   //There is data in the RX buffer (pdu + crc)
        if(p.sn_rx_data_ack_pdu.Length == ZERO_PAYLOAD) {   //Zero length payload
	   if(p.sn_rx_data_ack_pdu.LLID == LL_DATA_PDU_CE) {   //Empty PDU   	 
	      if(sn.transmitSeqNum != p.sn_rx_data_ack_pdu.NESN) {   //The previous packet sent has been acknowledged
	         error=5;
	      } else {error=2;}   //The previous packet sent hasn't been acknowledged
	   } else {error=4;}   //The LLID value does not match that of an ACK PDU 
	} else {error=3;}   //The payload size does not match that of an ACK PDU  
    } else {error=1;}
    
    if(error==5){
       if(p.sn_rx_data_ack_pdu.MD == MD_VAL_ZERO) {   //No more data is expected from the BS - close connection
          error=0; 
       } else if(p.sn_rx_data_ack_pdu.MD == MD_VAL_ONE) {   //Something is wrong - it is an ACK but the BS does not close the connection - SN will retransmit the previous packet
          error=5;
       }   
    }
    return error;
}
//------------------------------------------------------------------------------------------------------------------------------------------
sn_standby_param_s_t sn_standby(){   //review the settings to be done when the SN is in standby state for the nth time, n>1  
    sn_standby_param_s_t p={0};
    timer_init(TIMER_BASE);
#ifdef DBUG
    p.start = timer_time_us();   //for debugging purpose
#endif                    
    sys_init(); //System Init	    

    ble_off();   //Turn off BLE  		    
   
    p.adv_ch_start_idx=ADV_CH_FIRST;   //Configure advertising channels starting index 
        
    //Delay
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_SN_STANDBY(N_STD));
    
    p.nextState=SN_TX_ADV_DIRECT_IND;   //Go to next state	

#ifdef DBUG
    p.end = timer_time_us();   //for debugging purpose
#endif    
    return p;   
}    	       

sn_tx_adv_param_s_t sn_tx_adv(uint16_t sn_adv_ch_idx){
    sn_tx_adv_param_s_t p={0}; 
#ifdef DBUG
    p.start = timer_time_us();   //for debugging purpose
#endif
    p.sn_adv_ch_idx=sn_adv_ch_idx;        	
        			    
    p.sn_ch_freq = get_adv_ch_freq(p.sn_adv_ch_idx);   //Get the advertising channel frequency				    
	        
    ble_config(p.sn_ch_freq, 3);   //Configure ADPLL - 3 for TX
		        
    ble_send_on();   //Configure BLE for data transmission  
      
    wp_set_ch_index(p.sn_adv_ch_idx);   //Configure the whitener channel index   
    
    wp_set_aa(sn.aa);   //Set the advertising channel access address
          
    p.pdu_size = ADV_H_LEN + ADV_DIND_P_LEN;   //Configure PDU's size

    //Build PDU
    p.sn_tx_adv_pdu.h.PDU_Type=ADV_DIRECT_IND; 
    p.sn_tx_adv_pdu.h.RFU_1=ADV_DIND_H_RFU_1;	       
    p.sn_tx_adv_pdu.h.TxAdd=ADV_DIND_H_TxAdd; 
    p.sn_tx_adv_pdu.h.RxAdd=ADV_DIND_H_RxAdd;
    p.sn_tx_adv_pdu.h.Length=ADV_DIND_P_LEN; 
    p.sn_tx_adv_pdu.h.RFU_2=ADV_DIND_H_RFU_2;    
    p.sn_tx_adv_pdu.payload.AdvA=SN_DEVICE_ADDR; 
    p.sn_tx_adv_pdu.payload.InitA=BS_DEVICE_ADDR_OP; 		

#ifdef DBUG    
    p.start_tx = timer_time_us();   //for debugging purpose
#endif
					        
    ble_send((uint8_t *)&p.sn_tx_adv_pdu, p.pdu_size);   //Write the PDU to the HW/TX fifo

    //Wait for transmisstion 
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_PACKET(ADV_H_LEN,ADV_DIND_P_LEN));
    
    ble_off();   //Turn off BLE

#ifdef DBUG    
    p.boff = timer_time_us();   //for debugging purpose
#endif
  
    p.nextState=SN_RX_CONNECT_REQ;   //Go to next state 

#ifdef DBUG
    p.end = timer_time_us();   //for debugging purpose
#endif   
    return p;       
}

sn_rx_cnt_req_param_s_t sn_rx_cnt_req(uint16_t sn_adv_ch_idx){
    sn_rx_cnt_req_param_s_t p={0}; 
#ifdef DBUG
    p.start = timer_time_us();    //for debugging purpose 
#endif
    uint32_t next_adv_ch=0;
    p.sn_adv_ch_idx=sn_adv_ch_idx; p.error=-1;

    //Wait for inter frame space
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_IFS);	

    //Configure PDU's size
    p.pdu_size = ADV_H_LEN + CONNECT_REQ_P_LEN;
    ble_payload(p.pdu_size);
    			    
    p.sn_ch_freq = get_adv_ch_freq(p.sn_adv_ch_idx);   //Get the advertising channel frequency	
   
    ble_config((p.sn_ch_freq-1.0F), 2);   //Configure ADPLL - 2 for RX
    
    ble_recv_on();   //Configure BLE for data reception
   
    wp_set_ch_index(p.sn_adv_ch_idx);   //Configure the dewhitener channel index  

#ifdef DBUG    
    p.rx_on = timer_time_us();   //for debugging purpose
#endif

    //Stay on the advertising channel to listen to a connection request 
    start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_ADV);
    
    p.nbytes = ble_receive((uint8_t *)&p.sn_rx_connect_request_pdu, p.pdu_size);   //Read the PDU from the HW/RX fifo
    
    ble_off();   //Turn off BLE

#ifdef DBUG    
    p.boff = timer_time_us();   //for debugging purpose
#endif
    
    p.error=sn_advertiser_filter(p);   //Run the advertiser filter
     
    if(!p.error) {
        next_adv_ch=0;    
	p.nextState=SN_TX_DATA_TMP;   //Go to next state    
    } else{next_adv_ch=1;}
       		 
    if(next_adv_ch == 1) {
	p.sn_adv_ch_idx++;
	if(p.sn_adv_ch_idx > ADV_CH_LAST) {
           //Delay
	   start_time = timer_time_us();
           while((timer_time_us() - start_time) < (uint32_t)advDelay);
    	 	
           p.sn_adv_ch_idx=ADV_CH_FIRST;
	}   		  
	p.nextState=SN_TX_ADV_DIRECT_IND;    		  		  		      		 
    }
    
#ifdef DBUG
    p.end = timer_time_us();   //for debugging purpose
#endif     
    return p;
}	      

sn_tx_tmp_param_s_t sn_tx_data_tmp(uint32_t LLData_AA, uint64_t LLData_ChM, uint16_t LLData_WinOffset){   //procesing might be optimized in case of retransmission - te be reviewed
    sn_tx_tmp_param_s_t p={0}; uint32_t start_time=0;     
#ifdef DBUG
    p.start = timer_time_us();   //for debugging purpose
#endif
    uint64_t mask_data_ch=0x01; uint32_t k=0;  	

    if(sn.nSend == 1){   //Delay inserted after the end of the CONNECT_REQ PDU
       start_time = timer_time_us();
       while ((timer_time_us() - start_time) < (uint32_t)startTransmitWindow(LLData_WinOffset));
    }
     	          				
    for(int i=0; i<MAX_N_DATA_CHANNELS; i++){
	if(LLData_ChM & mask_data_ch) {
	     p.data_ch[k++]=(39 - i);
	}
	mask_data_ch <<= 1; 					
    }
   	       
    if(sn.nSend == 1){   //First transmission
       p.sn_data_ch_idx = p.data_ch[2];    //Set the data channel index - data ch=5 	 
    } else if(sn.nSend == 2){   //First retransmission   
       p.sn_data_ch_idx = p.data_ch[1];    //Set the data channel index - data ch=22
    } else if(sn.nSend == 3){   //Second retransmission   
       p.sn_data_ch_idx = p.data_ch[0];    //Set the data channel index - data ch=34
    }
    	
    p.sn_ch_freq = get_data_ch_freq(p.sn_data_ch_idx);   //Get the data channel frequency				    
                
    ble_config(p.sn_ch_freq, 3);   //Configure ADPLL	  
        	        
    ble_send_on();   //Configure BLE for data transmission  
   	     
    wp_set_ch_index(p.sn_data_ch_idx);   //Configure the whitener channel index   
     					       
    wp_set_aa(LLData_AA);   //Set the access address   
          
    p.pdu_size = LL_DATA_H_LEN + LL_DATA_TMP_P_LEN;    //Configure PDU's size    
      
    //Build PDU
    p.sn_tx_lldata_tmp_pdu.h.LLID=LL_DATA_PDU_SC;     
    p.sn_tx_lldata_tmp_pdu.h.NESN=sn.nextExpectedSeqNum;       
    p.sn_tx_lldata_tmp_pdu.h.SN=sn.transmitSeqNum;
    p.sn_tx_lldata_tmp_pdu.h.MD=MD_VAL_ONE;   //The SN will listen after sending its packet      
    p.sn_tx_lldata_tmp_pdu.h.RFU=0;
    p.sn_tx_lldata_tmp_pdu.h.Length=LL_DATA_TMP_P_LEN;	    
    p.sn_tx_lldata_tmp_pdu.payload=get_tmp();
        
#ifdef DBUG    
    p.start_tx = timer_time_us();   //for debugging purpose
#endif
		 	          
    ble_send((uint8_t *)&p.sn_tx_lldata_tmp_pdu, p.pdu_size);   //Write the PDU to the HW/TX fifo

    //Wait for transmisstion
    start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_PACKET(LL_DATA_H_LEN,LL_DATA_TMP_P_LEN));
    
    ble_off();   //Turn off BLE     

#ifdef DBUG    
    p.boff = timer_time_us();   //for debugging purpose
#endif
       
    p.nextState=SN_RX_DATA_ACK;   //Go to next state 

#ifdef DBUG
    p.end = timer_time_us();   //for debugging purpose
#endif
    return p;    
}

sn_rx_data_ack_param_s_t sn_rx_data_ack(uint16_t sn_data_ch_idx){
    sn_rx_data_ack_param_s_t p={0};
#ifdef DBUG
    p.start = timer_time_us();   //for debugging purpose
#endif   
    p.sn_data_ch_idx=sn_data_ch_idx;
    
    //Wait for inter frame space
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_IFS);
    
    //Configure PDU's size
    p.pdu_size = LL_DATA_H_LEN;  
    ble_payload(p.pdu_size);
    	
    p.sn_ch_freq = get_data_ch_freq(p.sn_data_ch_idx);   //Get the data channel frequency				    
      
    ble_config((p.sn_ch_freq-1.0F), 2);   //Configure ADPLL 
    
    ble_recv_on();   //Configure BLE for data reception  
   
    wp_set_ch_index(p.sn_data_ch_idx);   //Configure the dewhitener channel index		    

#ifdef DBUG    
    p.rx_on = timer_time_us();   //for debugging purpose
#endif
    
    //Delay
    start_time = timer_time_us(); 
    while((timer_time_us() - start_time) < (uint32_t)T_RX); 
          
    p.nbytes = ble_receive((uint8_t *)&p.sn_rx_data_ack_pdu, p.pdu_size);   //Read the PDU from the HW/RX fifo  
 
    ble_off();   //Turn off BLE
 
 #ifdef DBUG    
    p.boff = timer_time_us();   //for debugging purpose
#endif
  
    p.error=sn_rx_ack_check(p);
    
    //Retransmission
    if(p.error == 0){   //ACK - No retransmission needed
	sn.nSend=1;      //Reinitialize sn.nSend 
    } else {   //Retransmission needed
        sn.nSend++;         	  
    }       
    if(sn.nSend==1){p.nextState=SN_END_CONNECTION;}       
    else if(sn.nSend==2 || sn.nSend==3){p.nextState=SN_TX_DATA_TMP;}  //Retransmit previous PDU - LLID, SN, payload should be the same as the previous ones. 
    else {   //Stop sending data
        sn.nSend=1;   //Reinitialize sn.nSend      
        p.nextState=SN_END_CONNECTION;                          
    }	  
    
#ifdef DBUG
    p.end = timer_time_us();   //for debugging purpose
#endif

    return p;	      	       
} 

sn_end_cnt_param_s_t sn_end_cnt(){
    sn_end_cnt_param_s_t p={0};
#ifdef DBUG
    p.start = timer_time_us();   //for debugging purpose
#endif
    sn.transmitSeqNum=0; sn.nextExpectedSeqNum=0;   //Reinitialize SN and NESN - Keep it even if not incremented in this version 
    p.nextState=SN_STANDBY; 
    
#ifdef DBUG
    p.end = timer_time_us();   //for debugging purpose
#endif    

    return p; 	
}


