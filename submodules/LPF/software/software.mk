LPF_SW_DIR:=$(LPF_DIR)/software/driver
LPF_INC_DIR:=$(LPF_DIR)/hardware/include

#include
INCLUDE+=-I$(LPF_SW_DIR)

#headers
HDR+=$(LPF_SW_DIR)/*.h \
lpf_mem_map.h

#sources
SRC+=$(LPF_SW_DIR)/*.c

lpf_mem_map.h:
	sed -n 's/`LPF_ADDR_W//p' $(LPF_INC_DIR)/lpf.vh | sed 's/`/#/g' | sed "s/('d//g" | sed 's/)//g' > ./$@
