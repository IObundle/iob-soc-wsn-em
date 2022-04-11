#include "iob-uart.h"
#include "bs_def.h"

void bs_standby_print(bs_standby_param_s_t p) {
      uart_printf("\nRuntime(bs_standby)=%dus\n\n",p.tt);
}

void bs_tx_cnt_req_print(bs_tx_cnt_req_param_s_t p){    
    uart_printf("Runtime(bs_tx_cnt_req)=%dus",p.tt);
    uart_printf("\nBS TX sent %d bytes/CONNECT_REQ packet on the advertising channel (index,frequency)=(%d,%dMHz)\n", p.pdu_size, p.bs_adv_ch_idx, p.bs_ch_freq); 					  
    printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
	  p.bs_tx_connect_request_pdu.h.PDU_Type, p.bs_tx_connect_request_pdu.h.RFU_1, \
	  p.bs_tx_connect_request_pdu.h.TxAdd, p.bs_tx_connect_request_pdu.h.RxAdd, \
  	  p.bs_tx_connect_request_pdu.h.Length, p.bs_tx_connect_request_pdu.h.RFU_2);		  
    printf_("InitA=%#llx | AdvA=%#llx | AA=%#x | CRCInit=%#x\n", \
  	  p.bs_tx_connect_request_pdu.payload.InitA, p.bs_tx_connect_request_pdu.payload.AdvA, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_AA, p.bs_tx_connect_request_pdu.payload.LLData_CRCInit);	  
    printf_("WinSize=%d | WinOffset=%d | Interval=%d | Latency=%d | Timeout=%d\n", \
  	  p.bs_tx_connect_request_pdu.payload.LLData_WinSize, p.bs_tx_connect_request_pdu.payload.LLData_WinOffset, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_Interval, p.bs_tx_connect_request_pdu.payload.LLData_Latency, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_Timeout);  	  
    printf_("ChM=%#llx | Hop=%d | SCA=%d\n\n", \
  	  p.bs_tx_connect_request_pdu.payload.LLData_ChM, p.bs_tx_connect_request_pdu.payload.LLData_Hop, \
  	  p.bs_tx_connect_request_pdu.payload.LLData_SCA);								   
}

void bs_rx_adv_print(bs_rx_adv_param_s_t p){
    uart_printf("Runtime(bs_rx_adv)=%dus",p.tt);
    uart_printf("\nBS RX received %d bytes/ADV_DIRECT_IND expected on the advertising channel (index,frequency)=(%d,%dMHz)\n", p.nbytes, p.bs_adv_ch_idx, (p.bs_ch_freq-1));      	      				
    printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\nAdvA=%#llx | InitA=%#llx\n", \
         p.bs_rx_adv_pdu.h.PDU_Type, p.bs_rx_adv_pdu.h.RFU_1, \
         p.bs_rx_adv_pdu.h.TxAdd, p.bs_rx_adv_pdu.h.RxAdd, \
         p.bs_rx_adv_pdu.h.Length, p.bs_rx_adv_pdu.h.RFU_2, \
         p.bs_rx_adv_pdu.payload.AdvA, p.bs_rx_adv_pdu.payload.InitA);				
         
    if(p.error==0){printf_("INFO: BS received an ADV_DIRECT_IND packet. (BS-I1)\n\n");}	
    else if(p.error==1){printf_("ERROR - PDU_CRC size: %d bytes received instead of %d bytes. (BS-E1)\n\n", p.nbytes, (p.pdu_size + CRC_LEN));}
    else if(p.error==2){printf_("ERROR - Payload size in PDU's H config: %d bytes instead of %d bytes. (BS-E2)\n\n", p.bs_rx_adv_pdu.h.Length, ADV_DIND_P_LEN);}
    else if(p.error==3){printf_("ERROR - Device address: Wrong initiator device address. (BS-E3)\n\n");}    
    else if(p.error==4){printf_("ERROR - Device address: Wrong advertiser device address. (BS-E4)\n\n");}
    else if(p.error==5){printf_("ERROR - Packet type: Does not match a connectable directed advertising packet type. (BS-E5)\n\n");} 
    else if(p.error==-1){printf_("ERROR - Initiator filter: Something went wrong, the initiator filter did not run. (BS-E6)\n\n");}
}    

