function trendGen(padding,start, trend)
    disp(' ');
    % test montage creation
    raw = 100 - abs(testMontageCeation());
    disp(sprintf('(X - Y)Montage Creation Accuracy: %3.2f', raw));
    %test dc removal
    dc = 100 - abs(testDCRem());
    disp(sprintf('DC removal Accuracy: %3.2f', dc));
    % blw taste
    blw = 100 - abs(testBLWRem());
    disp(sprintf('Baseline wander Accuracy: %3.2f', blw));
    %filter test
    flt = 100 - abs(testFilterErr());
    disp(sprintf('Filter Accuracy: %3.2f', flt));
    % ArBurg test
    AoAccu = 100 - abs(testBurg());
    disp(sprintf('Burg Accuracy: %3.2f', AoAccu));
    % wgn test script is not ready yet but it is same as filter accuracy
%    wgnAccu = 100 - abs(testWgn());
%    disp(sprintf('Wgn Accuracy: %3.2f', wgnAccu));
    disp(' ');
    % running trend analysis
    if trend
        [cep,spec,ra,res,len,order] = trendAnalysis(padding, start);
    end
    save validation.mat;
end

function [cep,spec,ra,res,len,order] = trendAnalysis(padding,start)
    load Trend.txt;
    [row,col] = size(Trend);
    ceps_col = 1;
    spec_col = 2;
    ra_col = 3;
    wgn_col = 4;
    length_col=5;
    order_col=6;
    col_gap = 6;
    no_of_trend = col/col_gap
    result_cep=[];
    result_ra=[];
    result_wgn=[];
    result_spec=[];
    result_length=[];
    result_order=[];
	close all;
    for i=0:1:no_of_trend-1
        %ceps
        test_data = Trend(:,i*col_gap+ceps_col);
        [test_result_cep,t] = runTestTrend(test_data,padding, row);
        subplot(3,1,1);
        plot(t,abs(test_result_cep));
        ylabel('Cepstral');
        title('X vs Relative Time');
        grid('on');
        axis('tight');
        result_cep = [result_cep;test_result_cep];
%        pause
    %       spec
        test_data = Trend(:,i*col_gap+spec_col);
        [test_result_spec,t] = runTestTrend(test_data,padding, row);

        subplot(3,1,2);
        plot(t,abs(test_result_spec));
        ylabel('Spectral');
        grid('on');
        axis('tight');
        result_spec = [result_spec;test_result_spec];
 %       pause
    %       ra
        test_data = Trend(:,i*col_gap+ra_col);
        [test_result_ra,t] = runTestTrend(test_data,padding, row);

        subplot(3,1,3);
        plot(t,abs(test_result_ra));
        ylabel('Alpha SFR');
        grid('on');
        axis('tight');
        result_ra = [result_ra;test_result_ra];
%        pause
        %----
        saveas(gcf,[num2str(start) '_x1.jpg']);
        close(gcf);
	close all;
 %       pause
        %--------
        % wgn
        test_data = Trend(:,i*col_gap+wgn_col);
        [test_result_wgn,t] = runTestTrend(test_data,padding, row);

        subplot(3,1,1);
        plot(t,abs(test_result_wgn));
        ylabel('Residual');
        grid('on');
        axis('tight');
        result_wgn = [result_wgn;test_result_wgn];
%        pause

        %   length
        test_data = Trend(:,i*col_gap+length_col);
        [test_result_length,t] = runTestTrend(test_data,padding, row);

        subplot(3,1,2);
        plot(t,abs(test_result_length));
        ylabel('Length');
        grid('on');
        axis('tight');
        result_length = [result_length;test_result_length];
%        pause
        % order
        test_data = Trend(:,i*col_gap+order_col);
        [test_result_order,t] = runTestTrend(test_data,padding, row);

        subplot(3,1,3);
        plot(t,abs(test_result_order));
        ylabel('Order');
        grid('on');
        axis('tight');
        result_order = [result_order;test_result_order];
        %----
        saveas(gcf,[num2str(start) '_x2.jpg']);
        close(gcf);
	close all;
%       pause
        %----
        start = start + 1;
    end % end for
    cep = result_cep;
    spec = result_spec;
    ra = result_ra;
    res = result_wgn;
    len = result_length;
    order = result_order;
end % end function

%Utility functions

function [rawErr] = testMontageCeation()
    load X.txt;
    load Y.txt;
    load raw.txt;
    Z= (X-Y);
    stdv1 = xcorr(Z,'unbiased');
    stdv2 = xcorr(Z,raw,'unbiased');
    rawErr = mean((stdv1 - stdv2)*100/stdv1);
end

function [dcErr] = testDCRem()
    load raw.txt;
    load dc.txt;
    dc1 = raw - mean(raw);
    stdv1 = xcorr(dc,'unbiased');
    stdv2 = xcorr(dc,dc1,'unbiased');
    dcErr = mean((stdv1 - stdv2)*100/stdv1);
end

function [blwErr] = testBLWRem()
    load Xblw.txt;
    load blw.txt;
    load dc.txt;
    px = polyfit(Xblw,dc,2);
    py = polyval(px,Xblw);
    base1 = dc - py;
    stdv1 = xcorr(blw,'unbiased');
    stdv2 = xcorr(blw,base1,'unbiased');
    blwErr = mean((stdv1 - stdv2)*100/stdv1);
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
    stdv1 = xcorr(butter6,'unbiased');
    stdv2 = xcorr(butter6,low1,'unbiased');
    fltErr = mean((stdv1 - stdv2)*100/stdv1);
end

function [AoErr] = testBurg()
    load final.txt
    load Ao.txt;
    l = Ao(1)*250;
    Ao(1) = 1;
    P = max(size(Ao))-1;
% %test pburg
%     load Pf.txt;
%     load freq.txt;
%     length_freq = max(size(freq));
%     plot(freq,Pf);axis('tight');grid('on');
%     saveas(gcf,'spectrum.jpg');
%     close(gcf);
%     pburg(final(1:l),P);axis('tight');grid('on');
%     saveas(gcf,'pburg.jpg');
%     close(gcf);
% test coefficients
    Ax = arburg(final(1:l),P);
    stdv1 = xcorr(Ao,'unbiased');
    stdv2 = xcorr(Ao,Ax,'unbiased');
    AoErr = mean((stdv1 - stdv2)*100/stdv1);
%  %plot a sample signal
%     t = 1/250:1/250:4;
%     plot(t,final);axis('tight');grid('on');
%     saveas(gcf,'sample.jpg');
%     close(gcf);
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