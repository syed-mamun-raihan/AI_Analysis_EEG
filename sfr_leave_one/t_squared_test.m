cd ../rv1
%trendGen(10,10,true);
load validation.mat;
%load validation4.mat
vaso1 = ra(1,98:102);%1
nonvaso1=ra(1,71:75);
vaso11 = ra(2,98:102);%1
nonvaso11=ra(2,71:75);
cd ../masse2
%trendGen(10,20,true);
load validation.mat;
%load validation4.mat
vaso2 = ra(6,41:45);%6
nonvaso2 = ra(6,1:5);
vaso21 = ra(3,41:45);%6
nonvaso21 = ra(3,1:5);
cd ../voth3
%trendGen(10,30,true);
load validation.mat;
%load validation9.mat
vaso3 = ra(2,1:5);%5
nonvaso3 = ra(2,66:70);
vaso31 = ra(1,1:5);%1
nonvaso31 = ra(1,66:70);
cd ../liz4/
%trendGen(10,40,true);
load validation.mat;
%load validation6.mat
vaso4=ra(4,51:55);%4
nonvaso4=ra(4,6:10);
vaso41=ra(2,51:55);%2
nonvaso41=ra(2,6:10);
cd ../coma8 
%unable to determine 2 channel
%trendGen(10,50,true);
load validation.mat;
%load validation5.mat
vaso5=ra(4,6:10);%2
nonvaso5=ra(4,35:39);
vaso51=ra(5,6:10);%2
nonvaso51=ra(5,35:39);
cd ../coma9
%trendGen(10,60,true);
load validation.mat;
%load validation2.mat
vaso6=ra(4,5:9);%3
nonvaso6=ra(4,41:45);
vaso61=ra(6,5:9);%3
nonvaso61=ra(6,41:45);
%return
cd ../sfr_leave_one;
%---------------------------
%vaso61
set1v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6];
set1n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6];
%-------------------------
%all_vaso =mean([set1v;vaso61]')'; %x
%all_nonvaso = mean([set1n;nonvaso61]')';%y
all_vaso =[set1v;vaso61]; %x
all_nonvaso = [set1n;nonvaso61];%y
[row,col]=size(all_vaso);
sum_xx = 0;
sum_x = 0;
sum_yy = 0;
sum_y = 0;
sum_xy = 0;
for j=1:1:col
for i=1:1:row
    sum_xx = sum_xx + all_vaso(i,j)^2;
    sum_yy = sum_yy + all_nonvaso(i,j)^2;
    sum_xy = sum_xy + all_vaso(i,j)*all_nonvaso(i,j);
    sum_x = sum_x + all_vaso(i,j);
    sum_y = sum_y + all_nonvaso(i,j);
end
end
df=(row*col)-1;
ss_x = sum_xx - (sum_x^2)/(df+1);
ss_y = sum_yy - (sum_y^2)/(df+1);
sp_xy = sum_xy - (sum_x*sum_y)/(df+1);
r = sp_xy/sqrt(ss_y*ss_x);
t = r*sqrt(df)/sqrt(1-r^2);

disp(sprintf('pearson product moment correlation %2.2f%%',r));
disp(sprintf('t-calculated %2.2f%',t));
disp(sprintf('df %2i',(row*col)-1));
save t_test.mat