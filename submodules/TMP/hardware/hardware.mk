TMP_HW_DIR:=$(TMP_DIR)/hardware

#include
TMP_INC_DIR:=$(TMP_HW_DIR)/include
INCLUDE+=$(incdir)$(TMP_INC_DIR)

#headers
VHDR+=$(wildcard $(TMP_INC_DIR)/*.vh)

#sources
TMP_SRC_DIR:=$(TMP_HW_DIR)/src
VSRC+=$(wildcard $(TMP_SRC_DIR)/*.v)
