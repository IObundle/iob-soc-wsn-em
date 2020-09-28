# Signal noise ratio in dB
SNR_DB=50
# Number of packets = 2**NR_PCTKS_EXP (exponent of base 2)
N_PCKTS_W=2
# Packet paylod bytes 
N_PCKT_BYTE=8

# fir coefficient width (min is 2 for all 1s)
C_W=2

# firs
I1_W=2
O1_W=6
L1=32
L1_W=5

O2_W=11
L2=16
L2_W=4

O3_W=15
L3=16
L3_W=4

# slicer
#threshhold
T=45
#histeresis
H=10

# Demod defines
DEFINE+=$(defmacro)T=$(T) $(defmacro)H=$(H) $(defmacro)I1_W=$(I1_W) $(defmacro)O1_W=$(O1_W) $(defmacro)L1=$(L1) $(defmacro)L1_W=$(L1_W) $(defmacro)C1_W=$(C_W) $(defmacro)O2_W=$(O2_W) $(defmacro)L2=$(L2) $(defmacro)L2_W=$(L2_W) $(defmacro)C2_W=$(C_W) $(defmacro)O3_W=$(O3_W) $(defmacro)L3=$(L3) $(defmacro)L3_W=$(L3_W) $(defmacro)C3_W=$(C_W) $(defmacro)SNR_DB=$(SNR_DB)
