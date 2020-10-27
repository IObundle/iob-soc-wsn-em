ID_HW_DIR:=$(ID_DIR)/hardware

#include
ID_INC_DIR:=$(ID_HW_DIR)/include
INCLUDE+=$(incdir)$(ID_INC_DIR)

#headers
VHDR+=$(wildcard $(ID_INC_DIR)/*.vh)

#sources
ID_SRC_DIR:=$(ID_HW_DIR)/src
VSRC+=$(wildcard $(ID_SRC_DIR)/*.v)
