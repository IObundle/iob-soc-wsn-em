MIXER_HW_DIR:=$(MIXER_DIR)/hardware

#include
MIXER_INC_DIR:=$(MIXER_HW_DIR)/include
INCLUDE+=$(incdir)$(MIXER_INC_DIR)

#headers
VHDR+=$(wildcard $(MIXER_INC_DIR)/*.vh)

#sources
MIXER_SRC_DIR:=$(MIXER_HW_DIR)/src
VSRC+=$(wildcard $(MIXER_SRC_DIR)/*.v)
