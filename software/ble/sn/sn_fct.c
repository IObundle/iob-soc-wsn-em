#include "iob_timer.h"
#include "cm_def.h"
#include "sn_def.h"
#include "ble.h"

#define CST 1
#include "sn_config.h"

sn_s_t sn = {.id = SN_ID, .aa = 0x8E89BED6, .adv = 1, .nextState = MODE_SN_STANDBY};    //By default, SN is in STANDBY mode

uint32_t sn_standby(uint8_t adv){
    //System Init
    sys_init(); 	    
    
    //Turn off BLE
    ble_off();  		    
    
    //Delay
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)t_standby);
	     
    //Go to next state
    return MODE_SN_TX_ADV_DIRECT_IND;	   
}    	       

sn_tx_adv_param_s_t sn_tx_adv_ind(uint16_t sn_adv_ch_idx){
    sn_tx_adv_param_s_t p={0}; 
    	
    //Get the advertising channel frequency			    
    p.sn_ch_freq = get_adv_ch_freq(sn_adv_ch_idx);				    
	    
    //Configure ADPLL
    ble_config(p.sn_ch_freq, 3);   //3 for TX
		    
    //Configure BLE for data transmission
    ble_send_on();  
    
    //Configure the whitener channel index
    wp_set_ch_index(sn_adv_ch_idx);   
    
    //Configure PDU's size   
    p.pdu_size = ADV_H_LEN + ADV_DIND_P_LEN;

    //Build PDU
    p.sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type=ADV_DIRECT_IND; p.sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1=ADV_DIND_H_RFU_1;	   
    p.sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd=ADV_DIND_H_TxAdd; p.sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd=ADV_DIND_H_RxAdd;
    p.sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length=ADV_DIND_P_LEN; p.sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2=ADV_DIND_H_RFU_2;    
    p.sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA=SN_DEVIVCE_ADDR; p.sn_tx_adv_ind_pdu.pdu_adv_ind_payload.InitA=BS_DEVIVCE_ADDR_OP;	  	    
					    
    //Write the PDU to the HW
    ble_send((unsigned char *)&p.sn_tx_adv_ind_pdu, p.pdu_size);

    //Wait for transmisstion
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);

    //Turn off BLE
    ble_off();
   
    p.nextState=MODE_SN_RX_CONNECT_REQ; 
   
    return p;       
}

sn_rx_cnt_req_param_s_t sn_rx_cnt_req(uint16_t sn_adv_ch_idx, uint32_t sn_ch_freq){
    sn_rx_cnt_req_param_s_t p={0}; uint32_t next_adv_ch=0;
    p.sn_adv_ch_idx=sn_adv_ch_idx;
 		
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)5000);    //temporarily added delay

    //Configure PDU's size
    p.pdu_size = ADV_H_LEN + CONNECT_REQ_P_LEN;
    ble_payload(p.pdu_size);

    //Configure ADPLL
    ble_config((sn_ch_freq-1.0F), 2);   //2 for RX

    // Configure BLE for data reception
    ble_recv_on();

    //Configure the dewhitener channel index
    wp_set_ch_index(p.sn_adv_ch_idx);  

    //Stay on the advertising channel to listen to a connection request 
    start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)advDelay);

    //Read the PDU from the HW
    p.pdu_size += CRC_LEN;
    for (int i = 0; i < MAX_N_BYTES; i++) { sn.buffer_rx[i] = 0; }
    p.nbytes = ble_receive(sn.buffer_rx, p.pdu_size);

    //Turn off BLE
    ble_off();

    if (p.nbytes == p.pdu_size) { //There is data in the RX buffer
        for (int i = 0; i < (p.pdu_size - CRC_LEN); i++) {
	     ((unsigned char *)&p.sn_rx_connect_request_pdu)[i] = sn.buffer_rx[i];
  	} 
	if (p.sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type == CONNECT_REQ) {  //If a connection request is received
	     p.result=1;
	     next_adv_ch=0; 
	     p.sn_adv=1;  //might be removed in next versions			   
	     p.nextState=MODE_SN_TX_DATA_GPS; 
	     //p.nextState=0; 
	} else {
	     p.result=2;
	     next_adv_ch = 1;
	}		
     } else {
	p.result=3;
	next_adv_ch = 1;
     }		 		 
     if (next_adv_ch == 1) {
	p.sn_adv_ch_idx++;
	if (p.sn_adv_ch_idx > 39 ) { 
	    p.sn_adv_ch_idx = ADV_CH_FIRST;	
	    p.sn_adv=1;
	    //p.nextState=MODE_SN_STANDBY;    //temporarily settings
	    p.nextState=0; 
	} else {
	   p.nextState=MODE_SN_TX_ADV_DIRECT_IND;
	   //p.nextState=0; 
	}		  		  		      		 
     }
     
     return p;
}	      

