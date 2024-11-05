#../eegproc debug Trend /media/Voth50764AtoE/Voth50764A.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation1.mat;mv sample.jpg sample1.jpg;
#../eegproc debug Trend /media/Voth50764AtoE/Voth50764B.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation2.mat;mv sample.jpg sample2.jpg;
#../eegproc debug Trend /media/Voth50764AtoE/Voth50764C.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation3.mat;mv sample.jpg sample3.jpg;
#../eegproc debug Trend /media/Voth50764AtoE/Voth50764D.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation4.mat;mv sample.jpg sample4.jpg;
#../eegproc debug Trend /media/Voth50764AtoE/Voth50764E.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation5.mat;mv sample.jpg sample5.jpg;

../eegproc debug Trend /media/Voth50764FtoI/Voth50764F.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation6.mat;mv sample.jpg sample6.jpg;
../eegproc debug Trend /media/Voth50764FtoI/Voth50764G.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation7.mat;mv sample.jpg sample7.jpg;
../eegproc debug Trend /media/Voth50764FtoI/Voth50764H.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,30,false);exit";mv validation.mat validation8.mat;mv sample.jpg sample8.jpg;
../eegproc debug Trend /media/Voth50764FtoI/Voth50764I.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,30,true);exit";

#../eegproc release Dump /media/Voth50764AtoE/Voth50764B.txt 1 1 21 11 21
#../eegproc release Dump /media/Voth50764AtoE/Voth50764C.txt 1 1 21 11 21
#../eegproc release Dump /media/Voth50764AtoE/Voth50764D.txt 1 1 21 11 21
#../eegproc release Dump /media/Voth50764AtoE/Voth50764E.txt 1 1 21 11 21

#../eegproc release Split ./Dump.txt 01/30/2006 17 0 1 2 3 4 5 6 7