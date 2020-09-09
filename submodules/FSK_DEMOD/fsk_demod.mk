# fir filter lengths
N1_W = 5
N2_W = 4
N3_W = 4
N4_W = 2
# coefficient width (min is 2 for all 1s)
C_W = 2
# cut frequency
FC = 600
# compare histeresis
H = 1
TH = 9
TL = -45

FNS1 = 1 #mov av 32
FNS2 = 1 #mov av 16
FNS3 = 1 #mov av 16
FNS4 = 0 #transition detect

#snr=14dB works with 3 filters, T=45 and H=10


DEFINE+=$(defmacro)TL=$(TL) $(defmacro)TH=$(TH) $(defmacro)H=$(H) $(defmacro)L1_W=$(N1_W) $(defmacro)C1_W=$(C_W) $(defmacro)L2_W=$(N2_W) $(defmacro)C2_W=$(C_W) $(defmacro)L3_W=$(N3_W) $(defmacro)C3_W=$(C_W) $(defmacro)L4_W=$(N4_W) $(defmacro)C4_W=$(C_W)
