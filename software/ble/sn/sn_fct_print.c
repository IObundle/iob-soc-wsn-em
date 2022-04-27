#include "iob-uart.h"
#include "sn_def.h"

void sn_standby_print(sn_standby_param_s_t p){
      //start_time, end_time
      uart_printf("\nSN - STANDBY timing: %dus, %dus\n\n", p.start, p.end);
}

void sn_tx_adv_print(sn_tx_adv_param_s_t p){
      //start_time, pkt_tx_time, start_tx, ble_off, end_time
      uart_printf("SN - TX ADV timing: %dus, %dus, %dus, %dus, %dus\n", p.start, T_PACKET(ADV_H_LEN,ADV_DIND_P_LEN), p.start_tx, p.boff, p.end);
      uart_printf("SN sent a PDU of %d bytes on the adv ch (idx,freq)=(%d,%dMHz)\n", p.pdu_size, p.sn_adv_ch_idx, p.sn_ch_freq);   
      printf_("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\nAdvA=%#llx | InitA=%#llx\n", \
	   p.sn_tx_adv_pdu.h.PDU_Type, p.sn_tx_adv_pdu.h.RFU_1, \
	   p.sn_tx_adv_pdu.h.TxAdd, p.sn_tx_adv_pdu.h.RxAdd, \
	   p.sn_tx_adv_pdu.h.Length, p.sn_tx_adv_pdu.h.RFU_2, \
	   p.sn_tx_adv_pdu.payload.AdvA, p.sn_tx_adv_pdu.payload.InitA);  
      uart_printf("INFO: SN sent an ADV_DIRECT_IND PDU. (SN-I1)\n\n");
}			

