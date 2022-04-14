#include "iob-uart.h"
#include "iob_timer.h"
#include "cm_def.h"
#include "bs_def.h"
#include "ble.h"

#define CST 1
#include "bs_config.h"

bs_s_t bs = {.id = BS_ID, .aa = 0x8E89BED6, .isCh_aa = FALSE, .nextState = BS_STANDBY, .isBusy=FALSE};   //By default, BS is in STANDBY state

//Temporary setting for debugging purpose
uint32_t aa_plist[MAX_N_SN][2]={{FALSE, 0xA3B9C1E5}, {FALSE, 0x5E2C419D}, {FALSE, 0x3D5C8A2E}, {FALSE, 0xE1B79C3A}, {FALSE, 0xC85B3D1E}, 
			        {FALSE, 0x4CA3E1B9}, {FALSE, 0x7AD92C5B}, {FALSE, 0xB6CEA18A}, {FALSE, 0x2D5A7B68}, {FALSE, 0x9E1A4CB7}};    

gps_coordinates_s_t gps[MAX_N_SN]={0};
uint32_t temperature[MAX_N_SN][SAMPLING_RATE]={0};

uint8_t enable_scan_all=FALSE;

//------------------------------------------------------------------------------------------------------------------------------------------
init_filter_s_t bs_initiator_filter(bs_rx_adv_param_s_t p){
    init_filter_s_t pk={0};   
    if(p.nbytes == (p.pdu_size + CRC_LEN)) { //There is data in the RX buffer (pdu + crc) 		      								     
	  if(p.bs_rx_adv_pdu.h.Length == ADV_DIND_P_LEN) {
	    if(p.bs_rx_adv_pdu.payload.InitA == BS_DEVICE_ADDR) { //The packet is addressed to the BS
	       for(int i=0; i<MAX_N_SN; i++){
	           if(bs_whitelist[i] == p.bs_rx_adv_pdu.payload.AdvA) { //The advertiser device address is found
		      if(p.bs_rx_adv_pdu.h.PDU_Type == ADV_DIRECT_IND) { //It is a connectable directed advertising packet 
		         pk.error=0; pk.da_index=i;  
		      } else {pk.error=5;}		
     	           } else {pk.error=4;}
		   i=MAX_N_SN;
	       }
            } else {pk.error=3;} 	
	  } else {pk.error=2;}	  
    } else {pk.error=1;}
    
    return pk;
}      		  
//------------------------------------------------------------------------------------------------------------------------------------------
bs_standby_param_s_t bs_standby(uint8_t ch_aa){
    bs_standby_param_s_t p={0};
            
    sys_init();   //System Init							    

#ifdef DBUG
    uint32_t start_time_debug = timer_time_us();   //for debugging purpose
#endif
    
    ble_off();   //Turn off BLE
    
    p.adv_ch_start_idx=ADV_CH_FIRST;   //Configure advertising channels starting index  
          
    if(ch_aa){wp_set_aa(bs.aa);}   //Reinitialize the advertising channel access address if it has been changed
     
    //Delay
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_STANDBY);				
       
    p.nextState=BS_RX_ADV_DIRECT_IND;   //Go to next state 

#ifdef DBUG
    p.tt=timer_time_us() - start_time_debug;   //for debugging purpose
#endif  
    
    return p;
}

bs_tx_cnt_req_param_s_t bs_tx_cnt_req(uint16_t bs_adv_ch_idx, uint64_t AdvA){
#ifdef DBUG
    uint32_t start_time_debug = timer_time_us();   //for debugging purpose
#endif
    bs_tx_cnt_req_param_s_t p={0};
    p.bs_adv_ch_idx=bs_adv_ch_idx;
    
    //Wait for inter frame space
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (uint32_t)T_IFS);			  
       
    p.bs_ch_freq = get_adv_ch_freq(p.bs_adv_ch_idx);   //Get the advertising channel frequency
        
    ble_config(p.bs_ch_freq, 3);   //Configure ADPLL
    
    ble_send_on();   //Configure BLE for data transmission
    
    wp_set_ch_index(p.bs_adv_ch_idx);   //Configure the whitener channel index  
	    
    p.pdu_size = ADV_H_LEN + CONNECT_REQ_P_LEN;   //Configure PDU's size

    //Build PDU
    p.bs_tx_connect_request_pdu.h.PDU_Type=CONNECT_REQ; 
    p.bs_tx_connect_request_pdu.h.RFU_1=CONNECT_REQ_H_RFU_1;	  
    p.bs_tx_connect_request_pdu.h.TxAdd=CONNECT_REQ_H_TxAdd; 
    p.bs_tx_connect_request_pdu.h.RxAdd=CONNECT_REQ_H_RxAdd;	  
    p.bs_tx_connect_request_pdu.h.Length=CONNECT_REQ_P_LEN; 
    p.bs_tx_connect_request_pdu.h.RFU_2=CONNECT_REQ_H_RFU_2;      	      
    p.bs_tx_connect_request_pdu.payload.InitA=BS_DEVICE_ADDR;	    
    p.bs_tx_connect_request_pdu.payload.AdvA=AdvA;

    for(int i=0; i<MAX_N_SN; i++){
	if (aa_plist[i][0] == FALSE) { //Free access address 
	       p.bs_tx_connect_request_pdu.payload.LLData_AA=aa_plist[i][1];
	       aa_plist[i][0]=TRUE;
     	       i=MAX_N_SN;
	}
    }			   
	      
    p.bs_tx_connect_request_pdu.payload.LLData_CRCInit=0x555555;    
    p.bs_tx_connect_request_pdu.payload.LLData_WinSize=1; 
    p.bs_tx_connect_request_pdu.payload.LLData_WinOffset=2;	       
    p.bs_tx_connect_request_pdu.payload.LLData_Interval=3; 
    p.bs_tx_connect_request_pdu.payload.LLData_Latency=0;		     
    p.bs_tx_connect_request_pdu.payload.LLData_Timeout=5; 
    p.bs_tx_connect_request_pdu.payload.LLData_ChM=0x400400020;	      
    p.bs_tx_connect_request_pdu.payload.LLData_Hop=2; 
    p.bs_tx_connect_request_pdu.payload.LLData_SCA=1;		  
	      	      			    	      	       	   	      	          
    ble_send((uint8_t *)&p.bs_tx_connect_request_pdu, p.pdu_size);   //Write the PDU to the HW  

    //Wait for transmisstion
    start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_PACKET(ADV_H_LEN,CONNECT_REQ_P_LEN));
    
    ble_off();   //Turn off BLE
    
    p.nextState=BS_RX_DATA_GPS;   //Go to next state     

