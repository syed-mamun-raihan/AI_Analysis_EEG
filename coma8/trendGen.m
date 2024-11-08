function trendGen(padding, trend)
    disp(' ');
    % test montage creation
    AccRaw = 100 - abs(testMontageCeation());
    disp(sprintf('(X - Y)Montage Creation Accuracy: %3.2f', AccRaw));
    %test dc removal
    AccDc = 100 - abs(testDCRem());
    disp(sprintf('DC removal Accuracy: %3.2f', AccDc));
    % blw taste
    AccBlw = 100 - abs(testBLWRem());
    disp(sprintf('Baseline wander Accuracy: %3.2f', AccBlw));
    %filter test
    AccFlt = 100 - abs(testFilterErr());
    disp(sprintf('Filter Accuracy: %3.2f', AccFlt));
    %filter test
    AccMA = 100 - abs(testMAFilterErr());
    disp(sprintf('MA Filter Accuracy: %3.2f', AccMA));
    % FFT Filter text
    AccFft = 100 - abs(testFilterByFFTErr());
    disp(sprintf('Filter by FFT Accuracy: %3.2f', AccFft));
    % ArBurg test
    AccBurg = 100 - abs(testBurg());
    disp(sprintf('Burg Accuracy: %3.2f', AccBurg));
    % wgn test script is not ready yet but it is same as filter accuracy
%    wgnAccu = 100 - abs(testWgn());
%    disp(sprintf('Wgn Accuracy: %3.2f', wgnAccu));
    disp(' ');
    % running trend analysis
    if trend
        load alphaout.txt;
        [ra] = trendAnalysis(padding, alphaout, 1);
        load cepsout.txt;
        [cep] = trendAnalysis(padding, cepsout, 2);
        load spec.txt;
        [spec] = trendAnalysis(padding, specout, 3);
        load sfrout.txt;
        [res] = trendAnalysis(padding, sfrout, 4);
        load lengthsout.txt;
        [len] = trendAnalysis(padding, lengthsout, 5);
        load ordersout.txt;
        [order] = trendAnalysis(padding, ordersout, 6);
    end
    save validation.mat;
end

function [data] = trendAnalysis(padding, Trend, trendID)
    [row,col] = size(Trend);
    result=[];
	close all;
    for i=1:6
        test_data = Trend(:,i);
        [test_result,t] = runTestTrend(test_data,padding, row);
        doFCMGraph(test_result,t,i,trendID);
        result = [result;test_result];
    end % end for
    data = result;
end % end function

%Utility functions

function [rawErr] = testMontageCeation()
    load X.txt;
    load Y.txt;
    load raw.txt;
    rawErr = mean(((X-Y)-raw)./(X-Y))*100;
end

function [dcErr] = testDCRem()
    load raw.txt;
    load dc.txt;
    dc1 = raw - mean(raw);
    dcErr = mean((dc1 - dc)./dc1)*100;
end

function [blwErr] = testBLWRem()
    load Xblw.txt;
    load blw.txt; %output
    load dc.txt;
    px = polyfit(Xblw,dc,2);
    py = polyval(px,Xblw);
    base1 = dc - py;
    blwErr = mean((base1 - blw)./base1)*100;
end

function [MAErr] = testMAFilterErr()
    load blw.txt;%input
    load ma.txt;%output
%[B,A]=butter(6,[1/500,32/250],'bandpass');
    a=1;
    b=[];
    MAPoints = 4;
    for n=1:MAPoints;
        b=[b 1/MAPoints];
    end
    %[B,A]=butter(6,0.2,'low');
    low1 = filter(b,a,blw(1:250));
    low1 = [low1 filter(b,a,blw(251:500))];
    low1 = [low1 filter(b,a,blw(501:750))];
    low1 = [low1 filter(b,a,blw(751:1000))];
    MAErr = mean((low1 - ma)./low1)*100;
end

function [fltErr] = testFilterErr()
    load ma.txt;%input
    load butter6.txt;%output
