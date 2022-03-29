#include "iob_timer.h"
#include "id.h"
#include "sn_def.h"
#include "bs_def.h"
#include "sn_fct.h"
#include "bs_fct.h"

extern sn_s_t sn;  
extern bs_s_t bs;

sn_tx_adv_param_s_t p_stv={0}; sn_rx_cnt_req_param_s_t p_srq={0};
sn_tx_gps_param_s_t p_stgps={0}; sn_rx_llcontrol_param_s_t p_srack_gps={0};
sn_tx_tmp_param_s_t p_sttmp={0}; sn_rx_llcontrol_param_s_t p_srack_tmp={0}; 

bs_tx_cnt_req_param_s_t p_btq={0}; bs_rx_adv_param_s_t p_brv={0};
bs_rx_gps_param_s_t p_brgps={0}; bs_tx_llcontrol_param_s_t p_btack_gps={0};
bs_rx_tmp_param_s_t p_brtmp={0}; bs_tx_llcontrol_param_s_t p_btack_tmp={0};

int16_t sn_data_tmp=-15;   //temporarily setting for debugging purpose
   
int main() { 
  p_srq.sn_adv_ch_idx=ADV_CH_FIRST; 
  p_brv.bs_adv_ch_idx=ADV_CH_FIRST;
  
  //Init ID
  id_init(ID_BASE);
  					
  if(get_id()==sn.id) {
   uint32_t js=0;
    while(js<ITER) { //will be replaced by while(1) to run with non-stop
      switch(sn.nextState) {
      	case MODE_SN_STANDBY:  	  
	     sn.nextState=sn_standby(sn.adv);
	     bs.nextState=0; break; 
	      		
	case MODE_SN_TX_ADV_DIRECT_IND:	      
	     p_stv=sn_tx_adv_ind(p_srq.sn_adv_ch_idx);
#ifdef DBUG      
	     sn_tx_adv_ind_print(p_stv, p_srq); 	 
#endif	            	      
	     sn.nextState=p_stv.nextState;	            	       	      
  	     bs.nextState=0; break;
	      
	case MODE_SN_RX_CONNECT_REQ:	      	
	      p_srq=sn_rx_cnt_req(p_srq.sn_adv_ch_idx, p_stv.sn_ch_freq);	
#ifdef DBUG 
  	      uint32_t sn_start_time = timer_time_us();
  	      while ((timer_time_us() - sn_start_time) < (unsigned int)1000);    
	         
	      sn_rx_cnt_req_print(p_srq, p_stv);
#endif  	      
	      sn.nextState=p_srq.nextState;
	      bs.nextState=0; break;				

	case MODE_SN_TX_DATA_GPS:
	     p_stgps=sn_tx_data_gps(p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_AA, p_srq.sn_rx_connect_request_pdu.connect_req_payload.LLData_ChM);
#ifdef DBUG
	     sn_tx_data_gps_print(p_stgps);
#endif
	     sn.nextState=p_stgps.nextState;	
	     bs.nextState=0; break; 

  	case MODE_SN_RX_ACK_GPS:
	     p_srack_gps=sn_rx_llcontrol(p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq, 1);  	    
#ifdef DBUG
             sn_start_time = timer_time_us();
             while ((timer_time_us() - sn_start_time) < (unsigned int)1000);	      
             
	     sn_rx_llcontrol_print(p_srack_gps, p_stgps);
#endif	     
	     sn.nextState=p_srack_gps.nextState;	
	     bs.nextState=0; break; 	
	     	     
	case MODE_SN_TX_DATA_TMP:
	     p_sttmp=sn_tx_data_tmp(p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq, sn_data_tmp);	
#ifdef DBUG
	     sn_tx_data_tmp_print(p_sttmp, p_stgps);
#endif	     
	     sn.nextState=p_sttmp.nextState;
	     bs.nextState=0; break;      

  	case MODE_SN_RX_END_CONNECTION:
	     p_srack_tmp=sn_rx_llcontrol(p_stgps.sn_data_ch_idx, p_stgps.sn_ch_freq, 2);	     
#ifdef DBUG
             sn_start_time = timer_time_us();
             while ((timer_time_us() - sn_start_time) < (unsigned int)1000);	      
             
	     sn_rx_llcontrol_print(p_srack_tmp, p_stgps);
#endif	 
	     sn.nextState=p_srack_tmp.nextState;
	     bs.nextState=0; break; 	
		     	     
	default: sn.nextState=0; bs.nextState=0; break;	         	     
      }  
      js++;
     }    
  } else if (get_id()==bs.id) {
    uint32_t jb=0;
    while(jb<ITER) {  //will be replaced by while(1) to run with non-stop
      switch(bs.nextState) {
      	case MODE_BS_STANDBY:
	     bs.nextState=bs_standby();
	     sn.nextState=0; break; 
     
        case MODE_BS_TX_CONNECT_REQ:		             	  		       
	      p_btq=bs_tx_cnt_req(p_brv.bs_adv_ch_idx, p_brv.bs_ch_freq, p_brv.bs_rx_adv_ind_pdu.pdu_adv_ind_payload.AdvA);	      
#ifdef DBUG
	      bs_tx_cnt_req_print(p_btq, p_brv);
#endif	      
	      bs.nextState=p_btq.nextState; 
	      sn.nextState=0; break;
	      
  	case MODE_BS_RX_ADV_DIRECT_IND:  		      
	      p_brv=bs_rx_adv_ind(p_brv.bs_adv_ch_idx);
#ifdef DBUG	      
              uint32_t bs_start_time = timer_time_us();
    	      while ((timer_time_us() - bs_start_time) < (unsigned int)1000);   
	      
	      bs_rx_adv_ind_print(p_brv);
#endif	      	            
	      bs.nextState=p_brv.nextState; 
	      sn.nextState=0; break;  			      

  	case MODE_BS_RX_DATA_GPS:  	
              p_brgps=bs_rx_data_gps(p_btq.bs_tx_connect_request_pdu.connect_req_payload.LLData_AA);
#ifdef DBUG
              bs_start_time = timer_time_us();
              while ((timer_time_us() - bs_start_time) < (unsigned int)1000); 
	      
	      bs_rx_data_gps_print(p_brgps);
#endif    
	      bs.nextState=p_brgps.nextState;	            
	      sn.nextState=0; break;	

	case MODE_BS_TX_ACK_GPS:	
	      p_btack_gps=bs_tx_llcontrol(p_brgps.bs_ch_freq, p_brgps.bs_data_ch_idx, 1);
#ifdef DBUG
              bs_tx_llcontrol_print(p_btack_gps, p_brgps, 1);
#endif	      
	      bs.nextState=p_btack_gps.nextState;
	      sn.nextState=0; break;		      
	      
  	case MODE_BS_RX_DATA_TMP:  	
	      p_brtmp=bs_rx_data_tmp(p_brgps.bs_data_ch_idx, p_brgps.bs_ch_freq);
#ifdef DBUG
              bs_start_time = timer_time_us();
              while ((timer_time_us() - bs_start_time) < (unsigned int)1000); 
	      	      	      
	      bs_rx_data_tmp_print(p_brtmp, p_brgps);	
#endif	            
              bs.nextState=p_brtmp.nextState;		
	      sn.nextState=0; break;
	      	      	                  	 	 	 	      
	case MODE_BS_TX_END_CONNECTION:	    	   	   		   	      	      	      	      
	      p_btack_tmp=bs_tx_llcontrol(p_brgps.bs_ch_freq, p_brgps.bs_data_ch_idx, 2); 	
#ifdef DBUG
	      bs_tx_llcontrol_print(p_btack_tmp, p_brgps, 2);
#endif
	      bs.nextState=p_btack_tmp.nextState; 	      	       	      
  	      sn.nextState=0; break;		      
	
	default: sn.nextState=0; bs.nextState=0; break; 	               
     } 
     jb++; 
    }		      
  }  
  return 0;
}
