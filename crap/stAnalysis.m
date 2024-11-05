function [pst, st_time,st_frq] = stAnalysis(input)
[st_mat,st_time,st_frq]=st(input,-1,-1,.004,1);
st_frq = st_frq(5:65);
points = 10:10:1000;
st_mat = st_mat(5:65,:);
power = st_mat .* conj(st_mat);
power(power < 0.0001) = 0;
pst = power;