# Signal noise ratio in dB
SNR_DB = 50
# Number of packets = 2**NR_PCTKS_EXP (exponent of base 2)
N_PCKTS_W = 2
# Packet paylod bytes 
N_PCKT_BYTE = 8

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


FIR_DIR = ../../FIR
TXRX_DIR = ../../TXRX
LPF_DIR = ../../LPF
LIMITER_DIR = ../../LIMITER

#Icarus (-pfileline=1)
CC = iverilog
CFLAGS = -W all -g2005-sv -I$(TXRX_DIR)/hardware/include

VCDS = $(wildcard *.vcd)



SRC_DIR = .

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
	$(SRC_DIR)/src/fsk_demod.v
SRC_BB_TEST = \
	$(SRC_DIR)/gf.v \
	$(SRC_DIR)/fir_tap.v \
	$(SRC_DIR)/fsk_mod_noise.sv
SRC_BB = \
	$(LPF_DIR)/hardware/src/lpf.sv \
	$(LIMITER_DIR)/hardware/src/limiter.sv



tx_rx_bb:  
	$(CC) $(CFLAGS) $(DEFINE) $(DEFINE_BB) $(SRC_TX) $(SRC_RX) $(SRC_DEMOD) $(SRC_BB) $(SRC_BB_TEST) tx_rx_bb_tb.v
	./a.out

demod_iir_cal:
	echo "L=$(L1); L_W=$(L1_W); C_W=2;COEFF_FILE='fir1.hex'" > params.m
	octave --no-window-system -W $(FIR_DIR)/software/octave/iob_fir.m
	echo "L=$(L2); L_W=$(L2_W); C_W=2; COEFF_FILE='fir2.hex'" > params.m
	octave --no-window-system -W $(FIR_DIR)/software/octave/iob_fir.m
	echo "L=$(L3); L_W=$(L3_W); C_W=$(C_W); COEFF_FILE='fir3.hex'" > params.m
	octave --no-window-system -W $(FIR_DIR)/software/octave/iob_fir.m

noise_generator:
	octave noise_gen.m

clean:
	@rm -f  *~ *.vcd \#*\# a.out params.m  *.hex

.PHONY: tx_rx_bb demod_iir_cal clean
