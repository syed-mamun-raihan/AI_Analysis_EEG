function [success, dataX, dataY] = testCeps(sample1,sample2,classA,classB)
%[success, dataX, dataY] = testCeps(sample1,sample2,classA,classB)
% Author : Syed Mamun Raihan
%------------------------------------
%   INPUT
%------------------------------------
%  sample1:  variable containing channel data
%  sample2:  MONTAGE2 such as F4Fz - Numeric Value
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
msg_out = ['Testing Cepstrum Model Using NN, SVM, GLM(Linear-MLM), MR(Linear-LS) Discriminat Analysis' ]
sampleStart = 1;
sampleLength = 1000;

inputTrain = [];
inputTest = [];
targetTrain = [];
targetTest = [];
inputData = [];
targetData = [];

sampleSize = min([max(size(sample1)) max(size(sample2))]);
trainOrTest = 0;
order_length = 12;
    for i=0:sampleLength:(sampleSize-sampleLength+1)
    
        curStart = i + sampleStart;
        curEnd = i+ sampleLength;

        test1 = sample1(curStart:curEnd);
        test2 = sample2(curStart:curEnd);

  % 1st pair
        ceps = rceps(test1);
        dataA = ceps(1:order_length,1);
        ceps = rceps(test2);
        dataB = ceps(1:order_length,1);
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
    T2 = targetTest;

    input_range = minmax([P1 P2]);
    size_data = size(P1);
    net = newff(input_range, [size_data(1,1) 1],{'tansig','logsig'},'trainlm');
    net.trainParam.epochs = 50;
    net.trainParam.goal = 0.03;
    net.trainParam.min_grad = 0.01;
    
    net = train(net,P1,T1);
% following line is for experiment    
%-------------------------------------------
    temp = sim(net,P2);
    [successNet]= calcSuccess(temp, T2, classA, classB);
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
    P1_glm = P1';
    T1_glm = T1';
    P2_glm = P2';
    T2_glm = T2';
    [b,dev,statsg] = glmfit(P1_glm, T1_glm);
    temp = glmval(b,P2_glm,'identity');
    [successGLM]= calcSuccess(temp', T2, classA, classB);
%-------------------------------------------------------------------------  
    whichstats = {'yhat','r'};
    stats = regstats(dataY,dataX,'linear', whichstats);
    [successMR]= calcSuccess(stats.yhat', dataY', classA, classB);
%-------------------------------------------------------------------------  
     successDA = 0;
     c1 = [];
     length_p2 = max(size(P2_glm));
     for i=1:1:length_p2
    c1 = [c1 classify(P2_glm(i,:),P1_glm,T1_glm)];
     end
    [successDA]= calcSuccess(c1, T2_glm, classA, classB);
%-------------------------------------------------------------------------  
    success = [successNet successSVM successGLM successMR successDA];
end
%