function ST=s1f(h)
%2005 12 03.  Compute ST1  
%Method: 1D Gaussian computed in w domain
[NR,N]=size(h)
nhaf=fix(N/2);
odvn=1;
if nhaf*2==N; odvn=0;end
w=(2*pi/N)*[0:nhaf -nhaf+1-odvn:-1];%frequency
ST=zeros(N,N);
Hft=fft(h);
     for k=1:N
        HW=circshift(Hft,[0,-(k-1)]);%Hft circshifted
        W=w(k)*w;%for scaling Gaussian
        G=exp(-W.^2);%Gaussian in freq domain
        %ST(:,k)=ifft(HW.*G);%Compute voice
        ST(k,:)=ifft(HW.*G);%Compute voice
     end