void bs_rx_data_gps_print(bs_rx_gps_param_s_t p) {
    uart_printf("Runtime(bs_rx_data_gps)=%dus",p.tt);
    uart_printf("\nBS RX received %d bytes/DATA_GPS expected on the data channel (index,frequency)=(%d,%dMHz)\n", p.nbytes, p.bs_data_ch_idx, (p.bs_ch_freq - 1));			
    printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n", \
	 p.bs_rx_data_gps_pdu.h.LLID, p.bs_rx_data_gps_pdu.h.NESN, \
	 p.bs_rx_data_gps_pdu.h.SN, p.bs_rx_data_gps_pdu.h.MD, \
	 p.bs_rx_data_gps_pdu.h.RFU, p.bs_rx_data_gps_pdu.h.Length, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lat_D, p.bs_rx_data_gps_pdu.payload.DMS_Lat_M, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lat_S, p.bs_rx_data_gps_pdu.payload.DMS_Lat_C, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lng_D, p.bs_rx_data_gps_pdu.payload.DMS_Lng_M, \
	 p.bs_rx_data_gps_pdu.payload.DMS_Lng_S, p.bs_rx_data_gps_pdu.payload.DMS_Lng_C); 
    if(p.error==0){printf("INFO: BS received a DATA_GPS packet\n\n");}				
    else if(p.error==1){printf_("Error - PDU size: %d bytes received instead of %d bytes!\n\n", p.nbytes, p.pdu_size);}
}

void bs_tx_llcontrol_print(bs_tx_llcontrol_param_s_t p, uint32_t gps_tmp){
    uart_printf("Runtime(bs_tx_llcontrol)=%dus", p.tt);
    if(gps_tmp==1){
    	uart_printf("\nBS TX sent %d bytes/ACK_GPS packet on the data channel (index,frequency)=(%d,%dMHz)\n", p.pdu_size, p.bs_data_ch_idx, p.bs_ch_freq);	
    } else if(gps_tmp==2){  
        	uart_printf("\nBS TX sent %d bytes/END_CONNECTION packet on the data channel (index,frequency)=(%d,%dMHz)\n", p.pdu_size, p.bs_data_ch_idx, p.bs_ch_freq);
    }    		
    printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nOpcode=%d | CtrData=%d\n\n",
	p.bs_tx_pdu_llcontrol.h.LLID, p.bs_tx_pdu_llcontrol.h.NESN, \
	p.bs_tx_pdu_llcontrol.h.SN, p.bs_tx_pdu_llcontrol.h.MD, \
	p.bs_tx_pdu_llcontrol.h.RFU, p.bs_tx_pdu_llcontrol.h.Length, \
	p.bs_tx_pdu_llcontrol.payload.Opcode, \
	p.bs_tx_pdu_llcontrol.payload.CtrData); 				
}

void bs_rx_data_tmp_print(bs_rx_tmp_param_s_t p){
     uart_printf("Runtime(bs_rx_data_tmp)=%dus", p.tt);
     uart_printf("\nBS RX received %d bytes/DATA_TMP expected on the data channel (index,frequency)=(%d,%dMHz)\n", p.nbytes, p.bs_data_ch_idx, (p.bs_ch_freq - 1));  
				       
     printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nTemperature=%d\n", \
           p.bs_rx_lldata_tmp_pdu.h.LLID, p.bs_rx_lldata_tmp_pdu.h.NESN, \
           p.bs_rx_lldata_tmp_pdu.h.SN, p.bs_rx_lldata_tmp_pdu.h.MD, \
           p.bs_rx_lldata_tmp_pdu.h.RFU, p.bs_rx_lldata_tmp_pdu.h.Length, \
           p.bs_rx_lldata_tmp_pdu.payload);
     
     if(p.error==1){printf("BS received a DATA_TMP packet\n\n");}			       
     else if(p.error==2){printf_("BS received %d bytes instead of %d bytes!\n\n", p.nbytes, p.pdu_size);}
}	      
