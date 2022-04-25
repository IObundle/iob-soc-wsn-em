#pragma once

#include "cm_def.h"

#define SN_ID 0
#define T_SN_STANDBY 1000

typedef struct SN {    	  
     uint32_t  aa;     
     uint32_t  nextState; 
     uint8_t   id;   
     uint8_t   isCh_aa;
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
     uint32_t    tt;					  //for debugging purpose
     uint16_t    adv_ch_start_idx;
} sn_standby_param_s_t;

typedef struct {
     adv_direct_pdu_s_t    sn_tx_adv_pdu;                 //Connectable directed advertising PDU
     uint32_t              nextState;
     uint32_t              sn_ch_freq;  		  //for debugging purpose
     uint32_t              pdu_size;                      //for debugging purpose  
     uint32_t              tt;                            //for debugging purpose
     uint16_t              sn_adv_ch_idx;                 
} sn_tx_adv_param_s_t;

typedef struct {
     connect_request_pdu_s_t sn_rx_connect_request_pdu;    //Connection request PDU
     uint32_t                nextState;  
     uint32_t                sn_ch_freq;  		  //for debugging purpose
     uint32_t                pdu_size;	                  //for debugging purpose
     uint32_t                error;			  //for debugging purpose   
     uint32_t                tt;			  //for debugging purpose       
     uint16_t                sn_adv_ch_idx;
     int8_t 	             nbytes;	                  //for debugging purpose    
} sn_rx_cnt_req_param_s_t;

typedef struct {
     pdu_lldata_gps_s_t sn_tx_data_gps_pdu;                //GPS Coordinates data PDU
     uint32_t           nextState;  
     uint32_t           sn_ch_freq;		           //for debugging purpose		
     uint32_t           pdu_size;	                   //for debugging purpose
     uint32_t           tt;			           //for debugging purpose 
     uint16_t           sn_data_ch_idx;                    //for debugging purpose
     uint8_t            data_ch[MAX_N_DATA_CHANNELS];
     uint8_t            ch_aa;
} sn_tx_gps_param_s_t;
    
typedef struct {
     pdu_llcontrol_s_t  sn_rx_pdu_llcontrol;               //ACK or TERMINATE PDU   
     uint32_t           sn_ch_freq;		           //for debugging purpose	  
     uint32_t           nextState;  
     uint32_t           pdu_size;	                   //for debugging purpose 
     uint32_t           error;	                           //for debugging purpose
     uint32_t           tt;                                //for debugging purpose
     uint16_t           sn_data_ch_idx;                    //for debugging purpose          
     int8_t 	        nbytes;	                           //for debugging purpose            
} sn_rx_llcontrol_param_s_t;

typedef struct {
     pdu_lldata_tmp_s_t sn_tx_lldata_tmp_pdu;              //TMP data PDU
     uint32_t     nextState;  
     uint32_t     sn_ch_freq;  		                   //for debugging purpose	  
     uint32_t     pdu_size;	                           //for debugging purpose 
     uint32_t     tt;                                      //for debugging purpose   
     uint16_t     sn_data_ch_idx;                          //for debugging purpose   
} sn_tx_tmp_param_s_t;
