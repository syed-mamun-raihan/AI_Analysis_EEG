%-- 9/24/06 11:50 AM --%
vcor = 0;
ncor = 0;
tot = 0;

cd ../rv1
%trendGen(10,10,true);
load validation.mat;
%load validation4.mat
vaso1 = res(1,98:102);%1
nonvaso1=res(1,71:75);
vaso11 = res(2,98:102);%1
nonvaso11=res(2,71:75);
cd ../masse2
%trendGen(10,20,true);
load validation.mat;
%load validation4.mat
vaso2 = res(6,41:45);%6
nonvaso2 = res(6,1:5);
vaso21 = res(3,41:45);%6
nonvaso21 = res(3,1:5);
cd ../voth3
%trendGen(10,30,true);
load validation.mat;
%load validation9.mat
vaso3 = res(2,119:123);%5
nonvaso3 = res(2,66:70);
vaso31 = res(1,119:123);%5
nonvaso31 = res(1,66:70);
cd ../liz4/
%trendGen(10,40,true);
load validation.mat;
%load validation6.mat
vaso4=res(4,46:50);%2
nonvaso4=res(4,26:30);
vaso41=res(2,46:50);%2
nonvaso41=res(2,26:30);
cd ../coma8 
%unable to determine 2 channel
%trendGen(10,50,true);
load validation.mat;
%load validation5.mat
vaso5=res(4,6:10);%2
nonvaso5=res(4,35:39);
vaso51=res(5,6:10);%2
nonvaso51=res(5,35:39);
cd ../coma9
%trendGen(10,60,true);
load validation.mat;
%load validation2.mat
vaso6=res(4,5:9);%3
nonvaso6=res(4,41:45);
vaso61=res(6,5:9);%3
nonvaso61=res(6,41:45);
%---------------------------
%vaso61
set1v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso6];
set1n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso6];
%vaso6
set2v =[vaso1; vaso11; vaso2; vaso21; vaso3; vaso31; vaso4; vaso41; vaso5; vaso51; vaso61];
set2n =[nonvaso1; nonvaso11; nonvaso2; nonvaso21; nonvaso3; nonvaso31; nonvaso4; nonvaso41; nonvaso5; nonvaso51; nonvaso61];
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

groups = [.95;.95;.95;.95;.95;.95;.95;.95;.95;.95;.95;.05;.05;.05;.05;.05;.05;.05;.05;.05;.05;.05];
distm = 0;

class = classify(vaso61,[set1v;set1n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = mean(mahal(set1v,set1n));

class = classify(nonvaso61,[set1v;set1n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso6,[set2v;set2n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set2v,set2n)))/2;

class = classify(nonvaso6,[set2v;set2n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso1,[set3v;set3n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set3v,set3n)))/2;

class = classify(nonvaso1,[set3v;set3n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso11,[set4v;set4n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set4v,set4n)))/2;

class = classify(nonvaso11,[set4v;set4n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso2,[set5v;set5n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set5v,set5n)))/2;

class = classify(nonvaso2,[set5v;set5n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso21,[set6v;set6n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set6v,set6n)))/2;

class = classify(nonvaso21,[set6v;set6n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso3,[set7v;set7n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set7v,set7n)))/2;

class = classify(nonvaso3,[set7v;set7n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso31,[set8v;set8n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set8v,set8n)))/2;

class = classify(nonvaso31,[set8v;set8n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso4,[set9v;set9n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set9v,set9n)))/2;

class = classify(nonvaso4,[set9v;set9n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso41,[set10v;set10n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set10v,set10n)))/2;

class = classify(nonvaso41,[set10v;set10n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso5,[set11v;set11n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set11v,set11n)))/2;

class = classify(nonvaso5,[set11v;set11n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

class = classify(vaso51,[set12v;set12n],groups);
if class == 0.95
    vcor = vcor+1;
end
    tot = tot+1;
distm = (distm + mean(mahal(set12v,set12n)))/2;

class = classify(nonvaso51,[set12v;set12n],groups);
if class == 0.05
    ncor = ncor+1;
end
    tot = tot+1;

    tot = tot/2;
disp('-----------------')    
disp('Leave One Out Method with Residual using linear discriminant analysis')
disp('-----------------')
disp(sprintf('True positive segments %i out of %i',vcor, tot));
disp(sprintf('True negetive segments %i out of %i',ncor, tot));
TPF = (vcor / tot)*100;
TNF = (ncor / tot)*100;
disp(sprintf('TPF %2.2f%%',TPF));
disp(sprintf('TNF %2.2f%%',TNF));
disp(sprintf('Mean Mahalanobis distance %2.2f',distm));

cd ../sfr_leave_one
save wgn_test.mat;