function [xhat, yhat] = rceps(x)
%RCEPS Real cepstrum.
%   RCEPS(X) returns the real cepstrum of the real sequence X.
%
%   [XHAT, YHAT] = RCEPS(X) returns both the real cepstrum XHAT and 
%   YHAT which is a unique minimum-phase sequence that has the same real 
%   cepstrum as X.
%
%   EXAMPLE: Show that YHAT is a unique minimum-phase sequence that has the
%   same real cepstrum as X.
%   y = [4 1 5]; % Non-minimum phase sequence
%   [xhat,yhat] = rceps(y);
%   xhat2= rceps(yhat);
%   [xhat' xhat2']
%
%   See also CCEPS, ICCEPS, HILBERT, FFT.

%   Author(s): L. Shure, 6-9-88
%   Copyright 1988-2004 The MathWorks, Inc.
%   $Revision: 1.8.4.1 $  $Date: 2004/12/26 22:16:49 $

%   References: 
%     [1] A.V. Oppenheim and R.W. Schafer, Digital Signal 
%         Processing, Prentice-Hall, 1975.
%     [2] Programs for Digital Signal Processing, IEEE Press,
%         John Wiley & Sons, 1979, algorithm 7.2.

error(nargchk(1,1,nargin));

% Check for valid input signal
msg = chkinput(x);
error(msg)

n = length(x);
xhat = real(ifft(log(real(fft(x)))));
if nargout > 1
   odd = fix(rem(n,2));
   wn = [1; 2*ones((n+odd)/2-1,1) ; ones(1-rem(n,2),1); zeros((n+odd)/2-1,1)];
   yhat = zeros(size(x));
   yhat(:) = real(ifft(exp(fft(wn.*xhat(:)))));
end


%--------------------------------------------------------------------------
function msg = chkinput(x)
% Check for valid input signal

msg = '';

if isempty(x) || issparse(x) || ~isreal(x),
    msg.identifier = generatemsgid('invalidInput');
    msg.message = 'The input signal X must be real.';
    return;
end

