FIR_DIR:=$(SUBMODULES_DIR)/FIR
FSK_DEMOD_DIR:=$(SUBMODULES_DIR)/FSK_DEMOD

include $(FIR_DIR)/hardware/hardware.mk
include $(FSK_DEMOD_DIR)/hardware/hardware.mk

TXRX_HW_DIR:=$(TXRX_DIR)/hardware

#include
TXRX_INC_DIR:=$(TXRX_HW_DIR)/include
INCLUDE+=$(incdir) $(TXRX_INC_DIR)

#headers
VHDR+=$(wildcard $(TXRX_INC_DIR)/*.vh)

#sources
TXRX_SRC_DIR:=$(TXRX_HW_DIR)/src
VSRC+=$(wildcard $(TXRX_SRC_DIR)/*.v)
