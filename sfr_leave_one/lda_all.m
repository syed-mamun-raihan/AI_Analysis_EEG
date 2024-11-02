function lda_all()
%-- 9/24/06 11:50 AM --%

    vcor = 0;
    ncor = 0;
    tot = 0;

    [test,setv,setn,groups]=loadData();
%-------------------------
    all_vaso =[test(1,:);setv]; 
    all_nonvaso = [test(2,:);setn];
%------------------------
    start=1;
    padding=11;
    t=1;
    tgap=1;
    for c=1:1:max(size(test))/2
        endrow=start+padding-1;
        class = classify(test(t,:),[setv(start:endrow,:);setn(start:endrow,:)],groups,'quadratic', 'empirical');
        if class == 1
               vcor = vcor+1;
        else
            disp(sprintf('vaso %i failed',c));
        end
        tot = tot+1;
        t=t+tgap;
        class = classify(test(t,:),[setv(start:endrow,:);setn(start:endrow,:)],groups,'quadratic', 'empirical');
        if class == 0
            ncor = ncor +1;
        else
            disp(sprintf('nonvaso %i failed',c));
        end
        tot = tot+1;
        start = endrow+1;
        t=t+tgap;
    end;

    tot = tot/2;

disp('-----------------')
disp('Leave One Out Method with Alpha SFR + Cepstrum Distance using linear discriminant analysis')
disp('-----------------')
disp(sprintf('True positive segments %i out of %i',vcor, tot));
disp(sprintf('True negetive segments %i out of %i',ncor, tot));
TPF = (vcor / tot)*100;
TNF = (ncor / tot)*100;
disp(sprintf('TPF %2.2f%%',TPF));
disp(sprintf('TNF %2.2f%%',TNF));
disp(sprintf('Chi-Squared Goodness of fit with Yates correction %2.2f',((tot-vcor - 0.5)^2)/vcor+((tot-ncor - 0.5)^2)/ncor));
cd ../sfr_leave_one
save lda_all.mat;
end

function [test,train1,train2,groups]= loadData()
    cd ../rv1
%trendGen(10,10,true);
load validation.mat;
%load validation4.mat
vaso1 = [ra(1,98:102) cep(1,98:102)];%1
nonvaso1=[ra(1,71:75) cep(1,71:75)];
vaso11 = [ra(2,98:102) cep(2,98:102)];%1
nonvaso11=[ra(2,71:75) cep(2,71:75)];
cd ../masse2
%trendGen(10,20,true);
load validation.mat;
%load validation4.mat
vaso2 = [ra(6,41:45) cep(6,41:45)];%6
nonvaso2 = [ra(6,1:5) cep(6,1:5)];
vaso21 = [ra(3,41:45) cep(3,41:45)];%3
nonvaso21 = [ra(3,1:5) cep(3,1:5)];
cd ../voth3
%trendGen(10,30,true);
load validation.mat;
%load validation9.mat
vaso3 = [ra(2,1:5) cep(2,1:5)];%5
nonvaso3 = [ra(2,66:70) cep(2,66:70)];
vaso31 = [ra(1,1:5) cep(1,1:5)];%5
nonvaso31 = [ra(1,66:70) cep(1,66:70)];
cd ../liz4/
%trendGen(10,40,true);
load validation.mat;
%load validation6.mat
vaso4=[ra(4,51:55) cep(4,51:55)];%4
nonvaso4=[ra(4,6:10) cep(4,6:10)];
vaso41=[ra(2,51:55) cep(2,51:55)];%2
nonvaso41=[ra(2,6:10) cep(2,6:10)];
cd ../coma8 
%trendGen(10,50,true);
load validation.mat;
%load validation5.mat
vaso5=[ra(4,6:10) cep(4,6:10)];%2
nonvaso5=[ra(4,35:39) cep(4,35:39)];
vaso51=[ra(5,6:10) cep(5,6:10)];%2
nonvaso51=[ra(5,35:39) cep(5,6:10)];
cd ../coma9
%trendGen(10,60,true);
load validation.mat;
%load validation2.mat
vaso6=[ra(4,5:9) cep(4,5:9)];%4
nonvaso6=[ra(4,41:45) cep(4,41:45)];
vaso61=[ra(6,5:9) cep(6,5:9)];%6
nonvaso61=[ra(6,41:45) cep(6,41:45)];
%---------------------------
%vaso 1
set3v =[vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6; vaso61];
set3n =[nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso 11
set4v =[vaso1; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6; vaso61];
set4n =[nonvaso1; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso2
set5v =[vaso1; vaso11; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6; vaso61];
set5n =[nonvaso1; nonvaso11; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso21
set6v =[vaso1; vaso11; vaso2; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6; vaso61];
set6n =[nonvaso1; nonvaso11; nonvaso2; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso3
set7v =[vaso1; vaso11; vaso2; vaso21; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6; vaso61];
set7n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso31
set8v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso4; vaso41; vaso5; vaso51; vaso6; vaso61];
set8n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso4
set9v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso41; vaso5; vaso51; vaso6; vaso61];
set9n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso41; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso41
set10v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso5; vaso51; vaso6; vaso61];
set10n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso5; nonvaso51; nonvaso6; nonvaso61];
%vaso5
set11v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso51; vaso6; vaso61];
set11n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso51; nonvaso6; nonvaso61];
%vaso51
set12v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso6; vaso61];
set12n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso6; nonvaso61];
%vaso6
set2v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso61];
set2n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso61];
%vaso61------------------------------
set1v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6];
set1n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6];

test=[];
test = [vaso1;nonvaso1;vaso11;nonvaso11;vaso2;nonvaso2;vaso21;nonvaso21];
test = [test;vaso3;nonvaso3;vaso31;nonvaso31;vaso4;nonvaso4;vaso41;nonvaso41];
test = [test;vaso5;nonvaso5;vaso51;nonvaso51;vaso6;nonvaso6;vaso61;nonvaso61];

train1 = [];
train2 = [];
train1 = [train1;set3v;set4v;set5v;set6v;set7v;set8v;set9v;set10v;set11v;set12v;set2v;set1v];
train2 = [train2;set3n;set4n;set5n;set6n;set7n;set8n;set9n;set10n;set11n;set12n;set2n;set1n];

groups = [1;1;1;1;1;1;1;1;1;1;1;0;0;0;0;0;0;0;0;0;0;0];
end