#ifdef DBUG
    p.tt=timer_time_us() - start_time_debug;    //for debugging purpose
#endif  
    
    return p;
}     	      	     

bs_rx_adv_param_s_t bs_rx_adv(uint16_t bs_adv_ch_idx){
#ifdef DBUG
    uint32_t start_time_debug = timer_time_us();   //for debugging purpose
#endif
    bs_rx_adv_param_s_t p={0}; 		
    p.bs_adv_ch_idx=bs_adv_ch_idx; p.error=-1;    
                
    p.bs_ch_freq = get_adv_ch_freq(p.bs_adv_ch_idx);   //Get the advertising channel frequency
    
    //Configure PDU's size
    p.pdu_size = ADV_H_LEN + ADV_DIND_P_LEN;	    
    ble_payload(p.pdu_size);
  	
    ble_config((p.bs_ch_freq-1.0F), 2);   //Configure ADPLL
    
    ble_recv_on();   //Configure BLE for data reception
   
    wp_set_ch_index(p.bs_adv_ch_idx);   //Configure the dewhitener channel index      
    
    //Stay on the advertising channel to listen for a connectable directed advertising packet
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_ScanWindow(N_SW));
             
    p.nbytes = ble_receive((uint8_t *)&p.bs_rx_adv_pdu, p.pdu_size);   //Read the PDU from the HW
     
    ble_off();   //Turn off BLE    
    
    init_filter_s_t pk=bs_initiator_filter(p);   //Run the initiator filter
    p.error=pk.error;
    	
    //Go to next state
    if(p.error==0) {
        p.nextState=BS_TX_CONNECT_REQ;
    } else {
	if(enable_scan_all==TRUE){p.bs_adv_ch_idx++; if(p.bs_adv_ch_idx>ADV_CH_LAST){p.bs_adv_ch_idx=ADV_CH_FIRST;}}	
	while((timer_time_us() - start_time) < (uint32_t)T_ScanInterval(N_SI,N_SW));
        p.nextState=BS_RX_ADV_DIRECT_IND;
    }

#ifdef DBUG
    p.tt=timer_time_us() - start_time_debug;   //for debugging purpose
#endif  
     	      
    return p;
}        

bs_rx_gps_param_s_t bs_rx_data_gps(uint32_t LLData_AA){  	
#ifdef DBUG
    uint32_t start_time_debug = timer_time_us();   //for debugging purpose
#endif
    bs_rx_gps_param_s_t p={0};
             
    p.bs_data_ch_idx=5;  //Set data channel index - temporary setting for debugging purpose
    	
    p.bs_ch_freq = get_data_ch_freq(p.bs_data_ch_idx);   //Get the data channel frequency

    //Configure PDU's size
    p.pdu_size = LL_DATA_H_LEN + LL_DATA_GPS_P_LEN;	 
    ble_payload(p.pdu_size);
       
    ble_config((p.bs_ch_freq-1.0F), 2);   //Configure ADPLL
    
    ble_recv_on();   //Configure BLE for data reception
    
    wp_set_ch_index(p.bs_data_ch_idx);   //Configure the dewhitener channel index  
   
    wp_set_aa(LLData_AA);   //Set the access address
    p.ch_aa=TRUE;  

    //Delay - temporary setting
    uint32_t start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)10000);	
    
    p.nbytes = ble_receive((uint8_t *)&p.bs_rx_data_gps_pdu, p.pdu_size);   //Read the PDU from the HW
    
    ble_off();   //Turn off BLE			
    
    if (p.nbytes == (p.pdu_size + CRC_LEN)) { //There is data in the RX buffer (pdu +crc)         	           	
	p.error=0;	  
	p.nextState=BS_TX_ACK_GPS;    
     }	else {	 
        p.error=1;  
	p.nextState=0; 
     }	      	       

