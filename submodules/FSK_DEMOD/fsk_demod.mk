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


DEFINE+=  -DTL=$(TL) -DTH=$(TH) -DH=$(H) -DL1_W=$(N1_W) -DC1_W=$(C_W) -DL2_W=$(N2_W) -DC2_W=$(C_W) -DL3_W=$(N3_W) -DC3_W=$(C_W) -DL4_W=$(N4_W) -DC4_W=$(C_W)
