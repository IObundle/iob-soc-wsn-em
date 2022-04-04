bs_standby_param_s_t bs_standby();
bs_tx_cnt_req_param_s_t bs_tx_cnt_req(uint16_t bs_adv_ch_idx, uint64_t AdvA);
bs_rx_adv_param_s_t bs_rx_adv(uint16_t bs_adv_ch_idx);
bs_rx_gps_param_s_t bs_rx_data_gps(uint32_t LLData_AA);
bs_tx_llcontrol_param_s_t bs_tx_llcontrol(uint32_t bs_ch_freq, uint16_t bs_data_ch_idx, uint32_t gps_tmp);
bs_rx_tmp_param_s_t bs_rx_data_tmp(uint16_t bs_data_ch_idx, uint32_t bs_ch_freq);

#ifdef DBUG
void bs_tx_cnt_req_print(bs_tx_cnt_req_param_s_t p_btq);
void bs_rx_adv_print(bs_rx_adv_param_s_t p_brv);
void bs_rx_data_gps_print(bs_rx_gps_param_s_t p_brgps);
void bs_tx_llcontrol_print(bs_tx_llcontrol_param_s_t p_btack_gps, bs_rx_gps_param_s_t p_brgps, uint32_t gps_tmp);
void bs_rx_data_tmp_print(bs_rx_tmp_param_s_t p_brtmp, bs_rx_gps_param_s_t p_brgps);
#endif
