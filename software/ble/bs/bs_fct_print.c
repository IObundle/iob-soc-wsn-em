#include "iob-uart.h"
#include "bs_def.h"

void bs_standby_print(bs_standby_param_s_t p){
    //start_time, end_time
    uart_printf("\nBS - STANDBY timing: %dus, %dus\n\n", p.start, p.end);
}

void bs_tx_cnt_req_print(bs_tx_cnt_req_param_s_t p){ 
    //start_time, pkt_tx_time, start_tx, ble_off, end_time
    uart_printf("BS - TX CNT REQ timing: %dus, %dus, %dus, %dus, %dus\n", p.start, T_PACKET(ADV_H_LEN,CONNECT_REQ_P_LEN), p.start_tx, p.boff, p.end);
    uart_printf("BS sent a PDU of %d bytes on the adv ch (idx,freq)=(%d,%dMHz)\n", p.pdu_size, p.bs_adv_ch_idx, p.bs_ch_freq);				  
    uart_printf("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
	  p.bs_tx_connect_request_pdu.h.PDU_Type, p.bs_tx_connect_request_pdu.h.RFU_1, \
	  p.bs_tx_connect_request_pdu.h.TxAdd, p.bs_tx_connect_request_pdu.h.RxAdd, \
  	  p.bs_tx_connect_request_pdu.h.Length, p.bs_tx_connect_request_pdu.h.RFU_2);	  		  
#ifdef SIM_SYNTH
    uart_printf("InitA="); print_data(p.bs_tx_connect_request_pdu.payload.InitA, 6);	
    uart_printf(" | AdvA="); print_data(p.bs_tx_connect_request_pdu.payload.AdvA, 6);
    uart_printf(" | AA="); print_data(p.bs_tx_connect_request_pdu.payload.LLData_AA, 4);
    uart_printf(" | CRCInit="); print_data(p.bs_tx_connect_request_pdu.payload.LLData_CRCInit, 3);
    uart_printf("\n");
#else
    printf_("InitA=%#llx | AdvA=%#llx | AA=%#x | CRCInit=%#x\n", \
  	  p.bs_tx_connect_request_pdu.payload.InitA, p.bs_tx_connect_request_pdu.payload.AdvA, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_AA, p.bs_tx_connect_request_pdu.payload.LLData_CRCInit);	  
#endif
    uart_printf("WinSize=%d | WinOffset=%d | Interval=%d | Latency=%d | Timeout=%d\n", \
  	  p.bs_tx_connect_request_pdu.payload.LLData_WinSize, p.bs_tx_connect_request_pdu.payload.LLData_WinOffset, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_Interval, p.bs_tx_connect_request_pdu.payload.LLData_Latency, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_Timeout);  	  
#ifdef SIM_SYNTH
    uart_printf("ChM="); print_data(p.bs_tx_connect_request_pdu.payload.LLData_ChM, 5);	
#else
    printf_("ChM=%#llx", p.bs_tx_connect_request_pdu.payload.LLData_ChM);
#endif
    uart_printf(" | Hop=%d | SCA=%d\n", p.bs_tx_connect_request_pdu.payload.LLData_Hop, p.bs_tx_connect_request_pdu.payload.LLData_SCA);
    uart_printf("INFO: BS sent a CONNECT_REQ PDU. (BS-I2)\n\n");								   
}

void bs_rx_adv_print(bs_rx_adv_param_s_t p){
    uint16_t adv_ch_idx=p.bs_adv_ch_idx; 
    if(p.error){
        if(adv_ch_idx == ADV_CH_FIRST){adv_ch_idx=ADV_CH_LAST;}
	else if(adv_ch_idx > ADV_CH_FIRST) {adv_ch_idx--;}
    } 
    //start_time, rx_on, end_scan, ble_off, end_time
    uart_printf("BS - RX ADV timing: %dus, %dus, %dus, %dus, %dus\n", p.start, p.rx_on, p.end_scan, p.boff, p.end);  
    uart_printf("BS received %d bytes on the adv ch (idx,freq)=(%d,%dMHz)\n", p.nbytes, adv_ch_idx, (p.bs_ch_freq-1));            	      				
    uart_printf("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
         p.bs_rx_adv_pdu.h.PDU_Type, p.bs_rx_adv_pdu.h.RFU_1, \
         p.bs_rx_adv_pdu.h.TxAdd, p.bs_rx_adv_pdu.h.RxAdd, \
         p.bs_rx_adv_pdu.h.Length, p.bs_rx_adv_pdu.h.RFU_2);				
#ifdef SIM_SYNTH
    uart_printf("AdvA="); print_data(p.bs_rx_adv_pdu.payload.AdvA, 6);
    uart_printf(" | InitA="); print_data(p.bs_rx_adv_pdu.payload.InitA, 6);
    uart_printf("\n");
#else 
    printf_("AdvA=%#llx | InitA=%#llx\n", p.bs_rx_adv_pdu.payload.AdvA, p.bs_rx_adv_pdu.payload.InitA);
#endif         
    if(p.error==0){uart_printf("INFO: BS received an ADV_DIRECT_IND PDU. (BS-I1)\n\n");} 
    else if(p.error==1){uart_printf("ERROR - PDU_CRC size: %d bytes received instead of %d bytes. (BS-E1)\n\n", p.nbytes, (p.pdu_size + CRC_LEN));}
    else if(p.error==2){uart_printf("ERROR - Payload size in PDU's H config: %d bytes instead of %d bytes. (BS-E2)\n\n", p.bs_rx_adv_pdu.h.Length, ADV_DIND_P_LEN);}
    else if(p.error==3){uart_printf("ERROR - Device address: Wrong initiator device address. (BS-E3)\n\n");}    
    else if(p.error==4){uart_printf("ERROR - Device address: Wrong advertiser device address. (BS-E4)\n\n");}
    else if(p.error==5){uart_printf("ERROR - Packet type: Does not match a connectable directed advertising packet type. (BS-E5)\n\n");} 
    else if(p.error==-1){uart_printf("ERROR - Initiator filter: Something went wrong, the initiator filter did not run. (BS-E6)\n\n");}
}    

