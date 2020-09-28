include $(FSK_DEMOD_DIR)/fsk_demod.mk

FSK_DEMOD_HW_DIR:=$(FSK_DEMOD_DIR)/hardware

#include
FSK_DEMOD_INC_DIR:=$(FSK_DEMOD_HW_DIR)/include
INCLUDE+=$(incdir)$(FSK_DEMOD_INC_DIR)

#headers
VHDR+=$(wildcard $(FSK_DEMOD_INC_DIR)/*.vh)

#sources
FSK_DEMOD_SRC_DIR:=$(FSK_DEMOD_HW_DIR)/src
VSRC+=$(wildcard $(FSK_DEMOD_SRC_DIR)/*.v)