%[B,A]=butter(6,[1/500,32/250],'bandpass');
    load ../b.txt;
    load ../a.txt;
    %[B,A]=butter(6,0.2,'low');
    low1 = filter(b,a,ma(1:250));
    low1 = [low1 filter(b,a,ma(251:500))];
    low1 = [low1 filter(b,a,ma(501:750))];
    low1 = [low1 filter(b,a,ma(751:1000))];
    fltErr = mean((low1 - butter6)./low1)*100;
end

function [fftErr] = testFilterByFFTErr()
    load hanncorec.txt;
    load final.txt;
    fftY = fft(hanncorec(1:250));
    fftY(1:2) = 0;
    fftY(32:250)=0;
    fftY = real(ifft(fftY));
    fftErr = mean((fftY - final(1:250))./fftY)*100;
    fftY = fft(hanncorec(251:500));
    fftY(1:2) = 0;
    fftY(32:250)=0;
    fftY = real(ifft(fftY));
    fftErr = fftErr + mean((fftY - final(251:500))./fftY)*100;
    fftY = fft(hanncorec(501:750));
    fftY(1:2) = 0;
    fftY(32:250)=0;
    fftY = real(ifft(fftY));
    fftErr = fftErr + mean((fftY - final(501:750))./fftY)*100;
    fftY = fft(hanncorec(751:1000));
    fftY(1:2) = 0;
    fftY(32:250)=0;
    fftY = real(ifft(fftY));
    fftErr = fftErr + mean((fftY - final(751:1000))./fftY)*100;
    fftErr = fftErr / 4;
end

function [AoErr] = testBurg()
    load final.txt
    load Ao.txt;
    l = Ao(1)*250;
    Ao(1) = 1;
    P = max(size(Ao))-1;
%test pburg
    load Pf.txt;
    load freq.txt;
    length_freq = max(size(freq));
    plot(freq,Pf);axis('tight');grid('on');
    saveas(gcf,'spectrum.jpg');
    close(gcf);
    pburg(final(1:l),P);axis('tight');grid('on');
    saveas(gcf,'pburg.jpg');
    close(gcf);
% test coefficients
    Ax = arburg(final(1:l),P);
    AoErr = mean((Ax - Ao)./Ax)*100;
    %plot a sample signal
    t = 1/250:1/250:4;
    plot(t,final);axis('tight');grid('on');
    saveas(gcf,'sample.jpg');
    close(gcf);
end

function [wgnErr] = testWgn()
   load residualX.txt;
   load residualY.txt;
   load resAo.txt;
   [row,col]=size(residualX);
   X = filter(resAo,1,residualY(1:col));
   wgnErr = mean((X - residualX)./X)*100;
end

function [y,x] = runTestTrend(test_data,padding, row)
        test_Trend=[];
        t=[];
        for j=1:1:row
            if(j < padding+1)
                if(padding+j < row)
                            mean_val = mean(test_data(j:j+padding));
                else
                            mean_val = mean(test_data(j:row));
                end
        %		min_val = min(test_data(j:j+padding));
            else
                if(j+padding > row)
                    mean_val = mean(test_data(j-padding:j));
            %		min_val = min(test_data(j-padding:j));
                else
                    mean_val = mean(test_data(j-padding:j+padding));
            %		min_val = min(test_data(j-padding:j+padding));
                end
            end
            test_Trend = [test_Trend mean_val];
            t = [t;j];
        end
        y = test_Trend;
        minY = abs(min(min(y))); y = y / minY;
        x = t;
end

function doFCMGraph(y,x,chnID,typeID)
    data = [x' y'];
    [center, U, obj_fcn]=fcm(data,2);
    maxU = max(U);
    % Find the data points with highest grade of membership in cluster 1
    index1 = find(U(1,:) == maxU);
    % Find the data points with highest grade of membership in cluster 2
    index2 = find(U(2,:) == maxU);
    figure
    % Plot the cluster centers
    plot([center([1 2],1)],[center([1 2],2)],'*','color','k');
    xlabel('Time');ylabel('Feature');axis('tight');grid('on');title('Clustering');
    line(data(index1,1),data(index1,2),'marker','.','color','g');
    line(data(index2,1),data(index2,2),'marker','-','color','r');
    saveas(gcf,[num2str(typeId) num2str(chnId) '.jpg');
    close(gcf);
end