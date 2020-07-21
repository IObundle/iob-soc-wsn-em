LPF_HW_DIR:=$(LPF_DIR)/hardware

#include
LPF_INC_DIR:=$(LPF_HW_DIR)/include
INCLUDE+=$(incdir) $(LPF_INC_DIR)

#headers
VHDR+=$(wildcard $(LPF_INC_DIR)/*.vh)

#sources
LPF_SRC_DIR:=$(LPF_DIR)/hardware/src
VSRC+=$(wildcard $(LPF_HW_DIR)/src/*.v)
