#include "iob-uart.h"
#include "bs_def.h"

void bs_tx_cnt_req_print(bs_tx_cnt_req_param_s_t p_btq, bs_rx_adv_param_s_t p_brv){
    uart_printf("\nBS TX sent %d bytes/CONNECT_REQ packet on the advertising channel (index,frequency)=(%d,%dMHz)\n", p_btq.pdu_size, p_brv.bs_adv_ch_idx, p_brv.bs_ch_freq); 					  
    printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
	  p_btq.bs_tx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, p_btq.bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	  p_btq.bs_tx_connect_request_pdu.pdu_adv_ind_h.TxAdd, p_btq.bs_tx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
  	  p_btq.bs_tx_connect_request_pdu.pdu_adv_ind_h.Length, p_btq.bs_tx_connect_request_pdu.pdu_adv_ind_h.RFU_2);			  
    printf_("InitA=%#llx | AdvA=%#llx | AA=%#x | CRCInit=%#x\n", \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.InitA, p_btq.bs_tx_connect_request_pdu.connect_req_payload.AdvA, \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_AA, p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_CRCInit);		  
    printf_("WinSize=%d | WinOffset=%d | Interval=%d | Latency=%d | Timeout=%d\n", \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_WinSize, p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_Interval, p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_Latency, \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_Timeout);		  
    printf_("ChM=%#llx | Hop=%d | SCA=%d\n\n", \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_ChM, p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_Hop, \
  	  p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_SCA);								   
}

void bs_rx_adv_ind_print(bs_rx_adv_param_s_t p_brv){
    uart_printf("\nBS RX received %d bytes/ADV_DIRECT_IND expected on the advertising channel (index,frequency)=(%d,%dMHz)\n", p_brv.nbytes, p_brv.bs_adv_ch_idx, (p_brv.bs_ch_freq-1));      	      				
    printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\nAdvA=%#llx | InitA=%#llx\n", \
         p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
         p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
         p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_h.Length, p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
         p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_payload.InitA);					    
         
    if(p_brv.result==1){printf_("BS received an ADV_DIRECT_IND packet\n\n");}	   
    else if(p_brv.result==2){printf_("BS did not receive an ADV_IND packet\n\n");}
    else if(p_brv.result==3){printf_("BS received %d bytes instead of %d bytes!\n\n", p_brv.nbytes, p_brv.pdu_size);}
}    

void bs_rx_data_gps_print(bs_rx_gps_param_s_t p_brgps) {
    uart_printf("\nBS RX received %d bytes/DATA_GPS expected on the data channel (index,frequency)=(%d,%dMHz)\n", p_brgps.nbytes, p_brgps.bs_data_ch_idx, (p_brgps.bs_ch_freq - 1));			
    printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n", \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_h.LLID, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_h.NESN, \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_h.SN, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_h.MD, \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_h.RFU, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_h.Length, \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_D, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_M, \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_S, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_C, \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_D, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_M, \
	 p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_S, p_brgps.bs_rx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_C); 
    if(p_brgps.result==1){printf("BS received a DATA_GPS packet\n\n");}				
    else if(p_brgps.result==2){printf_("BS received %d bytes instead of %d bytes!\n\n", p_brgps.nbytes, p_brgps.pdu_size_1);}
}

void bs_tx_llcontrol_print(bs_tx_llcontrol_param_s_t p_btack_gps, bs_rx_gps_param_s_t p_brgps, uint32_t gps_tmp){
    if(gps_tmp==1){
    	uart_printf("\nBS TX sent %d bytes/ACK_GPS packet on the data channel (index,frequency)=(%d,%dMHz)\n", p_btack_gps.pdu_size_1, p_brgps.bs_data_ch_idx, p_brgps.bs_ch_freq);	
    } else if(gps_tmp==2){  
        	uart_printf("\nBS TX sent %d bytes/END_CONNECTION packet on the data channel (index,frequency)=(%d,%dMHz)\n", p_btack_gps.pdu_size_1, p_brgps.bs_data_ch_idx, p_brgps.bs_ch_freq);
    }    		
    printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nOpcode=%d | CtrData=%d\n\n",
	p_btack_gps.bs_tx_pdu_llcontrol.pdu_lldata_h.LLID, p_btack_gps.bs_tx_pdu_llcontrol.pdu_lldata_h.NESN, \
	p_btack_gps.bs_tx_pdu_llcontrol.pdu_lldata_h.SN, p_btack_gps.bs_tx_pdu_llcontrol.pdu_lldata_h.MD, \
	p_btack_gps.bs_tx_pdu_llcontrol.pdu_lldata_h.RFU, p_btack_gps.bs_tx_pdu_llcontrol.pdu_lldata_h.Length, \
	p_btack_gps.bs_tx_pdu_llcontrol.pdu_llcontrol_payload.Opcode, \
	p_btack_gps.bs_tx_pdu_llcontrol.pdu_llcontrol_payload.CtrData); 				
}

void bs_rx_data_tmp_print(bs_rx_tmp_param_s_t p_brtmp, bs_rx_gps_param_s_t p_brgps){
     uart_printf("\nBS RX received %d bytes/DATA_TMP expected on the data channel (index,frequency)=(%d,%dMHz)\n", p_brtmp.nbytes, p_brgps.bs_data_ch_idx, (p_brgps.bs_ch_freq - 1));  
				       
     printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nTemperature=%d\n", \
           p_brtmp.bs_rx_lldata_tmp_pdu.pdu_lldata_h.LLID, p_brtmp.bs_rx_lldata_tmp_pdu.pdu_lldata_h.NESN, \
           p_brtmp.bs_rx_lldata_tmp_pdu.pdu_lldata_h.SN, p_brtmp.bs_rx_lldata_tmp_pdu.pdu_lldata_h.MD, \
           p_brtmp.bs_rx_lldata_tmp_pdu.pdu_lldata_h.RFU, p_brtmp.bs_rx_lldata_tmp_pdu.pdu_lldata_h.Length, \
           p_brtmp.bs_rx_lldata_tmp_pdu.payload_tmp);
     
     if(p_brtmp.result==1){printf("BS received a DATA_TMP packet\n\n");}			       
     else if(p_brtmp.result==2){printf_("BS received %d bytes instead of %d bytes!\n\n", p_brtmp.nbytes, p_brtmp.pdu_size_1);}
}	      
