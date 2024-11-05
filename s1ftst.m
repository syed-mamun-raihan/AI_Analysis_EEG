%S1fTST.m
%051024:  Test S2s1N.m for agreement between FFT and FFT from ST2
%Agreement (abs(fft(h2)) - abs(sum(ST2(h2)))/abs(fft(h2))  < 10^(-12)
%Time for N=32 is 3.375sec
clear all
delete(get(0,'Children'))
%Form 2D data
%%%%%%%%%%%%%
N=32;
N21=fix(N/2)+1;
nmid=fix(N/2);
odvn=1; if fix(N/2)*2==N; odvn=0;end
kl=0; ku=nmid;
M=ku-kl+1; MT=2*M-1;
if(odvn==1 & kl~=0)|(odvn==0 & kl~=0 & ku~=nmid);MT=2*M;
elseif (odvn==0 & kl==0 & ku==nmid);MT=2*M-2;end
fq=[0:nmid -nmid+1-odvn:-1]; %fft symmetry
fqs(1:M)=fq(kl+1:ku+1);
%Form test data set, h2(,)
%2-D Chirp
h=zeros(N);
  fql=1; %Start of chirp low freq
  fqu=nmid;  %End of Chirp high freq
  angl=(pi/180)*10; %Angle of increase of chirp
  fqrl=fql*cos(angl); %Start freq along row (y axis)
  fqru=fqu*cos(angl);%End freq along row (y axis)
  tpixn=2*pi*[0:N-1]/N; %distance along row, or column
  ft=linspace(fqrl,fqru,N); %increase the frequency linearly along the row
h=cos(ft.*tpixn); %multiply with distance, with chirp frequency, and repeat
tic
ST=s1f(h);
toc
STc=circshift(ST,[0,-nmid-1]);
FH=fft(h);
stsum=sum(ST, 2);
stsum=rot90(stsum,-1);
stsum=circshift(stsum,[0,-nmid-1]);
figure
plot(1:N,h)
  title('Original 1D Dataset')
  
  figure
plot(1:N,abs(FH))
title('FFT all');

figure
pcolor(1:N,1:N,abs(ST))
title('ST');
   colorbar
shading flat

figure
pcolor(1:N,1:N,abs(STc))
title('ST');
   colorbar
shading flat
  
figure
plot(1:N,abs(stsum))
title('fftshifted STsum');

figure
plot(1:N,100*((abs(FH)-abs(stsum))./abs(FH)))
title('percent diff (abs(FH)-abs(STsum))./abs(FH)');

