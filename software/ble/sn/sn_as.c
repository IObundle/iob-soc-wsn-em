#include "iob-uart.h"
#include "sn_def.h"
#include "sn_fct.h"

extern sn_s_t sn;  

sn_standby_param_s_t p_sstd={0}; 
sn_tx_adv_param_s_t p_stv={0};   
sn_rx_cnt_req_param_s_t p_srq={0};
sn_tx_tmp_param_s_t p_sttmp={0}; 
sn_rx_data_ack_param_s_t p_srack={0};
sn_end_cnt_param_s_t p_sn_end_cnt={0};
    
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

	case SN_TX_DATA_TMP:
	     p_sttmp=sn_tx_data_tmp(p_srq.sn_rx_connect_request_pdu.payload.LLData_AA, p_srq.sn_rx_connect_request_pdu.payload.LLData_ChM, p_srq.sn_rx_connect_request_pdu.payload.LLData_WinOffset);
#ifdef DBUG
	     sn_tx_data_tmp_print(p_sttmp);
#endif
	     sn.nextState=p_sttmp.nextState;	
	     break; 

  	case SN_RX_DATA_ACK:
	     p_srack=sn_rx_data_ack(p_sttmp.sn_data_ch_idx);	    
#ifdef DBUG                 
	     sn_rx_data_ack_print(p_srack);
#endif	     
	     sn.nextState=p_srack.nextState;	
	     break; 	

  	case SN_END_CONNECTION:
	     p_sn_end_cnt=sn_end_cnt();	     
#ifdef DBUG                  
	     sn_end_cnt_print(p_sn_end_cnt);
#endif	 
	     sn.nextState=p_sn_end_cnt.nextState;
	     break; 	
		     	     
	default: sn.nextState=0; break;	         	     
      }  
   js++;
   }    
}   
