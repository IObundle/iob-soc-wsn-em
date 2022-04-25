#include "iob-uart.h"
#include "sn_def.h"

void sn_standby_print(sn_standby_param_s_t p){
      uart_printf("\nRuntime(sn_standby)=%dus\n\n",p.tt);
}

void sn_tx_adv_print(sn_tx_adv_param_s_t p){
      uart_printf("Runtime(sn_tx_adv)=%dus",p.tt);
      uart_printf("\nSN TX sent %d bytes/ADV_DIRECT_IND packet on the advertising channel (index,frequency)=(%d,%dMHz)\n", p.pdu_size, p.sn_adv_ch_idx, p.sn_ch_freq);
      printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\nAdvA=%#llx | InitA=%#llx\n\n", \
	   p.sn_tx_adv_pdu.h.PDU_Type, p.sn_tx_adv_pdu.h.RFU_1, \
	   p.sn_tx_adv_pdu.h.TxAdd, p.sn_tx_adv_pdu.h.RxAdd, \
	   p.sn_tx_adv_pdu.h.Length, p.sn_tx_adv_pdu.h.RFU_2, \
	   p.sn_tx_adv_pdu.payload.AdvA, p.sn_tx_adv_pdu.payload.InitA);  
}			

void sn_rx_cnt_req_print(sn_rx_cnt_req_param_s_t p){
       uint16_t adv_ch_idx=p.sn_adv_ch_idx; if(p.error) {adv_ch_idx--;}
       uart_printf("Runtime(sn_rx_cnt_req)=%dus",p.tt);
       uart_printf("\nSN RX received %d bytes/CONNECT_REQ expected on the advertising channel (index,frequency)=(%d,%dMHz)\n", p.nbytes, adv_ch_idx, (p.sn_ch_freq - 1));		        
       printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
	    p.sn_rx_connect_request_pdu.h.PDU_Type, p.sn_rx_connect_request_pdu.h.RFU_1, \
	    p.sn_rx_connect_request_pdu.h.TxAdd, p.sn_rx_connect_request_pdu.h.RxAdd, \
	    p.sn_rx_connect_request_pdu.h.Length, p.sn_rx_connect_request_pdu.h.RFU_2);
       printf_("InitA=%#llx | AdvA=%#llx | AA=%#x | CRCInit=%#x\n", \
	    p.sn_rx_connect_request_pdu.payload.InitA, p.sn_rx_connect_request_pdu.payload.AdvA, \
	    p.sn_rx_connect_request_pdu.payload.LLData_AA, p.sn_rx_connect_request_pdu.payload.LLData_CRCInit);	   
       printf_("WinSize=%d | WinOffset=%d | Interval=%d | Latency=%d | Timeout=%d\n", \
	    p.sn_rx_connect_request_pdu.payload.LLData_WinSize, p.sn_rx_connect_request_pdu.payload.LLData_WinOffset, \
	    p.sn_rx_connect_request_pdu.payload.LLData_Interval, p.sn_rx_connect_request_pdu.payload.LLData_Latency, \
	    p.sn_rx_connect_request_pdu.payload.LLData_Timeout); 		   
       printf_("ChM=%#llx | Hop=%d | SCA=%d\n", \
	    p.sn_rx_connect_request_pdu.payload.LLData_ChM, p.sn_rx_connect_request_pdu.payload.LLData_Hop, \
	    p.sn_rx_connect_request_pdu.payload.LLData_SCA);		       

       if(p.error==0){printf_("INFO: SN received a CONNECT_REQ packet. (SN-I1)\n\n");}	
       else if(p.error==1){printf_("ERROR - PDU_CRC size: %d bytes received instead of %d bytes. (SN-E1)\n\n", p.nbytes, (p.pdu_size + CRC_LEN));}
       else if(p.error==2){printf_("ERROR - Payload size in PDU's H config: %d bytes instead of %d bytes. (SN-E2)\n\n", p.sn_rx_connect_request_pdu.h.Length, CONNECT_REQ_P_LEN);}
       else if(p.error==3){printf_("ERROR - Device address: Wrong advertiser device address. (SN-E3)\n\n");}
       else if(p.error==4){printf_("ERROR - Device address: Wrong initiator device address. (SN-E4)\n\n");}
       else if(p.error==5){printf_("ERROR - Packet type: Does not match a connection request packet type. (SN-E5)\n\n");} 
       else if(p.error==-1){printf_("ERROR - Advertiser filter: Something went wrong, the advertiser filter did not run. (SN-E6)\n\n");}              
}       