void bs_rx_data_gps_print(bs_rx_gps_param_s_t p) {
    //start_time, rx_on, ble_off, end_time
    uart_printf("BS - RX DATA GPS timing: %dus, %dus, %dus, %dus\n", p.start, p.rx_on, p.boff, p.end); 
    uart_printf("BS received %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.nbytes, p.bs_data_ch_idx, (p.bs_ch_freq - 1));		
    uart_printf("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n", \
	 p.bs_rx_data_gps_pdu.h.LLID, p.bs_rx_data_gps_pdu.h.NESN, \
	 p.bs_rx_data_gps_pdu.h.SN, p.bs_rx_data_gps_pdu.h.MD, \
	 p.bs_rx_data_gps_pdu.h.RFU, p.bs_rx_data_gps_pdu.h.Length, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lat_D, p.bs_rx_data_gps_pdu.payload.DMS_Lat_M, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lat_S, p.bs_rx_data_gps_pdu.payload.DMS_Lat_C, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lng_D, p.bs_rx_data_gps_pdu.payload.DMS_Lng_M, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lng_S, p.bs_rx_data_gps_pdu.payload.DMS_Lng_C); 
    if(p.error==0){uart_printf("INFO: BS received a DATA_GPS PDU. (BS-I3)\n\n");}					
    else if(p.error==1){uart_printf("Error - PDU size: %d bytes received instead of %d bytes!\n\n", p.nbytes, p.pdu_size);}
    else if(p.error==2){uart_printf("Error - Sequence number: An old packet has been resent!\n\n");}
    else if(p.error==3){uart_printf("Error - MD: Something went wrong - The SN is not listening to new packets!\n\n");}
}

void bs_rx_data_tmp_print(bs_rx_tmp_param_s_t p){
     //start_time, rx_on, ble_off, end_time
     uart_printf("BS - RX DATA TMP timing: %dus, %dus, %dus, %dus\n", p.start, p.rx_on, p.boff, p.end); 
     uart_printf("BS received %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.nbytes, p.bs_data_ch_idx, (p.bs_ch_freq - 1));					       
     uart_printf("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d | Temperature=%d\n", \
           p.bs_rx_lldata_tmp_pdu.h.LLID, p.bs_rx_lldata_tmp_pdu.h.NESN, \
           p.bs_rx_lldata_tmp_pdu.h.SN, p.bs_rx_lldata_tmp_pdu.h.MD, \
           p.bs_rx_lldata_tmp_pdu.h.RFU, p.bs_rx_lldata_tmp_pdu.h.Length, \
           p.bs_rx_lldata_tmp_pdu.payload);
     
     if(p.error==0){uart_printf("INFO: BS received a DATA_TMP PDU. (BS-I5)\n\n");}			       
     else if(p.error==1){uart_printf("Error - PDU size: %d bytes received instead of %d bytes!\n\n", p.nbytes, p.pdu_size);}
     else if(p.error==2){uart_printf("Error - Sequence number: The expected sequence number is different from the received one!\n\n");}
     else if(p.error==3){uart_printf("Error - MD: The SN is not listening to new packets!\n\n");}
}	      

void bs_tx_data_ack_print(bs_tx_data_ack_param_s_t p, uint32_t gps_tmp){
    if(gps_tmp == 1) {
       //start_time, pkt_tx_time, start_tx, ble_off, end_time
       uart_printf("BS - TX DATA_ACK timing: %dus, %dus, %dus, %dus, %dus\n", p.start, T_PACKET(LL_DATA_H_LEN,ZERO_PAYLOAD), p.start_tx, p.boff, p.end); 
       uart_printf("BS sent a PDU of %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.pdu_size, p.bs_data_ch_idx, p.bs_ch_freq);
    } else if (gps_tmp == 2) {
       //start_time, pkt_tx_time, start_tx ble_off, end_time
       uart_printf("BS - TX END_CONNECTION timing: %dus, %dus, %dus, %dus, %dus\n", p.start, T_PACKET(LL_DATA_H_LEN,ZERO_PAYLOAD), p.start_tx, p.boff, p.end); 
       uart_printf("BS sent a PDU of %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.pdu_size, p.bs_data_ch_idx, p.bs_ch_freq); 
    }		
    uart_printf("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\n",
	p.bs_tx_data_ack_pdu.LLID, p.bs_tx_data_ack_pdu.NESN, \
	p.bs_tx_data_ack_pdu.SN, p.bs_tx_data_ack_pdu.MD, \
	p.bs_tx_data_ack_pdu.RFU, p.bs_tx_data_ack_pdu.Length);	
    if(gps_tmp == 1) {uart_printf("INFO: BS sent an ACK_GPS PDU. (BS-I4)\n\n");} 
    else if(gps_tmp ==2) {uart_printf("INFO: BS sent an END_CONNECTION PDU. (BS-I6)\n\n");}				
}
