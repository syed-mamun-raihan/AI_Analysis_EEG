function [end_val,raVar] = crapFFT2(data,start)

    sampling_rate = 250;

    fft_length = 256;

    decimate = 4;

    t=1/sampling_rate:1/sampling_rate:1;

    f = sampling_rate *(1:sampling_rate)/fft_length;

    [row,col] = size(data);

    no_of_seconds = row / sampling_rate;

    raVar = [];

    for i=0:1:no_of_seconds-1

        var_prefix = num2str(start); 

        temp = data(i*sampling_rate+1:(i+1)*sampling_rate);

        temp = temp - mean(temp);

        ffty = fft(temp,fft_length);

        ffty((fft_length/decimate):fft_length) = 0;

        temp = real(ifft(ffty));

        temp = temp - mean(temp);

        plot(t,temp(1:250));ylabel('Amplitude (µV)');xlabel('Time');title('EEG Sample');GRID ON;axis([0 1 -40 40]);axis tight;

        saveas(gcf,[var_prefix '_orig.jpg']);

        close(gcf);

        Y = ifft(ffty(1:(fft_length/decimate)));

        [a,b,c]=st(Y);

        ps = 10*log10(a .* conj(a));

        sum_ps = sum(ps(:,17:48))./32;
%
%       now plot Frequency Spectrum
%
        plot(f(1:32), sum_ps');xlabel('Frequency(Hz)');ylabel('Power (dB)');title('Power Spectrum');GRID ON;axis tight;

        saveas(gcf,[var_prefix '_fft.jpg']);

        close(gcf);
%
%       plot ST        
%
        mesh(ps(1:32,:)');xlabel('Frequency(Hz)');ylabel('Time(1 unit=0.015)');zlabel('Power (dB)');title('S-Transform of EEG Sample');GRID ON;axis tight;

        saveas(gcf,[var_prefix '_st.jpg']);

        close(gcf);
%
%       compute RA
%
        ra = sum(sum_ps(6:14))/sum(sum_ps(1:20));

        start = start + 1;

        raVar = [raVar ra];
    end
    end_val = start;
end 