void sn_tx_data_gps_print(sn_tx_gps_param_s_t p){
       uart_printf("Runtime(sn_tx_data_gps)=%dus",p.tt);
       uart_printf("\nSN TX sent %d bytes/DATA_GPS packet on the data channel (index,frequency)=(%d,%dMHz)\n", p.pdu_size, p.sn_data_ch_idx, p.sn_ch_freq);		  
       printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n\n", \
	   p.sn_tx_data_gps_pdu.h.LLID, p.sn_tx_data_gps_pdu.h.NESN, \
	   p.sn_tx_data_gps_pdu.h.SN, p.sn_tx_data_gps_pdu.h.MD, \
	   p.sn_tx_data_gps_pdu.h.RFU, p.sn_tx_data_gps_pdu.h.Length, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lat_D, p.sn_tx_data_gps_pdu.payload.DMS_Lat_M, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lat_S, p.sn_tx_data_gps_pdu.payload.DMS_Lat_C, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lng_D, p.sn_tx_data_gps_pdu.payload.DMS_Lng_M, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lng_S, p.sn_tx_data_gps_pdu.payload.DMS_Lng_C);				 
}

void sn_rx_llcontrol_print(sn_rx_llcontrol_param_s_t p, uint8_t gps_tmp){
      uart_printf("Runtime(sn_rx_llcontrol)=%dus", p.tt);
      uart_printf("\nINFO: SN RX received %d bytes/LL_CONTROL expected on the data channel (index,frequency)=(%d,%dMHz)\n", p.nbytes, p.sn_data_ch_idx, (p.sn_ch_freq - 1));	      	     				
      printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nOpcode=%d | CtrData=%d\n",
            p.sn_rx_pdu_llcontrol.h.LLID, p.sn_rx_pdu_llcontrol.h.NESN, \
	    p.sn_rx_pdu_llcontrol.h.SN, p.sn_rx_pdu_llcontrol.h.MD, \
	    p.sn_rx_pdu_llcontrol.h.RFU, p.sn_rx_pdu_llcontrol.h.Length, \
            p.sn_rx_pdu_llcontrol.payload.Opcode, \
	    p.sn_rx_pdu_llcontrol.payload.CtrData); 
      
      if(gps_tmp==1 && p.error==0){printf_("INFO: SN received an ACK_GPS packet\n\n");}			
      else if(gps_tmp==2 && p.error==0){printf_("INFO: SN received an END_CONNECTION packet - TMP successfully received!\n\n");}
      else if(p.error==1){printf_("ERROR: SN received %d bytes instead of %d bytes!\n\n", p.nbytes, (p.pdu_size + CRC_LEN));}
}

void sn_tx_data_tmp_print(sn_tx_tmp_param_s_t p){
      uart_printf("Runtime(sn_tx_data_tmp)=%dus", p.tt);
      uart_printf("\nSN TX sent %d bytes/DATA_TMP on the data channel (index,frequency)=(%d,%dMHz)\n", p.pdu_size, p.sn_data_ch_idx, p.sn_ch_freq);		
      printf_("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nTemperature=%d\n\n", \
	    p.sn_tx_lldata_tmp_pdu.h.LLID, p.sn_tx_lldata_tmp_pdu.h.NESN, \
	    p.sn_tx_lldata_tmp_pdu.h.SN, p.sn_tx_lldata_tmp_pdu.h.MD, \
	    p.sn_tx_lldata_tmp_pdu.h.RFU, p.sn_tx_lldata_tmp_pdu.h.Length, \
	    p.sn_tx_lldata_tmp_pdu.payload);   
}
