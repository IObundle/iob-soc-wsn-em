#include "iob_timer.h"
#include "cm_def.h"
#include "bs_def.h"
#include "ble.h"

#define CST 1
#include "bs_config.h"

bs_s_t bs = {.id = BS_ID, .aa = 0x8E89BED6, .nextState = MODE_BS_STANDBY, .isBusy=0};   //By default, BS is in STANDBY mode

uint32_t aa_plist[MAX_N_SN][2]={{AA_FREE, 0xA3B9C1E5}, {AA_FREE, 0x5E2C419D}, {AA_FREE, 0x3D5C8A2E}, {AA_FREE, 0xE1B79C3A}, {AA_FREE, 0xC85B3D1E}, 
			        {AA_FREE, 0x4CA3E1B9}, {AA_FREE, 0x7AD92C5B}, {AA_FREE, 0xB6CEA18A}, {AA_FREE, 0x2D5A7B68}, {AA_FREE, 0x9E1A4CB7}};    

uint64_t bs_whitelist[MAX_N_SN][4] = {{SN_DEVIVCE_ADDR_OP, DA_FREE, 0, 0}, {0xF2FFFFFF, DA_FREE, 0, 0}, 
  				      {0xF3FFFFFF, DA_FREE, 0, 0}, {0xF4FFFFFF, DA_FREE, 0, 0}, 
				      {0xF5FFFFFF, DA_FREE, 0, 0}, {0xF6FFFFFF, DA_FREE, 0, 0},
				      {0xF5FFFFFF, DA_FREE, 0, 0}, {0xF6FFFFFF, DA_FREE, 0, 0},
				      {0xF5FFFFFF, DA_FREE, 0, 0}, {0xF6FFFFFF, DA_FREE, 0, 0}};
uint8_t enable_scan_all=FALSE;

//------------------------------------------------------------------------------------------------------------------------------------------
int32_t bs_initiator_filter(bs_rx_adv_param_s_t p){
    int32_t da_found=0, error=-1;    
    if ((p.nbytes - CRC_LEN) == p.pdu_size) { //There is data in the RX buffer 		      								     
	  if(p.bs_rx_adv_pdu.pdu_adv_ind_h.Length==ADV_DIND_P_LEN){
	    if(p.bs_rx_adv_pdu.pdu_adv_ind_payload.InitA==BS_DEVIVCE_ADDR){ //The packet is addressed to the BS
	      for(int i=0; i<MAX_N_SN; i++) {
	           if (bs_whitelist[i][0] == p.bs_rx_adv_pdu.pdu_adv_ind_payload.AdvA) { //The advertiser device address is found
	      		da_found=1;
			if(bs_whitelist[i][1] == DA_FREE){ //This advertiser is new
			    bs_whitelist[i][1]=AA_ATTR; 
			 } 
			 i=MAX_N_SN;  
     	           }
	      }
            } else {error=3;} 	
	  } else {error=2;}
	  
	  if(da_found==1){
	     if (p.bs_rx_adv_pdu.pdu_adv_ind_h.PDU_Type == ADV_DIRECT_IND) { //It is a connectable directed advertising packet       
    	         error=0;	         
    	     } else {       
    	         error=5;
    	     }
	  } else if(da_found==0){error=4;}   
     } else {
  	  error=1;    	  
     }      
     return error;
}      		  
//------------------------------------------------------------------------------------------------------------------------------------------
bs_standby_param_s_t bs_standby(){
    bs_standby_param_s_t p={0};
    
    //System Init
    sys_init();							    

    //Turn off BLE
    ble_off();
 
    //Delay
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)T_STANDBY);				

    //Configure avdertising channels starting index
    p.adv_ch_start_idx=ADV_CH_FIRST;  
    
    //Go to next state
    p.nextState=MODE_BS_RX_ADV_DIRECT_IND; 
    
    return p;
}

