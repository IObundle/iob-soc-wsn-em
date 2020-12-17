ROOT_DIR = .
HW_DIR = $(ROOT_DIR)/hardware
INC_DIR = $(HW_DIR)/include
SRC_DIR = $(HW_DIR)/src
TB_DIR = $(HW_DIR)/testbench
SW_DIR = $(ROOT_DIR)/software
OCT_DIR = $(SW_DIR)/octave

SUBMODULES_DIR = ..
FIR_DIR = $(SUBMODULES_DIR)/FIR
TXRX_DIR = $(SUBMODULES_DIR)/TXRX
LPF_DIR = $(SUBMODULES_DIR)/LPF
LIMITER_DIR = $(SUBMODULES_DIR)/LIMITER

defmacro:=-D

include $(ROOT_DIR)/fsk_demod.mk

INCLUDE=-I$(INC_DIR) -I$(TXRX_DIR)/hardware/include

# Testbench tx_rx_bb defines
DEFINE_BB=$(defmacro)N_PCKTS_W=$(N_PCKTS_W) $(defmacro)N_PCKT_BYTE=$(N_PCKT_BYTE)

#Icarus (-pfileline=1)
CC = iverilog
CFLAGS = -Wall -g2005-sv

VCDS = $(wildcard *.vcd)

SRC_TX = \
	$(TXRX_DIR)/hardware/src/syn_fifo.v \
	$(TXRX_DIR)/hardware/src/regf_dp_d0w_d1r.v \
	$(TXRX_DIR)/hardware/src/byte_p2s.v \
	$(TXRX_DIR)/hardware/src/whitening.v \
	$(TXRX_DIR)/hardware/src/crc.v \
	$(TXRX_DIR)/hardware/src/tx.v
SRC_RX = \
	$(TXRX_DIR)/hardware/src/byte_s2p.v \
	$(TXRX_DIR)/hardware/src/aa_find.v \
	$(TXRX_DIR)/hardware/src/rx.v
SRC_DEMOD = \
	$(FIR_DIR)/hardware/src/iob_fir.v \
	$(SRC_DIR)/fsk_demod.v
SRC_BB_TEST = \
	$(TB_DIR)/BB/gf.v \
	$(TB_DIR)/BB/fir_tap.v \
	$(TB_DIR)/BB/fsk_mod_noise.sv
SRC_BB = \
	$(LPF_DIR)/hardware/testbench/module_tb.sv \
	$(LIMITER_DIR)/hardware/testbench/module_tb.sv

IOB_FIR_M = $(FIR_DIR)/software/octave/iob_fir.m

fsk_demod: demod_coeffs noise_floor.txt
	$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) $(DEFINE_BB) $(SRC_TX) $(SRC_RX) $(SRC_DEMOD) $(SRC_BB) $(SRC_BB_TEST) $(TB_DIR)/fsk_demod_tb.v
	./a.out

demod_coeffs:
	echo "L=$(L1); L_W=$(L1_W); C_W=$(C_W); COEFF_FILE='fir1coeffs.v';" > params.m
	octave-cli $(IOB_FIR_M)
	echo "L=$(L2); L_W=$(L2_W); C_W=$(C_W); COEFF_FILE='fir2coeffs.v';" > params.m
	octave-cli $(IOB_FIR_M)
	echo "L=$(L3); L_W=$(L3_W); C_W=$(C_W); COEFF_FILE='fir3coeffs.v';" > params.m
	octave-cli $(IOB_FIR_M)

noise_floor.txt: $(OCT_DIR)/noise_gen.m
	octave $(OCT_DIR)/noise_gen.m

clean:
	@rm -f  *~ *.vcd \#*\# a.out params.m *.v

.PHONY: fsk_demod demod_coeffs clean