sn_tx_gps_param_s_t sn_tx_data_gps(uint32_t LLData_AA, uint64_t LLData_ChM){
    sn_tx_gps_param_s_t p={0}; uint64_t mask_data_ch=0x01;  	
 	      
    uint32_t k=0;				
    for (int i=0; i<MAX_N_DATA_CHANNELS; i++) {
	if (LLData_ChM & mask_data_ch) {
	     p.data_ch[k++]=i;
	}
	mask_data_ch <<= 1; 					
    }

    //Initialize Sequence Number and Next Expected Sequence Number
    uint32_t transmitSeqNum=0;	    //for packets sent by the Link Layer
    uint32_t nextExpectedSeqNum=0;  //used by the peer to acknowledge the last data channel pdu sent or to request resending the last data channel pdu sent

    //Set data channel index - temporarily setting	       
    p.sn_data_ch_idx = 5; 	 
    
    //Get the data channel frequency	
    p.sn_ch_freq = get_data_ch_freq(p.sn_data_ch_idx);				    
            
    //Configure ADPLL
    ble_config(p.sn_ch_freq, 3);	  
        	    
    //Configure BLE for data transmission
    ble_send_on();  
   	 
    //Configure the whitener channel index
    wp_set_ch_index(p.sn_data_ch_idx);   
     					   
    //Set SN TX Access Address
    wp_set_aa(LLData_AA);  
    
    // Configure PDU's size   
    p.pdu_size_1 = LL_DATA_H_LEN + LL_DATA_GPS_P_LEN;    
      
    //Build PDU
    transmitSeqNum++;  //NSN, SN and LLID to be adjusted
    p.sn_tx_data_gps_pdu.pdu_lldata_h.LLID=LL_DATA_PDU_CE; p.sn_tx_data_gps_pdu.pdu_lldata_h.RFU=0;
    p.sn_tx_data_gps_pdu.pdu_lldata_h.NESN=nextExpectedSeqNum; p.sn_tx_data_gps_pdu.pdu_lldata_h.SN=transmitSeqNum;
    p.sn_tx_data_gps_pdu.pdu_lldata_h.MD=1; p.sn_tx_data_gps_pdu.pdu_lldata_h.Length=LL_DATA_GPS_P_LEN;	    

    p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_D=sn_gps_coordinates.DMS_Lat_D; p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_M=sn_gps_coordinates.DMS_Lat_M;
    p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_S=sn_gps_coordinates.DMS_Lat_S; p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_C=sn_gps_coordinates.DMS_Lat_C;
    p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_D=sn_gps_coordinates.DMS_Lng_D; p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_M=sn_gps_coordinates.DMS_Lng_M;
    p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_S=sn_gps_coordinates.DMS_Lng_S; p.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_C=sn_gps_coordinates.DMS_Lng_C;
		 	      
    //Write the PDU to the HW
    ble_send((unsigned char *)&p.sn_tx_data_gps_pdu, p.pdu_size_1);

    //Wait for transmisstion
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);

    //Turn off BLE
    ble_off();     
    
    //Go to next state
    p.nextState=MODE_SN_RX_ACK_GPS; 

    return p;    
}

