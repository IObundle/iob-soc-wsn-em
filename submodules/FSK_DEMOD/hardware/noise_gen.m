clear all
close all
clc
pkg load control
pkg load signal
pkg load statistics
%% Creates a .txt file containing the noise floor

% Sampling frequency
fs = 100e6;

% Noise Power per Hz
Pn_dBm_Hz = -174;
Pn_W = 10^((Pn_dBm_Hz - 30)/10) * fs/2;

%RMS voltage noise in a 50 ohm load
Vn_rms = sqrt(50 * Pn_W);

% Simulation time
nr_packets = 128
packet_len = 8 + 31 + 296 + 24;
packet_gap_len = 40;

nr_points = (nr_packets * (packet_len + packet_gap_len)+20) * fs / 1e6;
simulation_noise_time = nr_points * 1/fs 

%creates vector with noise points
%Ampl = normrnd(0, Vn_rms, [nr_points 1]); 

%Ampl = Vn_rms*randn([nr_points 1 ]);

fileID = fopen('noise_floor.txt','w');

 %Generates vector with noise
for i = 1: 1: nr_points;
 Ampl(i) = normrnd(0, Vn_rms, [1 1]);
 fprintf(fileID,'%.3e ',Ampl(i));
end
fclose(fileID);

xout = Ampl;

%--------------------- FFT --------------------------------
figure

m = [2^(nextpow2(length(xout))-1)]; #vector with number of points

for i=1:1:length(m);

N = m(i);

xdft = fft(xout,N);

xdft = xdft(1:N/2+1); # gets single side band

psdx = (1/(fs*N)) * abs(xdft).^2;

psdx(2:end-1) = 2*psdx(2:end-1);

freq = 0:fs/N:fs/2;

subplot(2,1,i)

plot(freq,10*log10(psdx))

text_t = sprintf('PSD using N-point FFT, with N = %d', m(i));

title(text_t, 'FontSize', 24)

xlabel('Frequency [Hz]', 'FontSize', 24)

ylabel('PSD [V**2/Hz]', 'FontSize', 24)

grid on

%semilogx(freq,10*log10(psdx))

# Assuming 50 ohm load:

end
Power1 = 2 * sum(psdx)*fs/N /(2*50) #factor of two because is 1-sided
Power1_dbm = 10*log10(Power1)+30


fmin = 500e3;
fmax = 1500e3;

%Find index more close to fmin and fmax
[dmin, idx_min] = min(abs(freq - fmin));
[dmin, idx_max] = min(abs(freq - fmax));
freq(idx_min)
freq(idx_max)

Power_chan = 2 * sum(psdx(idx_min:idx_max))*fs/N /(2*50);
Power_chan_dbm = 10*log10(Power_chan)+30
