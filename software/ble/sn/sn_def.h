#pragma once

#include "cm_def.h"

#define SN_ID 0
#define N_STD 2

//Standby delay in us
#define T_SN_STANDBY(N) N*T_Slot

typedef struct SN {    	  
     uint32_t  aa;     
     uint32_t  nextState; 
     uint32_t  nSend;
     uint8_t   id;   
     uint8_t   transmitSeqNum;       //used to identify packets sent by the Link Layer
     uint8_t   nextExpectedSeqNum;   //used by the peer to acknowledge the last data channel pdu sent or to request resending the last data channel pdu sent     
} sn_s_t;

typedef enum SN_STATES {
     SN_STANDBY=1,	   
     SN_TX_ADV_DIRECT_IND,	   
     SN_RX_CONNECT_REQ,
     SN_TX_DATA_GPS,
     SN_RX_ACK_GPS, 
     SN_TX_DATA_TMP,
     SN_RX_END_CONNECTION
} sn_states_e_t;

typedef struct {
     uint32_t    nextState;
     uint32_t    start;				     	  //for debugging purpose     
     uint32_t    end;				          //for debugging purpose
     uint32_t    boff;				          //for debugging purpose
     uint16_t    adv_ch_start_idx;
} sn_standby_param_s_t;

typedef struct {
     adv_direct_pdu_s_t    sn_tx_adv_pdu;                 //Connectable directed advertising PDU
     uint32_t              nextState;
     uint32_t              sn_ch_freq;  		  //for debugging purpose
     uint32_t              pdu_size;                      //for debugging purpose  
     uint32_t    	   start;			  //for debugging purpose    
     uint32_t    	   start_tx;			  //for debugging purpose  
     uint32_t              boff;			  //for debugging purpose
     uint32_t    	   end;				  //for debugging purpose
     uint16_t              sn_adv_ch_idx;                 
} sn_tx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t sn_rx_connect_request_pdu;            //Connection request PDU
     uint32_t                nextState;  
     uint32_t                sn_ch_freq;  		  	   //for debugging purpose
     uint32_t                pdu_size;	                  	   //for debugging purpose
     uint32_t                error;			           //for debugging purpose   
     uint32_t    	     start;			           //for debugging purpose     
     uint32_t    	     rx_on;			  	   //for debugging purpose  
     uint32_t                boff;				   //for debugging purpose
     uint32_t                end;			           //for debugging purpose    
     uint16_t                sn_adv_ch_idx;
     int8_t 	             nbytes;	                           //for debugging purpose    
} sn_rx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_gps_s_t sn_tx_data_gps_pdu;               //GPS Coordinates data PDU
     uint32_t           nextState;  
     uint32_t           sn_ch_freq;		          //for debugging purpose		
     uint32_t           pdu_size;	                  //for debugging purpose
     uint32_t    	start;				  //for debugging purpose     
     uint32_t    	start_tx;			  //for debugging purpose  
     uint32_t           boff;				  //for debugging purpose
     uint32_t    	end;				  //for debugging purpose
     uint16_t           sn_data_ch_idx;                   
     uint8_t            data_ch[MAX_N_DATA_CHANNELS];
} sn_tx_gps_param_s_t;
    
typedef struct {
     pdu_lldata_tmp_s_t sn_tx_lldata_tmp_pdu;             //TMP data PDU
     uint32_t     nextState;  
     uint32_t     sn_ch_freq;  		                  //for debugging purpose	  
     uint32_t     pdu_size;	                          //for debugging purpose 
     uint32_t     start;			          //for debugging purpose     
     uint32_t     start_tx;			          //for debugging purpose  
     uint32_t     boff;				          //for debugging purpose
     uint32_t     end;  			          //for debugging purpose
     uint16_t     sn_data_ch_idx;                            
} sn_tx_tmp_param_s_t;

typedef struct {
     pdu_lldata_h_s_t   sn_rx_data_ack_pdu;               //ACK PDU     
     uint32_t           nextState;  
     uint32_t           sn_ch_freq;		          //for debugging purpose	
     uint32_t           pdu_size;	                  //for debugging purpose 
     uint32_t           error;	                          //for debugging purpose
     uint32_t    	start;				  //for debugging purpose     
     uint32_t    	rx_on;			          //for debugging purpose  
     uint32_t           boff;				  //for debugging purpose
     uint32_t    	end;				  //for debugging purpose 
     uint16_t           sn_data_ch_idx;                              
     int8_t 	        nbytes;	                          //for debugging purpose            
} sn_rx_data_ack_param_s_t;

