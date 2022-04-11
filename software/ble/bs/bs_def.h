#pragma once

#include "cm_def.h"

#define BS_ID 1
#define N_SW 16
#define N_SI 1

typedef struct BS {
     uint32_t  aa;
     uint32_t  nextState;
     uint32_t  isBusy;
     uint8_t   id;	
     uint8_t   isCh_aa; 
} bs_s_t;

typedef enum BS_STATES {
     BS_STANDBY=11,     
     BS_RX_ADV_DIRECT_IND,
     BS_TX_CONNECT_REQ, 
     BS_RX_DATA_GPS,
     BS_TX_ACK_GPS,
     BS_RX_DATA_TMP,
     BS_TX_END_CONNECTION		   
} bs_states_e_t;

typedef struct {
     uint32_t    nextState;
     uint32_t    tt;					     //for debugging purpose
     uint16_t    adv_ch_start_idx;
} bs_standby_param_s_t;

typedef struct {
     adv_direct_pdu_s_t bs_rx_adv_pdu;                       //Connectable directed advertising PDU  
     uint32_t           nextState;
     uint32_t           bs_ch_freq;  		             //for debugging purpose		
     uint32_t           pdu_size;	                     //for debugging purpose  
     uint32_t           tt;				     //for debugging purpose     
     int32_t            error;	  		             //for debugging purpose 
     uint16_t           bs_adv_ch_idx;
     int8_t 	        nbytes;	   		             //for debugging purpose   
} bs_rx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t bs_tx_connect_request_pdu;      //Connection request PDU
     uint32_t     nextState;  
     uint32_t     bs_ch_freq;  		                     //for debugging purpose	     
     uint32_t     pdu_size;				     //for debugging purpose
     uint32_t     tt;					     //for debugging purpose     
     uint16_t     bs_adv_ch_idx;		             
} bs_tx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_gps_s_t bs_rx_data_gps_pdu;                  //GPS coordinates data PDU
     uint32_t     nextState;  
     uint32_t     bs_ch_freq;				     //for debugging purpose
     uint32_t     pdu_size;	                             //for debugging purpose
     uint32_t     tt;				             //for debugging purpose   
     uint32_t     error;	                             //for debugging purpose 
     uint16_t     bs_data_ch_idx;
     uint8_t      ch_aa;  
     int8_t 	  nbytes;	                             //for debugging purpose
} bs_rx_gps_param_s_t;

typedef struct {
     pdu_llcontrol_s_t  bs_tx_pdu_llcontrol;                 //ACK/TERMINATE PDU
     uint32_t     nextState;       
     uint32_t     bs_ch_freq;				     //for debugging purpose
     uint32_t     pdu_size;	                             //for debugging purpose  
     uint32_t     tt;				             //for debugging purpose                
     uint16_t     bs_data_ch_idx;
} bs_tx_llcontrol_param_s_t;

typedef struct {
     pdu_lldata_tmp_s_t bs_rx_lldata_tmp_pdu;                //TMP data PDU
     uint32_t     nextState;  
     uint32_t     bs_ch_freq;				     //for debugging purpose
     uint32_t     pdu_size;	                             //for debugging purpose 
     uint32_t     error;	                             //for debugging purpose 
     uint32_t     tt;				             //for debugging purpose             
     uint16_t     bs_data_ch_idx;
     int8_t       nbytes;				     //for debugging purpose
} bs_rx_tmp_param_s_t;

typedef struct {
     uint32_t da_index;
     int32_t  error; 
} init_filter_s_t;
