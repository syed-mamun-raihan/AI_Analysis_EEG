function [successRate]= calcSuccess(temp, T2, classA, classB)
    length_t = size(temp); 
    errorCount = 0;
    errorLevel = abs(classA -classB)*0.25;
    if T2(1) < T2(2)
        for i=1:2:length_t(1,2)-1
            if temp(i) > temp(i+1) 
                errorCount = errorCount + 1 ;
            elseif abs(temp(i)-temp(i+1)) < errorLevel
                errorCount = errorCount + 1 ;
            end
        end
    else
        for i=1:2:length_t(1,2)-1
            if temp(i) < temp(i+1) 
                errorCount = errorCount + 1 + abs(temp(i) - temp(i+1));
            elseif abs(temp(i)-temp(i+1)) < errorLevel
                errorCount = errorCount + 1 ;
            end
        end
    end
    maxSuccess = length_t(1,2);
    successRate =  (maxSuccess - errorCount) / maxSuccess;
end