bs_tx_cnt_req_param_s_t bs_tx_cnt_req(uint16_t bs_adv_ch_idx, uint64_t AdvA){
    bs_tx_cnt_req_param_s_t p={0};
    p.bs_adv_ch_idx=bs_adv_ch_idx;
    
    //Wait for inter frame space
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)T_IFS);			  

    //Get the advertising channel frequency   
    p.bs_ch_freq = get_adv_ch_freq(p.bs_adv_ch_idx);
    
    //Configure ADPLL
    ble_config(p.bs_ch_freq, 3);

    //Configure BLE for data transmission
    ble_send_on();

    //Configure the whitener channel index
    wp_set_ch_index(p.bs_adv_ch_idx);  
	
    //Configure PDU's size
    p.pdu_size = ADV_H_LEN + CONNECT_REQ_P_LEN;

    //Build PDU
    p.bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type=CONNECT_REQ; p.bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1=CONNECT_REQ_H_RFU_1;      
    p.bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd=CONNECT_REQ_H_TxAdd; p.bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd=CONNECT_REQ_H_RxAdd;	      
    p.bs_tx_connect_request_pdu.pdu_adv_ind_h.Length=CONNECT_REQ_P_LEN; p.bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2=CONNECT_REQ_H_RFU_2;	  
	      
    p.bs_tx_connect_request_pdu.connect_req_payload.InitA=BS_DEVIVCE_ADDR;	    
    p.bs_tx_connect_request_pdu.connect_req_payload.AdvA=AdvA;

    for(int i=0; i<MAX_N_SN; i++) {
	if (aa_plist[i][0] == AA_FREE) { //Free access address 
	       p.bs_tx_connect_request_pdu.connect_req_payload.LLData_AA=aa_plist[i][1];
	       bs_whitelist[i][0]=AA_ATTR;    //to get back to AA_FREE once finished
     	       i=MAX_N_SN;
	}
    }			   
	      
    p.bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit=0x555555;    
    p.bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize=1; p.bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset=2;	   
    p.bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval=3; p.bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency=0;		 
    p.bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout=5; p.bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM=0x400400020;		  
    p.bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop=2; p.bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA=1; 	      
	      	      			    	      	       	   	      	      
    //Write the PDU to the HW
    ble_send((unsigned char *)&p.bs_tx_connect_request_pdu, p.pdu_size);  

    //Wait for transmisstion
    start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);

    //Turn off BLE
    ble_off();
    
    p.nextState=MODE_BS_RX_DATA_GPS;     
    
    return p;
}     	      	     

bs_rx_adv_param_s_t bs_rx_adv(uint16_t bs_adv_ch_idx){
    bs_rx_adv_param_s_t p={0}; 		
    p.bs_adv_ch_idx=bs_adv_ch_idx; p.error=-1;    
          
    //Get the advertising channel frequency   
    p.bs_ch_freq = get_adv_ch_freq(p.bs_adv_ch_idx);
    
    //Configure PDU's size
    p.pdu_size = ADV_H_LEN + ADV_DIND_P_LEN;	    
    ble_payload(p.pdu_size);  //CRC length is added by ble_payload() - for debugging purpose when needed

    //Configure ADPLL	
    ble_config((p.bs_ch_freq-1.0F), 2);

    //Configure BLE for data reception
    ble_recv_on();

    //Configure the dewhitener channel index
    wp_set_ch_index(p.bs_adv_ch_idx);      
    
    //Stay on the advertising channel to listen, during the scan window, for a connectable directed advertising event
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)T_ScanWindow(N_SW));
        
    //Read the PDU from the HW 
    p.nbytes = ble_receive((unsigned char *)&p.bs_rx_adv_pdu, p.pdu_size);
 
    //Turn off BLE
    ble_off();    

    //Run the initiator filter
    p.error=bs_initiator_filter(p);
    	
    //Go to next state
    if(p.error==0) {
        p.nextState=MODE_BS_TX_CONNECT_REQ;
    } else {
	if(enable_scan_all==TRUE){p.bs_adv_ch_idx++; if(p.bs_adv_ch_idx>ADV_CH_LAST) {p.bs_adv_ch_idx=ADV_CH_FIRST;}}	
	while ((timer_time_us() - start_time) < (unsigned int)T_ScanInterval(N_SI,N_SW));
        p.nextState=MODE_BS_RX_ADV_DIRECT_IND;
    }
     	      
    return p;
}        

