#include "iob-uart.h"
#include "bs_def.h"
#include "bs_fct.h"

extern bs_s_t bs;

bs_standby_param_s_t p_bstd={0};
bs_tx_cnt_req_param_s_t p_btq={0}; 
bs_rx_adv_param_s_t p_brv={0};
bs_rx_tmp_param_s_t p_brtmp={0}; 
bs_tx_data_ack_param_s_t p_btack={0};
bs_end_cnt_param_s_t p_bs_end_cnt={0};
   
void base_station(){
     uint32_t jb=0;

     while(jb<ITER) {  //will be replaced by while(1) to run with non-stop
      switch(bs.nextState) {
      	case BS_STANDBY:
	     p_bstd=bs_standby();
	     p_brv.bs_adv_ch_idx=p_bstd.adv_ch_start_idx;
#ifdef DBUG  	     
	     bs_standby_print(p_bstd); 
#endif	
	     bs.nextState=p_bstd.nextState;
	     break; 
     
        case BS_TX_CONNECT_REQ:		             	  		       
	     p_btq=bs_tx_cnt_req(p_brv.bs_adv_ch_idx, p_brv.bs_rx_adv_pdu.payload.AdvA);	      
#ifdef DBUG
	     bs_tx_cnt_req_print(p_btq);
#endif	      
	     bs.nextState=p_btq.nextState; 
	     break;
	      
  	case BS_RX_ADV_DIRECT_IND:  		      
	     p_brv=bs_rx_adv(p_brv.bs_adv_ch_idx);
#ifdef DBUG	      
	     bs_rx_adv_print(p_brv);
#endif	      	            
	     bs.nextState=p_brv.nextState; 
	     break;  			      

  	case BS_RX_DATA_TMP:  	
             p_brtmp=bs_rx_data_tmp(p_btq.bs_tx_connect_request_pdu.payload.LLData_AA, p_btq.bs_tx_connect_request_pdu.payload.LLData_ChM, p_btq.bs_tx_connect_request_pdu.payload.LLData_WinOffset);
#ifdef DBUG
	     bs_rx_data_tmp_print(p_brtmp);
#endif    
	     bs.nextState=p_brtmp.nextState;	            
	     break;	

	case BS_TX_DATA_ACK:	
	     p_btack=bs_tx_data_ack(p_brtmp.bs_data_ch_idx);
#ifdef DBUG
             bs_tx_data_ack_print(p_btack);
#endif	      
	     bs.nextState=p_btack.nextState;
	     break;		      
	      	      	      	                  	 	 	 	      
	case BS_END_CONNECTION:	    	   	   		   	      	      	      	      
	     p_bs_end_cnt=bs_end_cnt();		
#ifdef DBUG
	     bs_end_cnt_print(p_bs_end_cnt);
#endif
	     bs.nextState=p_bs_end_cnt.nextState; 	      	       	      
  	     break;		      
	
	default: bs.nextState=0; break; 	               
    } 
  jb++; 
  }		      
}
