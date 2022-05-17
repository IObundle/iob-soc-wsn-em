#pragma once

bs_standby_param_s_t bs_standby();
bs_tx_cnt_req_param_s_t bs_tx_cnt_req(uint16_t bs_adv_ch_idx, uint64_t AdvA);
bs_rx_adv_param_s_t bs_rx_adv(uint16_t bs_adv_ch_idx);
bs_rx_tmp_param_s_t bs_rx_data_tmp(uint32_t LLData_AA, uint64_t LLData_ChM, uint16_t LLData_WinOffset);
bs_tx_data_ack_param_s_t bs_tx_data_ack(uint16_t bs_data_ch_idx);
bs_end_cnt_param_s_t bs_end_cnt();
init_filter_s_t bs_initiator_filter(bs_rx_adv_param_s_t p);

#ifdef DBUG
void bs_standby_print(bs_standby_param_s_t p);
void bs_tx_cnt_req_print(bs_tx_cnt_req_param_s_t p);
void bs_rx_adv_print(bs_rx_adv_param_s_t p);
void bs_rx_data_tmp_print(bs_rx_tmp_param_s_t p);
void bs_tx_data_ack_print(bs_tx_data_ack_param_s_t p);
void bs_end_cnt_print();
#endif
