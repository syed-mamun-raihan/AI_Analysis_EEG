cd ../rv1
load validation4.mat
vaso1 = wgn(1,111:115);
nonvaso1=wgn(1,271:275);
cd ../masse2
load validation4.mat
vaso2 = wgn(6,1:5);
nonvaso2 = wgn(6,46:50);
cd ../voth3
load validation9.mat
vaso3 = wgn(5,1:5);
nonvaso3 = wgn(5,76:80);
cd ../liz4/
load validation6.mat
vaso4=wgn(2,1:5);
nonvaso4=wgn(2,26:30);
cd ../coma8
load validation5.mat
vaso5=wgn(2,31:35);
nonvaso5=wgn(2,1:5);
cd ../coma9
load validation2.mat
vaso6=wgn(3,6:10);
nonvaso6=wgn(3,41:45);
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
vcor = 0;
ncor = 0;
tot = 0;
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
save wgn_nn.mat;
