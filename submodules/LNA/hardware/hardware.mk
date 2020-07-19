LNA_HW_DIR:=$(LNA_DIR)/hardware

#include
LNA_INC_DIR:=$(LNA_HW_DIR)/include
INCLUDE+=$(incdir) $(LNA_INC_DIR)

#headers
VHDR+=$(wildcard $(LNA_INC_DIR)/*.vh)

#sources
LNA_SRC_DIR:=$(LNA_DIR)/hardware/src
VSRC+=$(wildcard $(LNA_HW_DIR)/src/*.v)