void sn_rx_cnt_req_print(sn_rx_cnt_req_param_s_t p){
       uint16_t adv_ch_idx=p.sn_adv_ch_idx; 
       if(p.error){
           if(adv_ch_idx == ADV_CH_FIRST){adv_ch_idx=ADV_CH_LAST;}
	   else if(adv_ch_idx > ADV_CH_FIRST) {adv_ch_idx--;}
       }        
       //start_time, rx_on, ble_off, end_time
       uart_printf("SN - RX CNT REQ timing: %dus, %dus, %dus, %dus\n", p.start, p.rx_on, p.boff, p.end); 
       uart_printf("SN received %d bytes on the adv ch (idx,freq)=(%d,%dMHz)\n", p.nbytes, adv_ch_idx, (p.sn_ch_freq - 1));      
       uart_printf("PDU_Type=%d | RFU_1=%d | TxAdd=%d | RxAdd=%d | Length=%d | RFU_2=%d\n", \
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
       if(p.error==0){uart_printf("INFO: SN received a CONNECT_REQ PDU. (SN-I2)\n\n");}	
       else if(p.error==1){uart_printf("ERROR - PDU_CRC size: %d bytes received instead of %d bytes. (SN-E1)\n\n", p.nbytes, (p.pdu_size + CRC_LEN));}
       else if(p.error==2){uart_printf("ERROR - Payload size in PDU's H config: %d bytes instead of %d bytes. (SN-E2)\n\n", p.sn_rx_connect_request_pdu.h.Length, CONNECT_REQ_P_LEN);}
       else if(p.error==3){uart_printf("ERROR - Device address: Wrong advertiser device address. (SN-E3)\n\n");}
       else if(p.error==4){uart_printf("ERROR - Device address: Wrong initiator device address. (SN-E4)\n\n");}
       else if(p.error==5){uart_printf("ERROR - Packet type: Does not match a connection request packet type. (SN-E5)\n\n");} 
       else if(p.error==-1){uart_printf("ERROR - Advertiser filter: Something went wrong, the advertiser filter did not run. (SN-E6)\n\n");}              
}       

void sn_tx_data_gps_print(sn_tx_gps_param_s_t p){
       //start_time, pkt_tx_time, start_tx, ble_off, end_time
       uart_printf("SN - TX DATA GPS timing: %dus, %dus, %dus, %dus, %dus\n", p.start, T_PACKET(LL_DATA_H_LEN,LL_DATA_GPS_P_LEN), p.start_tx, p.boff, p.end);
       uart_printf("SN sent a PDU of %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.pdu_size, p.sn_data_ch_idx, p.sn_ch_freq);		  
       uart_printf("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\nGPS coordinates\n\tLatitude : %ddeg %dmin %ds %c\n\tLongitude: %ddeg %dmin %ds %c\n", \
	   p.sn_tx_data_gps_pdu.h.LLID, p.sn_tx_data_gps_pdu.h.NESN, \
	   p.sn_tx_data_gps_pdu.h.SN, p.sn_tx_data_gps_pdu.h.MD, \
	   p.sn_tx_data_gps_pdu.h.RFU, p.sn_tx_data_gps_pdu.h.Length, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lat_D, p.sn_tx_data_gps_pdu.payload.DMS_Lat_M, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lat_S, p.sn_tx_data_gps_pdu.payload.DMS_Lat_C, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lng_D, p.sn_tx_data_gps_pdu.payload.DMS_Lng_M, \
	   p.sn_tx_data_gps_pdu.payload.DMS_Lng_S, p.sn_tx_data_gps_pdu.payload.DMS_Lng_C);
      uart_printf("INFO: SN sent a DATA_GPS PDU. (SN-I3)\n\n");		   	   				 
}

void sn_tx_data_tmp_print(sn_tx_tmp_param_s_t p){    
      //start_time, pkt_tx_time, start_tx, ble_off, end_time
      uart_printf("SN - TX DATA TMP timng: %dus, %dus, %dus, %dus, %dus\n", p.start, T_PACKET(LL_DATA_H_LEN,LL_DATA_TMP_P_LEN), p.start_tx, p.boff, p.end); 
      uart_printf("SN sent a PDU of %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.pdu_size, p.sn_data_ch_idx, p.sn_ch_freq);	              		
      uart_printf("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d | Temperature=%d\n", \
	    p.sn_tx_lldata_tmp_pdu.h.LLID, p.sn_tx_lldata_tmp_pdu.h.NESN, \
	    p.sn_tx_lldata_tmp_pdu.h.SN, p.sn_tx_lldata_tmp_pdu.h.MD, \
	    p.sn_tx_lldata_tmp_pdu.h.RFU, p.sn_tx_lldata_tmp_pdu.h.Length, \
	    p.sn_tx_lldata_tmp_pdu.payload);  
      uart_printf("INFO: SN sent a DATA_TMP PDU. (SN-I5)\n\n");	     
}

void sn_rx_data_ack_print(sn_rx_data_ack_param_s_t p, uint32_t gps_tmp){     
      if(gps_tmp==1) {
          //start_time, rx_on, ble_off, end_time
          uart_printf("SN - RX DATA_ACK timing: %dus, %dus, %dus, %dus\n", p.start, p.rx_on, p.boff, p.end);	  
	  uart_printf("SN received %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.nbytes, p.sn_data_ch_idx, (p.sn_ch_freq - 1));	    	     		      
      } else if(gps_tmp==2) {
          //start_time, rx_on, ble_off, end_time
          uart_printf("SN - RX END_CONNECTION timing: %dus, %dus, %dus, %dus\n", p.start, p.rx_on, p.boff, p.end);  
          uart_printf("SN received %d bytes on the data ch (idx,freq)=(%d,%dMHz)\n", p.nbytes, p.sn_data_ch_idx, (p.sn_ch_freq - 1));
      }		
      uart_printf("LLID=%d | NESN=%d | SN=%d | MD=%d | RFU=%d | Length=%d\n", \
            p.sn_rx_data_ack_pdu.LLID, p.sn_rx_data_ack_pdu.NESN, \
	    p.sn_rx_data_ack_pdu.SN, p.sn_rx_data_ack_pdu.MD, \
	    p.sn_rx_data_ack_pdu.RFU, p.sn_rx_data_ack_pdu.Length);
      
      if((p.error == 0) && (gps_tmp == 1)){uart_printf("INFO: SN received an ACK_GPS PDU. (SN-I4)\n\n");}			
      else if((p.error == 5) && (gps_tmp == 2)){uart_printf("INFO: SN received an END_CONNECTION PDU. (SN-I6)\n\n");}
      else if(p.error==1){uart_printf("ERROR - PDU_CRC size: %d bytes received instead of %d bytes\n\n", p.nbytes, (p.pdu_size + CRC_LEN));}
      else if(p.error==2){uart_printf("Error - Sequence number: The previous packet sent has not been acknowledged!\n\n");}
      else if(p.error==3){uart_printf("Error - Payload size in PDU's H config: It does not match the size of an ACK/NACK packet payload\n\n");}
      else if(p.error==4){uart_printf("Error - LLID: LLID value does not match that of an ACK/NACK PDU\n\n");}
}
