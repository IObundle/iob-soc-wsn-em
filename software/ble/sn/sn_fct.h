sn_standby_param_s_t sn_standby();
sn_tx_adv_param_s_t sn_tx_adv(uint16_t sn_adv_ch_idx);
sn_rx_cnt_req_param_s_t sn_rx_cnt_req(uint16_t sn_adv_ch_idx);
sn_tx_tmp_param_s_t sn_tx_data_tmp(uint32_t LLData_AA, uint64_t LLData_ChM, uint16_t LLData_WinOffset);
sn_rx_data_ack_param_s_t sn_rx_data_ack(uint16_t sn_data_ch_idx);
sn_end_cnt_param_s_t sn_end_cnt();
int32_t sn_advertiser_filter(sn_rx_cnt_req_param_s_t p);
int32_t sn_rx_ack_check(sn_rx_data_ack_param_s_t p);

#ifdef DBUG
void sn_standby_print(sn_standby_param_s_t p);
void sn_tx_adv_print(sn_tx_adv_param_s_t p);
void sn_rx_cnt_req_print(sn_rx_cnt_req_param_s_t p);
void sn_tx_data_tmp_print(sn_tx_tmp_param_s_t p);
void sn_rx_data_ack_print(sn_rx_data_ack_param_s_t p);
void sn_end_cnt_print();
#endif
