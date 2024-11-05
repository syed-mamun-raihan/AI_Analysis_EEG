function [ output_args ] = fftproc( sample )
%UNTITLED1 Summary of this function goes here
%   Detailed explanation goes here
sampleStart = 1;
sampleLength = 1000;
cols = [];

sampleSize = size(sample);
for j=1:1:sampleSize(1,2)
    row = [];
    for i=0:sampleLength:(sampleSize(1,1)-sampleLength+1)
    
        curStart = i + sampleStart;
        curEnd = i+ sampleLength;

        test1 = sample(curStart:curEnd,j);
        test1 = preProcess(test1);
        result = fft(test1,1024);
        row = [row;result(1:64)];
    end
    cols = [cols row];
end
output_args = cols;
end
