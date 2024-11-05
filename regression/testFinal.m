CpX = [lizCPX;masseCPX;vothCPX];
>> CpY = [lizY;masseY;vothY];
>> [b,dev,statsg] = glmfit(CpX, CpY);
>> temp = glmval(b,rvCPX,'identity');
>> [successGLM1]= calcSuccess(temp', rvY, .01, .99);