bs_rx_gps_param_s_t bs_rx_data_gps(uint32_t LLData_AA){  	
    bs_rx_gps_param_s_t p={0};
         
    //Set data channel index
    p.bs_data_ch_idx=5;  //temporarily for debugging purpose

    //Get the data channel frequency	
    p.bs_ch_freq = get_data_ch_freq(p.bs_data_ch_idx);

    //Configure PDU's size
    p.pdu_size_1 = LL_DATA_H_LEN + LL_DATA_GPS_P_LEN;	 
    ble_payload(p.pdu_size_1);

    //Configure ADPLL   
    ble_config((p.bs_ch_freq-1.0F), 2);

    //Configure BLE for data reception
    ble_recv_on();

    //Configure the dewhitener channel index
    wp_set_ch_index(p.bs_data_ch_idx);  

    //Set BS RX Access Address
    wp_set_aa(LLData_AA);  

    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)T_ScanWindow(N_SW));	//delay to be changed

    //Read the PDU from the HW
    p.pdu_size_1 += CRC_LEN;
    for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_rx[i] = 0; }
    p.nbytes = ble_receive(bs.buffer_rx, p.pdu_size_1);

    //Turn off BLE
    ble_off();			
    
    if (p.nbytes == p.pdu_size_1) { //There is data in the RX buffer
        for (int i = 0; i < (p.pdu_size_1 - CRC_LEN); i++) {
	  	((unsigned char *)&p.bs_rx_data_gps_pdu)[i] = bs.buffer_rx[i];
  	}           	           	
	p.result=1;	  
	p.nextState=MODE_BS_TX_ACK_GPS;    
     }	else {	 
        p.result=2;  
	p.nextState=0; 
     }	      	       

     return p;
}     

bs_tx_llcontrol_param_s_t bs_tx_llcontrol(uint32_t bs_ch_freq, uint16_t bs_data_ch_idx, uint32_t gps_tmp){
    bs_tx_llcontrol_param_s_t p={0};

    //Configure ADPLL
    ble_config(bs_ch_freq, 3);		 
		   
    //Configure BLE for data transmission
    ble_send_on();  
   
   //Configure the whitener channel index
   wp_set_ch_index(bs_data_ch_idx);   
   
   //Configure PDU's size    
   p.pdu_size_1 = LL_DATA_H_LEN + LL_CONTROL_P_LEN;
     
   //Build PDU
   p.bs_tx_pdu_llcontrol.pdu_lldata_h.LLID=LL_CONTROL_PDU; p.bs_tx_pdu_llcontrol.pdu_lldata_h.RFU=0;
   p.bs_tx_pdu_llcontrol.pdu_lldata_h.NESN=0; p.bs_tx_pdu_llcontrol.pdu_lldata_h.SN=0;
   p.bs_tx_pdu_llcontrol.pdu_lldata_h.MD=0; p.bs_tx_pdu_llcontrol.pdu_lldata_h.Length=LL_CONTROL_P_LEN;	
   
   if(gps_tmp==1){      
   	p.bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode=LL_CONNECTION_ACK;
   	p.bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData=POSITIVE_ACK;		       
   } else if(gps_tmp==2){
        p.bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode=LL_TERMINATE_IND;
        p.bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData=END_CONNECTION;      
   } 
    					   
   //Write the PDU to the HW
   ble_send((unsigned char *)&p.bs_tx_pdu_llcontrol, p.pdu_size_1);

   // Wait for transmisstion
   uint32_t start_time = timer_time_us();
   while ((timer_time_us() - start_time) < (unsigned int)1000);

   //Turn off BLE
   ble_off();	  
   
   if(gps_tmp==1){p.nextState=MODE_BS_RX_DATA_TMP;}
   else if(gps_tmp==2){p.nextState=0;}
   
   return p; 
}

bs_rx_tmp_param_s_t bs_rx_data_tmp(uint16_t bs_data_ch_idx, uint32_t bs_ch_freq){ 
    bs_rx_tmp_param_s_t p={0};	
    
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)1000);   //temporarily setting

    //Configure PDU's size
    p.pdu_size_1 = LL_DATA_H_LEN + LL_DATA_TMP_P_LEN;    
    ble_payload(p.pdu_size_1);

    //Configure ADPLL   
    ble_config((bs_ch_freq-1.0F), 2);

    //Configure BLE for data reception
    ble_recv_on();

    //Configure the dewhitener channel index
    wp_set_ch_index(bs_data_ch_idx);  

    start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (unsigned int)T_ScanWindow(N_SW));   //delay to be changed

    //Read the PDU from the HW
    p.pdu_size_1 += CRC_LEN;
    for (int i = 0; i < MAX_N_BYTES; i++) { bs.buffer_rx[i] = 0; }
    p.nbytes = ble_receive(bs.buffer_rx, p.pdu_size_1);

    //Turn off BLE
    ble_off();

    if (p.nbytes == p.pdu_size_1) { //There is data in the RX buffer
        for (int i = 0; i < (p.pdu_size_1 - CRC_LEN); i++) {
 	     ((unsigned char *)&p.bs_rx_lldata_tmp_pdu)[i] = bs.buffer_rx[i];
        }	  
        p.result=1;
        p.nextState=MODE_BS_TX_END_CONNECTION;				 
    } else {
        p.result=2;
        p.nextState=0; 
    }	    
	
    return p;
} 
	
