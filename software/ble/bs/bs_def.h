#pragma once

#include "cm_def.h"

#define BS_ID 1
#define N_SW 16
#define N_SI 1

typedef enum BS_MODES {
	MODE_BS_STANDBY=11,	
	MODE_BS_RX_ADV_DIRECT_IND,
	MODE_BS_TX_CONNECT_REQ,	
	MODE_BS_RX_DATA_GPS,
	MODE_BS_TX_ACK_GPS,
	MODE_BS_RX_DATA_TMP,
	MODE_BS_TX_END_CONNECTION		
} bs_modes_e_t;

typedef enum AA_STATUS {
	AA_FREE=1,
	AA_ATTR
} aa_status_e_t;

typedef enum DA_STATUS {
	DA_FREE=1,
	GPS_WAIT,
	GPS_ATTR
} da_status_e_t;


typedef struct BS {
    	uint32_t aa;
    	uint32_t nextState;
    	uint32_t isBusy;
    	uint8_t  id;
	char     buffer_tx[MAX_N_BYTES];
	char	 buffer_rx[MAX_N_BYTES];	
} bs_s_t;

typedef struct {
     uint32_t    nextState;
     uint16_t    adv_ch_start_idx;
} bs_standby_param_s_t;

typedef struct {
     adv_direct_pdu_s_t bs_rx_adv_pdu;                       //Connectable directed advertising PDU  
     uint32_t           nextState;
     uint16_t           bs_adv_ch_idx;
     uint32_t           bs_ch_freq;  		             //for debugging purpose		
     uint32_t           pdu_size;	                     //for debugging purpose  
     int32_t            error;	  		             //for debugging purpose 
     int8_t 	        nbytes;	   		             //for debugging purpose   
} bs_rx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t bs_tx_connect_request_pdu;      //Connection request PDU
     uint32_t     nextState;  
     uint16_t     bs_adv_ch_idx;		             //for debugging purpose
     uint32_t     bs_ch_freq;  		                     //for debugging purpose	     
     uint32_t     pdu_size;				     //for debugging purpose
} bs_tx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_gps_s_t bs_rx_data_gps_pdu;                  //GPS Coordinates data PDU
     uint32_t     nextState;  
     uint16_t     bs_data_ch_idx;
     uint32_t     bs_ch_freq;
     uint32_t     pdu_size_1;	                             //for debugging purpose
     uint32_t     result;	                             //for debugging purpose 
     int8_t 	  nbytes;	                             //for debugging purpose  
} bs_rx_gps_param_s_t;

typedef struct {
     pdu_llcontrol_s_t  bs_tx_pdu_llcontrol;                 //ACK/TERMINATE PDU
     uint32_t     nextState;       
     uint32_t     pdu_size_1;	                             //for debugging purpose          
} bs_tx_llcontrol_param_s_t;


typedef struct {
     pdu_lldata_tmp_s_t bs_rx_lldata_tmp_pdu;                //TMP data PDU
     uint32_t     nextState;  
     uint32_t     pdu_size_1;	                             //for debugging purpose 
     uint32_t     result;	                             //for debugging purpose      
     int8_t       nbytes;
} bs_rx_tmp_param_s_t;