#ifdef DBUG
    p.tt=timer_time_us() - start_time_debug;   //for debugging purpose
#endif  

    return p;
}     

bs_tx_llcontrol_param_s_t bs_tx_llcontrol(uint16_t bs_data_ch_idx, uint32_t gps_tmp){
#ifdef DBUG
    uint32_t start_time_debug = timer_time_us();   //for debugging purpose
#endif
    bs_tx_llcontrol_param_s_t p={0};
    p.bs_data_ch_idx=bs_data_ch_idx;

    //Wait for inter frame space
    uint32_t start_time = timer_time_us();
    while ((timer_time_us() - start_time) < (uint32_t)T_IFS);			  
    	
    p.bs_ch_freq = get_data_ch_freq(p.bs_data_ch_idx);   //Get the data channel frequency
    
    ble_config(p.bs_ch_freq, 3);   //Configure ADPLL		 
		       
    ble_send_on();   //Configure BLE for data transmission  
      
    wp_set_ch_index(p.bs_data_ch_idx);   //Configure the whitener channel index   
         
    p.pdu_size = LL_DATA_H_LEN + LL_CONTROL_P_LEN;   //Configure PDU's size 
     
    //Build PDU
    p.bs_tx_pdu_llcontrol.h.LLID=LL_CONTROL_PDU; 
    p.bs_tx_pdu_llcontrol.h.NESN=0; 
    p.bs_tx_pdu_llcontrol.h.SN=0;
    p.bs_tx_pdu_llcontrol.h.MD=0; 
    p.bs_tx_pdu_llcontrol.h.RFU=0;
    p.bs_tx_pdu_llcontrol.h.Length=LL_CONTROL_P_LEN; 
   
    if(gps_tmp==1){      
   	p.bs_tx_pdu_llcontrol.payload.Opcode=LL_CONNECTION_ACK;
   	p.bs_tx_pdu_llcontrol.payload.CtrData=POSITIVE_ACK;			 
    } else if(gps_tmp==2){
        p.bs_tx_pdu_llcontrol.payload.Opcode=LL_TERMINATE_IND;
        p.bs_tx_pdu_llcontrol.payload.CtrData=END_CONNECTION;	   
    } 
    					       
    ble_send((uint8_t *)&p.bs_tx_pdu_llcontrol, p.pdu_size);   //Write the PDU to the HW/TX fifo

    // Wait for transmisstion
    start_time = timer_time_us();
    while((timer_time_us() - start_time) < (uint32_t)T_PACKET(LL_DATA_H_LEN,LL_CONTROL_P_LEN));
   
    ble_off();   //Turn off BLE	  
   
    if(gps_tmp==1){p.nextState=BS_RX_DATA_TMP;}
    else if(gps_tmp==2){p.nextState=0;}

#ifdef DBUG
     p.tt=timer_time_us() - start_time_debug;    //for debugging purpose
#endif 
   
     return p; 
}

bs_rx_tmp_param_s_t bs_rx_data_tmp(uint16_t bs_data_ch_idx){ 
#ifdef DBUG
     uint32_t start_time_debug = timer_time_us();   //for debugging purpose
#endif
     bs_rx_tmp_param_s_t p={0};	
     p.bs_data_ch_idx=bs_data_ch_idx;

     //Wait for inter frame space
     uint32_t start_time = timer_time_us();
     while((timer_time_us() - start_time) < (uint32_t)T_IFS);			  

     //Configure PDU's size
     p.pdu_size = LL_DATA_H_LEN + LL_DATA_TMP_P_LEN;    
     ble_payload(p.pdu_size);
    	
     p.bs_ch_freq = get_data_ch_freq(p.bs_data_ch_idx);   //Get the data channel frequency
       
     ble_config((p.bs_ch_freq-1.0F), 2);   //Configure ADPLL
    
     ble_recv_on();   //Configure BLE for data reception
    
     wp_set_ch_index(p.bs_data_ch_idx);   //Configure the dewhitener channel index  

     //Delay - temporary settings
     start_time = timer_time_us();
     while((timer_time_us() - start_time) < (uint32_t)10000);
     
     p.nbytes = ble_receive((uint8_t *)&p.bs_rx_lldata_tmp_pdu, p.pdu_size);   //Read the PDU from the HW/RX fifo
    
     ble_off();   //Turn off BLE

     if (p.nbytes == (p.pdu_size + CRC_LEN)) {   //There is data in the RX buffer (pdu + crc)  
        p.error=1;
        p.nextState=BS_TX_END_CONNECTION;				 
     } else {
        p.error=2;
        p.nextState=0; 
     }	    
     
#ifdef DBUG
     p.tt=timer_time_us() - start_time_debug;    //for debugging purpose
#endif
#ifdef TMPO     
     uart_printf("BS received TMP: %d\n\n", p.bs_rx_lldata_tmp_pdu.payload);   //used temporarily to check tmp value when DBUG is not enabled   		
#endif	
     return p;
} 
	
