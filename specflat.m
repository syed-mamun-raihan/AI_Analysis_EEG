function SFR = specflat(sig_anal);

% computes the spectral flatness ratio (SFR).  SFR is defined as the ratio
% of the geometric and arithmetic means.  For a perfectly flat spectrum,
% the geometric and arithmetic means are the same, so the SFR is 0 dB.
% Deviation from flat spectrum will result in non-zero SFR values.

blocks = floor(length(sig_anal)/256);

clear SFR
for j=1:blocks
    index = (j-1)*250+1:j*250;
    temp_sig = sig_anal(index);
    A = lpc(temp_sig,10);
    sig_res = filter(A,1,temp_sig);
    sig_res_F = abs(fft(sig_res.*hanning(250)));
    SFR(j) = 20*log10(g_mean(sig_res_F)/mean(sig_res_F));
end;

function Gm = g_mean(x);

Gm = exp(mean(log(x)));
