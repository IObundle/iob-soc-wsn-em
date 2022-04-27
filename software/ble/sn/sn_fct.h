sn_standby_param_s_t sn_standby();
sn_tx_adv_param_s_t sn_tx_adv(uint16_t sn_adv_ch_idx);
sn_rx_cnt_req_param_s_t sn_rx_cnt_req(uint16_t sn_adv_ch_idx);
sn_tx_gps_param_s_t sn_tx_data_gps(uint32_t LLData_AA, uint64_t LLData_ChM);
sn_tx_tmp_param_s_t sn_tx_data_tmp(uint16_t sn_data_ch_idx);
sn_rx_data_ack_param_s_t sn_rx_data_ack(uint16_t sn_data_ch_idx, uint32_t gps_tmp);
int32_t sn_advertiser_filter(sn_rx_cnt_req_param_s_t p);
int32_t sn_rx_ack_check(sn_rx_data_ack_param_s_t p);

#ifdef DBUG
void sn_standby_print(sn_standby_param_s_t p);
void sn_tx_adv_print(sn_tx_adv_param_s_t p);
void sn_rx_cnt_req_print(sn_rx_cnt_req_param_s_t p);
void sn_tx_data_gps_print(sn_tx_gps_param_s_t p);
void sn_tx_data_tmp_print(sn_tx_tmp_param_s_t p);
void sn_rx_data_ack_print(sn_rx_data_ack_param_s_t p, uint32_t gps_tmp);
#endif
