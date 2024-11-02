function [success, dataX, dataY] = testARModel(sample1,sample2,classA,classB)
%[success, dataX, dataY] = testARModel(sample1,sample2,classA,classB)
%------------------------------------
%   INPUT
%------------------------------------
%  sample:  variable containing channel data
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
msg_out = ['Testing AR Model Using NN, SVM, GLM(Linear-MLM), MR(Linear-LS) Discrimant Analysis' ]
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

        Ao = arburg(test1,order_length);
        dataA = Ao';
        Ao = arburg(test2,order_length);
        dataB = Ao';
        
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
     successDA = -1;
%     c1 = [];
%     length_p1 = max(size(P1_glm));
%     for i=1:1:length_p1
%    c1 = [c1 classify(P1_glm(i,:),P2_glm,T2_glm)];
%     end
%    [successDA]= calcSuccess(c1, T1_glm, classA, classB);
%-------------------------------------------------------------------------  
    success = [successNet successSVM successGLM successMR successDA];
end
%