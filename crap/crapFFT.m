function [end_val, raVar] = crapFFT(data,start)
    sampling_rate = 250;
    fft_length = 1024;
    data_len = 1000;
    t=1/sampling_rate:1/sampling_rate:4;
    f = sampling_rate *(1:sampling_rate)/fft_length;
    [len,width] = size(data);
    no_of_seconds = len / data_len;
    raVar = [];
    for i=0:1:no_of_seconds-1
        temp = data(i*data_len+1:(i+1)*data_len);
        temp = temp - mean(temp);
        plot(t,temp);ylabel('Amplitude(uV)');xlabel('Time');title('EEG Sample');GRID ON;axis([0 1 -40 40]);axis tight;
        saveas(gcf,[num2str(start) '_orig.jpg']);
        close(gcf);
        
        ffty = fft(temp,fft_length);
        pyy = 10*log10(ffty .* conj(ffty)./fft_length);
        
        plot(f(4:128),pyy(4:128));ylabel('Power (dB)');xlabel('Frequency(Hz)');title('FFT');GRID ON; axis tight;
        saveas(gcf,[num2str(start) '_fft.jpg']);
        close(gcf);

        [s_s,s_t,s_f] = st(temp,-1,-1,-1,-1);
        ps_s = 10*log10(s_s .* conj(s_s));
        mesh(f(4:128),t,ps_s(4:128,:)');xlabel('Frequency(Hz)');ylabel('Time');zlabel('Power (dB)');title('S-Transform');GRID ON;axis([1 250 1 19 0 60]);axis tight;
        saveas(gcf,[num2str(start) '_st.jpg']);
        close(gcf);
%
%       compute RA
%
        ra = sum(pyy(24:56))/sum(pyy(8:120));

        start = start + 1;

        raVar = [raVar ra];
    end
    end_val = start;
end 