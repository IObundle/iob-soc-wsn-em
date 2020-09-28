LIMITER_HW_DIR:=$(LIMITER_DIR)/hardware

#include
LIMITER_INC_DIR:=$(LIMITER_HW_DIR)/include
INCLUDE+=$(incdir)$(LIMITER_INC_DIR)

#headers
VHDR+=$(wildcard $(LIMITER_INC_DIR)/*.vh)

#sources
LIMITER_SRC_DIR:=$(LIMITER_HW_DIR)/src
VSRC+=$(wildcard $(LIMITER_SRC_DIR)/*.v)
