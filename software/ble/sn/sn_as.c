#include "sn_def.h"
#include "sn_fct.h"

extern sn_s_t sn;  

sn_standby_param_s_t p_sstd={0}; 
sn_tx_adv_param_s_t p_stv={0};   
sn_rx_cnt_req_param_s_t p_srq={0};
sn_tx_gps_param_s_t p_stgps={0}; 
sn_tx_tmp_param_s_t p_sttmp={0}; 
sn_rx_data_ack_param_s_t p_srack_gps={0};
sn_rx_data_ack_param_s_t p_sr_cnt_end={0};
   
void sensor_node(){
     uint32_t js=0;
     
     while(js<ITER){ //will be replaced by while(1) to run with non-stop
      switch(sn.nextState) {
      	case SN_STANDBY:  	  
	     p_sstd=sn_standby();
	     p_srq.sn_adv_ch_idx=p_sstd.adv_ch_start_idx;
#ifdef DBUG  	     
	     sn_standby_print(p_sstd); 
#endif	     
	     sn.nextState=p_sstd.nextState;	     	     
	     break; 
	      		
	case SN_TX_ADV_DIRECT_IND:	      
	     p_stv=sn_tx_adv(p_srq.sn_adv_ch_idx);
#ifdef DBUG      
	     sn_tx_adv_print(p_stv); 	 
#endif	            	      
	     sn.nextState=p_stv.nextState;	            	       	      
  	     break;
	      
	case SN_RX_CONNECT_REQ:	      	
	     p_srq=sn_rx_cnt_req(p_srq.sn_adv_ch_idx);	
#ifdef DBUG 	         
	     sn_rx_cnt_req_print(p_srq);
#endif  	      
	     sn.nextState=p_srq.nextState;
	     break;				

	case SN_TX_DATA_GPS:
	     p_stgps=sn_tx_data_gps(p_srq.sn_rx_connect_request_pdu.payload.LLData_AA, p_srq.sn_rx_connect_request_pdu.payload.LLData_ChM, p_srq.sn_rx_connect_request_pdu.payload.LLData_WinOffset);
#ifdef DBUG
	     sn_tx_data_gps_print(p_stgps);
#endif
	     sn.nextState=p_stgps.nextState;	
	     break; 

  	case SN_RX_ACK_GPS:
	     p_srack_gps=sn_rx_data_ack(p_stgps.sn_data_ch_idx, 1);	    
#ifdef DBUG                 
	     sn_rx_data_ack_print(p_srack_gps, 1);
#endif	     
	     sn.nextState=p_srack_gps.nextState;	
	     break; 	
	     	     
	case SN_TX_DATA_TMP:
	     p_sttmp=sn_tx_data_tmp(p_stgps.sn_data_ch_idx);	
#ifdef DBUG
	     sn_tx_data_tmp_print(p_sttmp);
#endif	     
	     sn.nextState=p_sttmp.nextState;
	     break;      

  	case SN_RX_END_CONNECTION:
	     p_sr_cnt_end=sn_rx_data_ack(p_stgps.sn_data_ch_idx, 2);	     
#ifdef DBUG                  
	     sn_rx_data_ack_print(p_sr_cnt_end, 2);
#endif	 
	     sn.nextState=p_sr_cnt_end.nextState;
	     break; 	
		     	     
	default: sn.nextState=0; break;	         	     
      }  
   js++;
   }    
}   
