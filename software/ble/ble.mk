include $(ADPLL_DIR)/adpll.mk
include $(ADPLL_DIR)/software/software.mk
include $(PA_DIR)/software/software.mk
include $(MIXER_DIR)/software/software.mk
include $(LPF_DIR)/software/software.mk
include $(LIMITER_DIR)/software/software.mk
include $(TXRX_DIR)/software/software.mk
include $(IREF_DIR)/software/software.mk
include $(TMP_DIR)/software/software.mk

INCLUDE+=$(incdir)$(BLE_DIR)

#local sources
HDR+=$(BLE_DIR)/*.h
SRC+=$(BLE_DIR)/*.c

#SN
INCLUDE+=$(incdir)$(BLE_DIR)/sn
HDR+=$(BLE_DIR)/sn/*.h
SRC+=$(BLE_DIR)/sn/sn_fct.c
SRC+=$(BLE_DIR)/sn/sn_as.c
ifeq ($(DBUG),1)
SRC+=$(BLE_DIR)/sn/sn_fct_print.c
endif

#BS
INCLUDE+=$(incdir)$(BLE_DIR)/bs
HDR+=$(BLE_DIR)/bs/*.h
SRC+=$(BLE_DIR)/bs/bs_fct.c
SRC+=$(BLE_DIR)/bs/bs_im.c
ifeq ($(DBUG),1)
SRC+=$(BLE_DIR)/bs/bs_fct_print.c
endif
