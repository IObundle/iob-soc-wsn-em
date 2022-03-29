#pragma once

#include "cm_def.h"

#define SN_ID 0

typedef enum SN_MODES {
	MODE_SN_STANDBY=1,	
	MODE_SN_TX_ADV_DIRECT_IND,	
	MODE_SN_RX_CONNECT_REQ,
	MODE_SN_TX_DATA_GPS,
	MODE_SN_RX_ACK_GPS, 
	MODE_SN_TX_DATA_TMP,
	MODE_SN_RX_END_CONNECTION
} sn_modes_e_t;

typedef struct SN {    	  
    	uint32_t  aa;     
    	uint32_t  nextState; 
    	uint8_t   id;   
    	uint8_t   adv;  
	char      buffer_tx[MAX_N_BYTES];
	char	  buffer_rx[MAX_N_BYTES];
} sn_s_t;

typedef struct {
     adv_direct_pdu_s_t sn_tx_adv_ind_pdu;                 //Connectable undirected advertising PDU
     uint32_t           nextState;
     uint32_t           sn_ch_freq;  
     uint32_t           pdu_size;	                   //for debugging purpose   
} sn_tx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t sn_rx_connect_request_pdu;    //Connection request PDU
     uint32_t                nextState;  
     uint16_t                sn_adv_ch_idx;
     uint8_t                 sn_adv;
     uint32_t                pdu_size;	                   //for debugging purpose
     uint32_t                result;			   //for debugging purpose     
     int8_t 	             nbytes;	                   //for debugging purpose    
} sn_rx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_gps_s_t sn_tx_data_gps_pdu;                //GPS Coordinates data PDU
     uint32_t           nextState;  
     uint32_t           sn_ch_freq;
     uint32_t           pdu_size_1;	                   //for debugging purpose
     uint16_t           sn_data_ch_idx;                    //for debugging purpose
     uint8_t            data_ch[MAX_N_DATA_CHANNELS];
} sn_tx_gps_param_s_t;
    
typedef struct {
     pdu_llcontrol_s_t  sn_rx_pdu_llcontrol;               //ACK or TERMINATE PDU     
     uint32_t           nextState;  
     uint32_t           pdu_size_1;	                   //for debugging purpose 
     uint32_t           result;	                           //for debugging purpose
     int8_t 	        nbytes;	                           //for debugging purpose            
} sn_rx_llcontrol_param_s_t;

typedef struct {
     pdu_lldata_tmp_s_t sn_tx_lldata_tmp_pdu;              //TMP data PDU
     uint32_t     nextState;  
     uint32_t     pdu_size_1;	                           //for debugging purpose 
} sn_tx_tmp_param_s_t;
