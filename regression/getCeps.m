function [Fp1,Fp2,F3,F4,F7,F8,T3,T4,C3,C4,P3,P4,T5,T6] = getCeps(fileName)
%[Cepstrum] = getCeps(fileName)
%  fileName:  file of containing channel data
%  Cepstrum:  Sum Of Cepstrum Array

pack;
sample = load(fileName);
sampleStart = 1;
sampleLength = 1000;

sampleSize = size(sample);
x1 = 1/250:1/250:4;
x1 = x1';
[b,a]=butter(6,[.001,.25],'bandpass');
cols = [];
for j=1:1:sampleSize(1,2)
    rows = [];
%main loop
    for i=0:sampleLength:(sampleSize(1,1)-sampleLength+1)
    
        curStart = i + sampleStart;
        curEnd = i+ sampleLength;

        testData = sample(curStart:curEnd,j);

        %remove dc and baseline and filter
        testData = testData - mean(testData);
        px = polyfit(x1,testData,2);
        py = polyval(px,x1);
        testData = testData - py;
        testData = filter(b,a,testData);
    
% 1st pair
        ceps = abs(rceps(testData));
        rows = [rows;sum(ceps(1:12,1))];
    end
    cols = [cols rows];
end
Fp1 = cols(:,1);
Fp2 = cols(:,2);
F3 = cols(:,3);
F4 = cols(:,4);
F7 = cols(:,5);
F8 = cols(:,6);
T3 = cols(:,7);
T4 = cols(:,8);
C3 = cols(:,9);
C4 = cols(:,10);
P3 = cols(:,11);
P4 = cols(:,12);
T5 = cols(:,13);
T6 = cols(:,14);
