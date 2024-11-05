../eegproc debug Trend /media/Masse950D-G/Masse950D.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,20,false);exit";mv validation.mat validation1.mat;mv sample.jpg sample1.jpg;
../eegproc debug Trend /media/Masse950D-G/Masse950E.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,20,false);exit";mv validation.mat validation2.mat;mv sample.jpg sample2.jpg;
../eegproc debug Trend /media/Masse950D-G/Masse950F.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,20,false);exit";mv validation.mat validation3.mat;mv sample.jpg sample3.jpg;
../eegproc debug Trend /media/Masse950D-G/Masse950G.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,20,true);exit";

#../eegproc release Dump /media/Masse950D-G/Masse950D.txt 1 1 21 11 21
#../eegproc release Dump /media/Masse950D-G/Masse950E.txt 1 1 21 11 21
#../eegproc release Dump /media/Masse950D-G/Masse950F.txt 1 1 21 11 21
#../eegproc release Dump /media/Masse950D-G/Masse950G.txt 1 1 21 11 21
#../eegproc release Split ./Dump.txt 11/03/2005 09 0 1 2 3 4 5 6 7