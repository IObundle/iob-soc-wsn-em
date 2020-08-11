IREF_HW_DIR:=$(IREF_DIR)/hardware

#include
IREF_INC_DIR:=$(IREF_HW_DIR)/include
INCLUDE+=$(incdir) $(IREF_INC_DIR)

#headers
VHDR+=$(wildcard $(IREF_INC_DIR)/*.vh)

#sources
IREF_SRC_DIR:=$(IREF_DIR)/hardware/src
VSRC+=$(wildcard $(IREF_HW_DIR)/src/*.v)
