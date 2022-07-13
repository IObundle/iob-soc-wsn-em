#pragma once

#include "cm_def.h"

#define BS_ID   1
#define N_STD   1
#define N_SW   16
#define N_SI    1

//Standby delay in us
#define T_BS_STANDBY(N) N*T_Slot

typedef struct BS {
     uint32_t  aa;
     uint32_t  nextState;
     uint32_t  isBusy;
     uint32_t  nRec;
     uint8_t   toSBC;
     uint32_t  isCount;
     uint8_t   id;	
     uint8_t   transmitSeqNum;       //used to identify packets sent by the Link Layer
     uint8_t   nextExpectedSeqNum;   //used by the peer to acknowledge the last data channel pdu sent or to request resending the last data channel pdu sent
} bs_s_t;

typedef enum BS_STATES {
     BS_STANDBY=11,     
     BS_RX_ADV_DIRECT_IND,
     BS_TX_CONNECT_REQ, 
     BS_RX_DATA_TMP,
     BS_TX_DATA_ACK,
     BS_END_CONNECTION,
     BS_TX_TOSBC	   
} bs_states_e_t;

typedef struct {
     uint32_t    nextState;
     uint32_t    start;					     //for debugging purpose     
     uint32_t    end;					     //for debugging purpose
     uint16_t    adv_ch_start_idx;
} bs_standby_param_s_t;

typedef struct {
     adv_direct_pdu_s_t bs_rx_adv_pdu;                       //Connectable directed advertising PDU  
     uint32_t           nextState;
     uint32_t           bs_ch_freq;  		             //for debugging purpose		
     uint32_t           pdu_size;	                     //for debugging purpose  
     uint32_t    	start;				     //for debugging purpose     
     uint32_t    	rx_on;			             //for debugging purpose  
     uint32_t    	end_scan;			     //for debugging purpose
     uint32_t    	boff;				     //for debugging purpose     
     uint32_t    	end;				     //for debugging purpose
     int32_t            error;	  		             //for debugging purpose 
     uint16_t           bs_adv_ch_idx;
     int8_t 	        nbytes;	   		             //for debugging purpose   
} bs_rx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t bs_tx_connect_request_pdu;               //Connection request PDU
     uint32_t     nextState;  
     uint32_t     bs_ch_freq;  		                     	      //for debugging purpose	     
     uint32_t     pdu_size;				              //for debugging purpose
     uint32_t     start;				              //for debugging purpose     
     uint32_t     start_tx;			  		      //for debugging purpose  
     uint32_t     end;				                      //for debugging purpose 
     uint32_t     boff;				                      //for debugging purpose
     uint16_t     bs_adv_ch_idx;		             
} bs_tx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_tmp_s_t bs_rx_lldata_tmp_pdu;                //TMP data PDU
     uint32_t     nextState;  
     uint32_t     bs_ch_freq;				     //for debugging purpose
     uint32_t     pdu_size;	                             //for debugging purpose 
     uint32_t     error;	                             //for debugging purpose 
     uint32_t     start;				     //for debugging purpose     
     uint32_t     rx_on;			             //for debugging purpose           
     uint32_t     boff;				             //for debugging purpose
     uint32_t     end;				             //for debugging purpose 
     uint16_t     bs_data_ch_idx;
     uint8_t      data_ch[MAX_N_DATA_CHANNELS];
     int8_t       nbytes;				     //for debugging purpose
} bs_rx_tmp_param_s_t;

typedef struct {
     pdu_lldata_h_s_t   bs_tx_data_ack_pdu;                  //ACK PDU     
     uint32_t           nextState;  
     uint32_t           bs_ch_freq;		             //for debugging purpose	
     uint32_t           pdu_size;	                     //for debugging purpose 
     uint32_t           error;	                             //for debugging purpose
     uint32_t    	start;				     //for debugging purpose     
     uint32_t    	start_tx;			     //for debugging purpose  
     uint32_t           boff;				     //for debugging purpose
     uint32_t    	end;				     //for debugging purpose
     uint32_t    	rec;				     //for debugging purpose
     uint16_t           bs_data_ch_idx;                              
     int8_t 	        nbytes;	                             //for debugging purpose            
} bs_tx_data_ack_param_s_t;

typedef struct {
     uint32_t da_index;
     int32_t  error; 
} init_filter_s_t;

typedef struct {
     uint32_t           nextState;  
     uint32_t    	start;				  //for debugging purpose     
     uint32_t    	end;				  //for debugging purpose 
} bs_end_cnt_param_s_t;

typedef struct {
     uint64_t           sn_device_addr;
     uint32_t           temperature;
     uint32_t           nextState;
     uint32_t           start;                            //for debugging purpose
     uint32_t           end;                              //for debugging purpose
} bs_tx_tosbc_param_s_t;
