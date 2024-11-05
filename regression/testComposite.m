function [success, net, svmStruct, svmCp, dataX, dataY] = testCompsite(sample,raw,ch1,ch2,classA,classB)
%[success, net, svmStruct, svmCp, dataX, dataY] = testCompsite(sample,ch1,ch2,classA,classB)
%------------------------------------
%   INPUT
%------------------------------------
%  sample:  variable containing channel data
%  raw: true if data need to be filtered
%  ch1:  MONTAGE1 such as F3Fz - Numeric Value
%  ch2:  MONTAGE2 such as F4Fz - Numeric Value
%  classA:  Numeric Value to indicate 1st Type
%  classB:  Numeric Value to indicate 2nd type
%------------------------------------
%   OUTPUT
%------------------------------------
%  success: a vector of classification success using NN, SVM & GLM
%  dataX: data used through out the system (N * P)
%  dataY: data used through out the system (N * 1)

close all;

pack;
MONTAGE1 = ch1;
MONTAGE2 = ch2;
msg_out = ['Testing Composite Model with Using NN, SVM, GLM(Linear-MLM), MR(Linear-LS)' ]
sampleStart = 1;
sampleLength = 1000;

inputTrain = [];
inputTest = [];
targetTrain = [];
targetTest = [];
inputData = [];
targetData = [];

sampleSize = size(sample);
trainOrTest = 0;

    for i=0:sampleLength:(sampleSize(1,1)-sampleLength+1)
    
        curStart = i + sampleStart;
        curEnd = i+ sampleLength;

        test1 = sample(curStart:curEnd,MONTAGE1);
        test2 = sample(curStart:curEnd,MONTAGE2);
        
        %remove dc and baseline and filter
        if(raw == true)
            test1 = preProcess(test1);
            test2 = preProcess(test2);
        end

        [Cx,Rx,Lx] = princomp([test1 test2]);

         ceps = abs(rceps(test1));

         dataA = [Rx(1:36,1)./10;ceps(1:12,1)];
         
         ceps = abs(rceps(test2));
         dataB = [Rx(1:36,2)./10;ceps(1:12,1)];
        if trainOrTest == 0 
            inputTrain = [inputTrain dataA dataB];
            targetTrain = [targetTrain classA classB];
            trainOrTest = 1;
        else
            inputTest = [inputTest dataA dataB];
            targetTest = [targetTest classA classB];
            trainOrTest = 0;
        end
    end
    pack;
    P1 = inputTrain;
    T1 = targetTrain;
    P2 = inputTest;
    input_range = minmax([P1 P2]);
    size_data = size(P1);
    net = newff(input_range, [size_data(1,1) 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.01;
    net.trainParam.min_grad = 0.01;
    net.trainParam.mu = 0.01;
    net = train(net,P1,T1);
% following line is for experiment    
%-------------------------------------------
    temp = sim(net,P2);
    [Y2, successNet]= calcSuccess(temp, T2, classA, classB);
%-----------------------------------------------------------------------
    dataX = [P1 P2]';
    groups = [T1 T2];
    groups(groups == classA) = 0;
    groups(groups == classB) = 1;
    [trainX,testX] = crossvalind('holdOut',groups);
    cp = classperf(groups);
    svmStruct = svmtrain(dataX(trainX,:),groups(trainX),'kernel_function','rbf');
    classes = svmclassify(svmStruct, dataX(testX,:));
    classperf(cp, classes,testX);
    successSVM = cp.CorrectRate;
    svmCp = cp;
    groups(groups == 0) = classA;
    groups(groups == 1) = classB;
    dataY = groups';
%-------------------------------------------------------------------------  
    P1 = P1';
    T1 = T1';
    P2 = P2';
    T2 = T2';
    [b,dev,statsg] = glmfit(P1, T1);
    temp = glmval(b,P2,'identity');
    [successGLM]= calcSuccess(temp', T2', classA, classB);
%-------------------------------------------------------------------------  
    whichstats = {'yhat','r'};
    stats = regstats(dataY,dataX,'linear', whichstats);
    [successMR]= calcSuccess(stats.yhat', dataY', classA, classB);
%-------------------------------------------------------------------------  
    success = [successNet successSVM successGLM successMR];
end
%