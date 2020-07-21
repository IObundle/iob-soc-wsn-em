PA_HW_DIR:=$(PA_DIR)/hardware

#include
PA_INC_DIR:=$(PA_HW_DIR)/include
INCLUDE+=$(incdir) $(PA_INC_DIR)

#headers
VHDR+=$(wildcard $(PA_INC_DIR)/*.vh)

#sources
PA_SRC_DIR:=$(PA_DIR)/hardware/src
VSRC+=$(wildcard $(PA_HW_DIR)/src/*.v)
