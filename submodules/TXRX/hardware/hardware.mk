TXRX_HW_DIR:=$(TXRX_DIR)/hardware

#include
TXRX_INC_DIR:=$(TXRX_HW_DIR)/include
INCLUDE+=$(incdir) $(TXRX_INC_DIR)

#headers
VHDR+=$(wildcard $(TXRX_INC_DIR)/*.vh)

#sources
TXRX_SRC_DIR:=$(TXRX_DIR)/hardware/src
VSRC+=$(wildcard $(TXRX_HW_DIR)/src/*.v) \
$(wildcard $(SUBMODULES_DIR)/FIR/hardware/src/*.v)
