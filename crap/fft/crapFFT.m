function [end_val, raVar] = crapFFT(data, start)
    sampling_rate = 250;
    data_length = 1000;
    fft_length = 1024;
    t=1/sampling_rate:1/sampling_rate:4;
    f = sampling_rate *(1:sampling_rate)/fft_length;
    size_data = size(data);
    length_data = size_data(1,1);
%    no_of_seconds = length_data / sampling_rate;
    no_of_seconds = length_data / data_length;
    raVar = [];
    for i=0:1:no_of_seconds-1
%        temp = data(i*sampling_rate+1:(i+1)*sampling_rate);
        temp = data(i*data_length+1:(i+1)*data_length);
        
        temp = temp - mean(temp);
        plot(t,temp(1:1000));ylabel('Amplitude');xlabel('Time');title('EEG Sample');GRID ON;axis tight;
        saveas(gcf,[num2str(start) '_orig.jpg']);
        close(gcf);
        
        ffty = fft(temp,fft_length);
        pyy = 10*log10(ffty .* conj(ffty)./fft_length);
        
        plot(f(4:80),pyy(4:80));ylabel('Power (dB)');xlabel('Frequency(Hz)');title('Fast Fourier Transform of EEG Sample');GRID ON; axis tight;
        saveas(gcf,[num2str(start) '_fft.jpg']);
        close(gcf);

        [s_s,s_t,s_f] = st(temp(1:1000),-1,-1,-1,-1);
        ps_s = 10*log10(s_s .* conj(s_s));

        mesh(ps_s(4:80,:)');xlabel('Frequency(Hz)/4');ylabel('Time(1 unit=0.004)');zlabel('Power (dB)');title('S-Transform of EEG Sample');GRID ON;axis([1 250 1 19 0 60]);axis tight;
        saveas(gcf,[num2str(start) '_st.jpg']);
        close(gcf);
%
%       compute RA scaled by a factor of 4
%
        ra = sum(pyy(24:56))/sum(pyy(8:120));

        start = start + 1;

        raVar = [raVar ra];
    end
    end_val = start;
end 