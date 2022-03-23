#include "iob-uart.h"
#include "def.h"

void sn_tx_adv_ind_print(sn_tx_adv_param_s_t p_stv, sn_rx_cnt_req_param_s_t p_srq) {
      uart_printf("\nSN TX sent %d bytes/ADV_IND packet on the advertising channel (index,frequency)=(%d,%dMHz)\n", p_stv.pdu_size_1, p_srq.sn_adv_ch_idx, p_stv.sn_ch_freq);	      printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\nAdvA=%#llx | AdvData=%#x\n\n", \
	   p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_h.PDU_Type, p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_1, \
	   p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_h.TxAdd, p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_h.RxAdd, \
	   p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_h.Length, p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_h.RFU_2, \
	   p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvA, p_stv.sn_tx_adv_ind_pdu.pdu_adv_ind_payload.AdvData);  
}			

void sn_rx_cnt_req_print(sn_rx_cnt_req_param_s_t p_srq, sn_tx_adv_param_s_t p_stv) {
       uart_printf("\nSN RX received %d bytes/CONNECT_REQ expected on the advertising channel (index,frequency)=(%d,%dMHz)\n", p_srq.nbytes, p_srq.sn_adv_ch_idx, (p_stv.sn_ch_freq - 1));		        
       printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
	    p_srq.sn_rx_connect_request_pdu.pdu_adv_ind_h.PDU_Type, p_srq.sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_1, \
	    p_srq.sn_rx_connect_request_pdu.pdu_adv_ind_h.TxAdd, p_srq.sn_rx_connect_request_pdu.pdu_adv_ind_h.RxAdd, \
	    p_srq.sn_rx_connect_request_pdu.pdu_adv_ind_h.Length, p_srq.sn_rx_connect_request_pdu.pdu_adv_ind_h.RFU_2);
       printf_("InitA=%#llx | AdvA=%#llx | AA=%#x | CRCInit=%#x\n", \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.InitA, p_srq.sn_rx_connect_request_pdu.connect_req_payload.AdvA, \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_AA, p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_CRCInit); 	    
       printf_("WinSize=%d | WinOffset=%d | Interval=%d | Latency=%d | Timeout=%d\n", \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_WinSize, p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_WinOffset, \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_Interval, p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_Latency, \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_Timeout);		    
       printf_("ChM=%#llx | Hop=%d | SCA=%d\n", \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_ChM, p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_Hop, \
	    p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_SCA);		    

       if(p_srq.result==1) {printf_("SN received a CONNECT_REQ packet\n\n");}
       else if(p_srq.result==2){printf_("SN did not receive a CONNECT_REQ packet\n\n");}
       else if(p_srq.result==3){printf_("SN received %d bytes instead of %d bytes!\n\n", p_srq.nbytes, p_srq.pdu_size_1);}
}       

void sn_tx_data_gps_print(sn_tx_gps_param_s_t p_stgps) {
       uart_printf("\nSN TX sent %d bytes/DATA_GPS packet on the data channel (index,frequency)=(%d,%dMHz)\n", p_stgps.pdu_size_1, p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq);		  
       printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n\n", \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_h.LLID, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_h.NESN, \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_h.SN, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_h.MD, \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_h.RFU, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_h.Length, \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_D, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_M, \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_S, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lat_C, \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_D, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_M, \
	   p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_S, p_stgps.sn_tx_data_gps_pdu.pdu_lldata_payload.DMS_Lng_C);				   
}

void sn_rx_llcontrol_print(sn_rx_llcontrol_param_s_t p_srack, sn_tx_gps_param_s_t p_stgps) {
      uart_printf("\nSN RX received %d bytes/LL_CONTROL expected on the data channel (index,frequency)=(%d,%dMHz)\n", p_srack.nbytes, p_stgps.sn_data_ch_idx, (p_stgps.sn_ch_freq - 1));	      	     				
      printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nOpcode=%d | CtrData=%d\n",
            p_srack.sn_rx_pdu_llcontrol.pdu_lldata_h.LLID, p_srack.sn_rx_pdu_llcontrol.pdu_lldata_h.NESN, \
	    p_srack.sn_rx_pdu_llcontrol.pdu_lldata_h.SN, p_srack.sn_rx_pdu_llcontrol.pdu_lldata_h.MD, \
	    p_srack.sn_rx_pdu_llcontrol.pdu_lldata_h.RFU, p_srack.sn_rx_pdu_llcontrol.pdu_lldata_h.Length, \
            p_srack.sn_rx_pdu_llcontrol.pdu_llcontrol_payload.Opcode, \
	    p_srack.sn_rx_pdu_llcontrol.pdu_llcontrol_payload.CtrData); 
      
      if(p_srack.result==1){printf_("SN received an ACK_GPS packet\n\n");}			
      else if(p_srack.result==2){printf_("SN received an END_CONNECTION packet - TMP successfully received!\n\n");}
      else if(p_srack.result==3){printf_("SN received %d bytes instead of %d bytes!\n\n", p_srack.nbytes, p_srack.pdu_size_1);}
}

void sn_tx_data_tmp_print(sn_tx_tmp_param_s_t p_sttmp, sn_tx_gps_param_s_t p_stgps) {
	    uart_printf("\nSN TX sent %d bytes/DATA_TMP on the data channel (index,frequency)=(%d,%dMHz)\n", p_sttmp.pdu_size_1, p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq);		
	    printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nTemperature=%d\n\n", \
	         p_sttmp.sn_tx_lldata_tmp_pdu.pdu_lldata_h.LLID, p_sttmp.sn_tx_lldata_tmp_pdu.pdu_lldata_h.NESN, \
	         p_sttmp.sn_tx_lldata_tmp_pdu.pdu_lldata_h.SN, p_sttmp.sn_tx_lldata_tmp_pdu.pdu_lldata_h.MD, \
	         p_sttmp.sn_tx_lldata_tmp_pdu.pdu_lldata_h.RFU, p_sttmp.sn_tx_lldata_tmp_pdu.pdu_lldata_h.Length, \
	         p_sttmp.sn_tx_lldata_tmp_pdu.payload_tmp);   
}
