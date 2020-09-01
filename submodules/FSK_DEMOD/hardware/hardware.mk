# Signal noise ratio in dB
SNR_DB = 80
# Number of packets = 2**NR_PCTKS_EXP (exponent of base 2)
N_PCKTS_W = 2
# Packet paylod bytes 
N_PCKT_BYTE = 8
# Channel freq in MHz
FREQ_CHANNEL = 2450.000 
# dco phase noise flag
DCO_PN = 0

# fir coefficient width (min is 2 for all 1s)
C_W = 2

# firs
I1_W = 2
O1_W = 6
L1 = 32 
L1_W = 5

O2_W = 11
L2 = 16
L2_W = 4

O3_W = 15
L3 = 16
L3_W = 4

# slicer 
T = 45  #threshhold
H = 10  #histeresis

# Demod defines
DEFINE = -DT=$(T) -DH=$(H) -DI1_W=$(I1_W) -DO1_W=$(O1_W) -DL1=$(L1) -DL1_W=$(L1_W) -DC1_W=$(C_W) -DO2_W=$(O2_W) -DL2=$(L2) -DL2_W=$(L2_W) -DC2_W=$(C_W) -DO3_W=$(O3_W) -DL3=$(L3) -DL3_W=$(L3_W) -DC3_W=$(C_W)

# Testbench tx_rx_bb defines
DEFINE_BB = -DSNR_DB=$(SNR_DB) -DN_PCKTS_W=$(N_PCKTS_W) -DN_PCKT_BYTE=$(N_PCKT_BYTE)

DEFINE_RF = -DFREQ_CHANNEL=$(FREQ_CHANNEL) -DDCO_PN=$(DCO_PN)


FIR_DIR = ../../submodules/fir
ADPLL_DIR = ../../submodules/adpll_rtl_wsn

#Icarus (-pfileline=1)
CC = iverilog
CFLAGS = -W all -g2005-sv -I../include -I$(ADPLL_DIR) 

VCDS = $(wildcard *.vcd)

#Incisive
CFLAGS_INC = -errormax 15 -status -update -linedebug -SV 
EFLAGS_INC = -errormax 15 -access +wc -status
SFLAGS_INC = -errormax 15 -status


SRC_DIR = ../src

SRC_TX = \
	$(SRC_DIR)/syn_fifo.v \
	$(SRC_DIR)/regf_dp_d0w_d1r.v \
	$(SRC_DIR)/byte_p2s.v \
	$(SRC_DIR)/whitening.v \
	$(SRC_DIR)/crc.v \
	$(SRC_DIR)/tx.v
SRC_RX = \
	$(SRC_DIR)/byte_s2p.v \
	$(SRC_DIR)/aa_find.v \
	$(SRC_DIR)/rx.v
SRC_DEMOD = \
	$(FIR_DIR)/iob_fir.v \
	$(SRC_DIR)/fsk_demod.v
SRC_BB_TEST = \
	$(SRC_DIR)/analog/gf.v \
	$(SRC_DIR)/analog/fir_tap.v \
	$(SRC_DIR)/analog/fsk_mod_noise.sv
SRC_BB = \
	$(SRC_DIR)/analog/lpf.sv \
	$(SRC_DIR)/analog/limiter_rssi.sv
SRC_RF = \
	$(SRC_DIR)/analog/rf_osc_ideal.sv \
	$(SRC_DIR)/analog/mixer.sv
SRC_ADPLL = \
	$(ADPLL_DIR)/DCO/dco.sv \
	$(ADPLL_DIR)/DCO/row_col_cod.v \
	$(ADPLL_DIR)/DCO/row_col_cod_5x5.v \
	$(ADPLL_DIR)/DCO/c_sel.sv \
	$(ADPLL_DIR)/TDC/ring_osc.sv \
	$(ADPLL_DIR)/TDC/ring_inv.sv \
	$(ADPLL_DIR)/TDC/d_ff1.sv \
	$(ADPLL_DIR)/TDC/d_ff2.sv \
	$(ADPLL_DIR)/TDC/d_latch1.sv \
	$(ADPLL_DIR)/TDC/counter.sv \
	$(ADPLL_DIR)/TDC/tdc_analog.sv \
	$(ADPLL_DIR)/TDC/tdc_digital.v \
	$(ADPLL_DIR)/adpll_ctr0.v 


tx_rx_rf_bb:  
	$(CC) $(CFLAGS) $(DEFINE) $(DEFINE_RF) $(DEFINE_BB) $(SRC_TX) $(SRC_RX) $(SRC_DEMOD) $(SRC_BB) $(SRC_RF) $(SRC_ADPLL) tx_rx_rf_bb_tb.v
	./a.out

tx_rx_bb:  
	$(CC) $(CFLAGS) $(DEFINE) $(DEFINE_BB) $(SRC_TX) $(SRC_RX) $(SRC_DEMOD) $(SRC_BB) $(SRC_BB_TEST) tx_rx_bb_tb.v
	./a.out

tx_rx:  
	$(CC) $(CFLAGS)  $(DEFINE) $(DEFINE_BB) $(SRC_TX) $(SRC_RX) tx_rx_tb.v
	./a.out
tx:  
	$(CC) $(CFLAGS) $(DEFINE_BB) $(SRC_TX) tx_tb.v
	./a.out
p2s:  
	$(CC) $(CFLAGS) $(SRC_DIR)/byte_p2s.v byte_p2s_tb.v
	./a.out

demod_iir_cal:
	echo "L=$(L1); L_W=$(L1_W); C_W=2;COEFF_FILE='fir1.hex'" > params.m
	octave --no-window-system -W $(FIR_DIR)/iob_fir.m
	echo "L=$(L2); L_W=$(L2_W); C_W=2; COEFF_FILE='fir2.hex'" > params.m
	octave --no-window-system -W $(FIR_DIR)/iob_fir.m
	echo "L=$(L3); L_W=$(L3_W); C_W=$(C_W); COEFF_FILE='fir3.hex'" > params.m
	octave --no-window-system -W $(FIR_DIR)/iob_fir.m	

clean:
	@rm -f  *~ *.vcd \#*\# a.out params.m  *.hex

.PHONY: tx clean
