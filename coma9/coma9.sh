../eegproc debug Trend /media/My_Disc/Coma9.28June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,60,false);exit";mv validation.mat validation1.mat;mv sample.jpg sample1.jpg;
../eegproc debug Trend /media/My_Disc/Coma9.29June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,60,true);exit";


#../eegproc release Dump /media/Lizmore50875/Lizmore50875A.txt 1 1 21 11 21
#../eegproc release Dump /media/Lizmore50875/Lizmore50875B.txt 1 1 21 11 21

#../eegproc release Split ./Dump.txt 02/16/2006 17 0 1 2 3 4 5
