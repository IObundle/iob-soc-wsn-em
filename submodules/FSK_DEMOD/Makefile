defmacro:=-D

include fsk_demod.mk

IOB_FIR_M:=../FIR/software/octave/iob_fir.m

# Demodulator with testbench all in HDL (icarus tool)
demod:  
	echo "FNS=$(FNS1); N_W=$(N1_W); C_W=$(C_W); FC=$(FC);COEFF_FILE='fir1.hex';" > params.m
	octave $(IOB_FIR_M)
	echo "FNS=$(FNS2); N_W=$(N2_W); C_W=2; FC=$(FC); COEFF_FILE='fir2.hex';" > params.m
	octave $(IOB_FIR_M)
	echo "FNS=$(FNS3); N_W=$(N3_W); C_W=$(C_W); FC=$(FC); COEFF_FILE='fir3.hex';" > params.m
	octave $(IOB_FIR_M)
	echo "FNS=$(FNS4); N_W=$(N4_W); C_W=$(C_W); FC=$(FC); COEFF_FILE='fir4.hex';" > params.m
	octave $(IOB_FIR_M)
