uint32_t sn_standby(uint8_t adv);
sn_tx_adv_param_s_t sn_tx_adv_ind(uint16_t sn_adv_ch_idx);
sn_rx_cnt_req_param_s_t sn_rx_cnt_req(uint16_t sn_adv_ch_idx, uint32_t sn_ch_freq);
sn_tx_gps_param_s_t sn_tx_data_gps(uint32_t LLData_AA, uint64_t LLData_ChM);
sn_tx_tmp_param_s_t sn_tx_data_tmp(uint16_t sn_data_ch_idx, uint32_t sn_ch_freq, int16_t sn_data_tmp);
sn_rx_llcontrol_param_s_t sn_rx_llcontrol(uint16_t sn_data_ch_idx, uint32_t sn_ch_freq, uint8_t gps_tmp);


#ifdef DBUG
void sn_tx_adv_ind_print(sn_tx_adv_param_s_t p_stv, sn_rx_cnt_req_param_s_t p_srq);
void sn_rx_cnt_req_print(sn_rx_cnt_req_param_s_t p_srq, sn_tx_adv_param_s_t p_stv);
void sn_tx_data_gps_print(sn_tx_gps_param_s_t p_stgps);
void sn_tx_data_tmp_print(sn_tx_tmp_param_s_t p_sttmp, sn_tx_gps_param_s_t p_stgps);
void sn_rx_llcontrol_print(sn_rx_llcontrol_param_s_t p_srack, sn_tx_gps_param_s_t p_stgps);
#endif
