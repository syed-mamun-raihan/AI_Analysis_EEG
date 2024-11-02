function cep_nn()
% cep_nn()
%Author: Syed Mamun Raihan
% INPUT : None
% OUTPUT: None
vcor = 0;
ncor = 0;
tot = 0;

for chan=1:1:6
cd ../rv1
load validation4.mat
vaso1 = cep(chan,111:115);%1
nonvaso1=cep(chan,271:275);
cd ../masse2
load validation4.mat
vaso2 = cep(chan,1:5);%6
nonvaso2 = cep(chan,46:50);
cd ../voth3
load validation9.mat
vaso3 = cep(chan,1:5);%5
nonvaso3 = cep(chan,76:80);
cd ../liz4/
load validation6.mat
vaso4=cep(chan,1:5);%2
nonvaso4=cep(chan,26:30);
cd ../coma8
load validation5.mat
vaso5=cep(chan,31:35);%2
nonvaso5=cep(chan,1:5);
cd ../coma9
load validation2.mat
vaso6=cep(chan,6:10);%3
nonvaso6=cep(chan,41:45);
%---------
set1v =[vaso1; vaso2; vaso3; vaso4; vaso5];
set1n =[nonvaso1; nonvaso2; nonvaso3; nonvaso4; nonvaso5];
set2v =[vaso2; vaso3; vaso4; vaso5; vaso6;];
set2n =[nonvaso2; nonvaso3; nonvaso4; nonvaso5; nonvaso6];
set3v =[vaso3; vaso4; vaso5; vaso6; vaso1];
set3n =[nonvaso3; nonvaso4; nonvaso5; nonvaso6; nonvaso1];
set4v =[vaso4; vaso5; vaso6; vaso1; vaso2];
set4n =[nonvaso4; nonvaso5; nonvaso6; nonvaso1; nonvaso2];
set5v =[vaso5; vaso6; vaso1; vaso2; vaso3];
set5n =[nonvaso5; nonvaso6; nonvaso1; nonvaso2; nonvaso3];
set6v =[vaso6; vaso1; vaso2; vaso3; vaso4];
set6n =[nonvaso6; nonvaso1; nonvaso2; nonvaso3; nonvaso4];
groups = [.95;.95;.95;.95;.95;.05;.05;.05;.05;.05];
%----------------------------
    data = [set1v;set1n]';
    input_range = minmax(data);
    [row1,col1] = size(data);
    clear net;
    net = newff(input_range, [row1 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,data,groups');
    class = sim(net,vaso6');
% following line is for experiment    
if class > 0.5
    vcor = vcor+1;
end
    tot = tot+1;
    class = sim(net,nonvaso6');
if class < 0.5
    ncor = ncor+1;
end
    tot = tot+1;
%----------------------------
%----------------------------
    data = [set2v;set2n]';
    input_range = minmax(data);
    [row1,col1] = size(data);
    net = newff(input_range, [row1 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,data,groups');
    class = sim(net,vaso1');
% following line is for experiment    
if class > 0.5
    vcor = vcor+1;
end
    tot = tot+1;
    class = sim(net,nonvaso1');
if class < 0.5
    ncor = ncor+1;
end
    tot = tot+1;
%----------------------------
%----------------------------
    data = [set3v;set3n]';
    input_range = minmax(data);
    [row1,col1] = size(data);
    clear net;
    net = newff(input_range, [row1 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,data,groups');
    class = sim(net,vaso2');
% following line is for experiment    
if class > 0.5
    vcor = vcor+1;
end
    tot = tot+1;
    class = sim(net,nonvaso2');
if class < 0.5
    ncor = ncor+1;
end
    tot = tot+1;
%----------------------------
%----------------------------
    data = [set4v;set4n]';
    input_range = minmax(data);
    [row1,col1] = size(data);
    clear net;
    net = newff(input_range, [row1 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,data,groups');
    class = sim(net,vaso3');
% following line is for experiment    
if class > 0.5
    vcor = vcor+1;
end
    tot = tot+1;
    class = sim(net,nonvaso3');
if class < 0.5
    ncor = ncor+1;
end
    tot = tot+1;
%----------------------------
%----------------------------
    data = [set5v;set5n]';
    input_range = minmax(data);
    [row1,col1] = size(data);
    clear net;
    net = newff(input_range, [row1 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,data,groups');
    class = sim(net,vaso4');
% following line is for experiment    
if class > 0.5
    vcor = vcor+1;
end
    tot = tot+1;
    class = sim(net,nonvaso4');
if class < 0.5
    ncor = ncor+1;
end
    tot = tot+1;
%----------------------------
%----------------------------
    data = [set6v;set6n]';
    input_range = minmax(data);
    [row1,col1] = size(data);
    clear net;
    net = newff(input_range, [row1 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,data,groups');
    class = sim(net,vaso5');
% following line is for experiment    
if class > 0.5
    vcor = vcor+1;
end
    tot = tot+1;
    class = sim(net,nonvaso5');
if class < 0.5
    ncor = ncor+1;
end
    tot = tot+1;
%----------------------------
end
    tot = tot/2;
disp('-----------------')    
disp('Test result with Sum-Of-Cepstrum using Linear Discriminant Analysis')
disp('-----------------')
disp(sprintf('True positive segments %i out of %i',vcor, tot));
disp(sprintf('True negetive segments %i out of %i',ncor, tot));
TPF = (vcor / tot)*100;
TNF = (ncor / tot)*100;
disp(sprintf('TPF %2.2f%%',TPF));
disp(sprintf('TNF %2.2f%%',TNF));
cd ../sfr_leave_one
save cep_nn.mat;
end