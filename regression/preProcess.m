function [result] = preProcess(montage)
% [result] = preProcess(montage)
% remove dc and baseline and filter from monatge (must be length of 1000)
%  montage := input array of 1000 element
%  result := output of 1000 element

%x1 = 1/250:1/250:4;
%x1 = x1';
%[b,a]=butter(6,[1/250,20/250],'bandpass');
%px = polyfit(x1,temp,2);
%py = polyval(px,x1);
%temp = temp - py;
%result = filter(b,a,temp);
result = montage - mean(montage);