sn_rx_llcontrol_param_s_t sn_rx_llcontrol(uint16_t sn_data_ch_idx, uint32_t sn_ch_freq, uint8_t gps_tmp) {
    sn_rx_llcontrol_param_s_t p={0};
    
    //Delay - temporarily setting
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1500);
    
    //Configure PDU's size
    p.pdu_size_1 = LL_DATA_H_LEN + LL_CONTROL_P_LEN;  
    ble_payload(p.pdu_size_1);

    //Configure ADPLL   
    ble_config((sn_ch_freq-1.0F), 2);

    //Configure BLE for data reception
    ble_recv_on();

    //Configure the dewhitener channel index
    wp_set_ch_index(sn_data_ch_idx);		    
    
    //Delay - temporarily setting
    start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)10000);  
    
    //Read the PDU from the HW
    p.pdu_size_1 += CRC_LEN;
    for (int i = 0; i < MAX_N_BYTES; i++) { sn.buffer_rx[i] = 0; }
    p.nbytes = ble_receive(sn.buffer_rx, p.pdu_size_1);

    //Turn off BLE
    ble_off();
   
    if (p.nbytes == p.pdu_size_1) { //There is data in the RX buffer
  	for (int i = 0; i < (p.pdu_size_1 - CRC_LEN); i++) {
	    ((unsigned char *)&p.sn_rx_pdu_llcontrol)[i] = sn.buffer_rx[i];
        }    	 
	if(gps_tmp==1){if(p.sn_rx_pdu_llcontrol.pdu_llcontrol_payload.CtrData==POSITIVE_ACK) {p.result=1; p.nextState=MODE_SN_TX_DATA_TMP;}}  
	else if(gps_tmp==2){if(p.sn_rx_pdu_llcontrol.pdu_llcontrol_payload.CtrData==END_CONNECTION) {p.result=2; p.nextState=0;}}   
    } else {
        p.result=3;	   
	p.nextState=0; 
    }
     
    return p;	      	       
} 

sn_tx_tmp_param_s_t sn_tx_data_tmp(uint16_t sn_data_ch_idx, uint32_t sn_ch_freq, int16_t sn_data_tmp){
     sn_tx_tmp_param_s_t p={0};

     //Configure ADPLL
     ble_config(sn_ch_freq, 3);	       
		 
     //Configure BLE for data transmission
     ble_send_on();  
 
    //Configure the whitener channel index
    wp_set_ch_index(sn_data_ch_idx);						 
 
    //Configure PDU's size   
    p.pdu_size_1 = LL_DATA_H_LEN + LL_DATA_TMP_P_LEN;
   
    //Build PDU
    //NSN and SN to be adjusted
    p.sn_tx_lldata_tmp_pdu.pdu_lldata_h.LLID=LL_DATA_PDU_SC; p.sn_tx_lldata_tmp_pdu.pdu_lldata_h.RFU=0;
    p.sn_tx_lldata_tmp_pdu.pdu_lldata_h.NESN=0; p.sn_tx_lldata_tmp_pdu.pdu_lldata_h.SN=0;
    p.sn_tx_lldata_tmp_pdu.pdu_lldata_h.MD=0; p.sn_tx_lldata_tmp_pdu.pdu_lldata_h.Length=LL_DATA_TMP_P_LEN;
     
    p.sn_tx_lldata_tmp_pdu.payload_tmp=sn_data_tmp;  //temporarily for debugging purpose 				 
				 
    //Write the PDU to the HW
    ble_send((unsigned char *)&p.sn_tx_lldata_tmp_pdu, p.pdu_size_1);

    //Wait for transmisstion
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);

    //Turn off BLE
    ble_off();	
    
    p.nextState=MODE_SN_RX_END_CONNECTION;
    
    return p;
}    

