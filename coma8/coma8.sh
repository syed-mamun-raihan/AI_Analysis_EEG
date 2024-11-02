../eegproc debug Trend /media/Coma8/01Coma8.28June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,50,false);exit";mv validation.mat validation1.mat;mv sample.jpg sample1.jpg;
../eegproc debug Trend /media/Coma8/02Coma8.28June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,50,false);exit";mv validation.mat validation2.mat;mv sample.jpg sample2.jpg;
../eegproc debug Trend /media/Coma8/03Coma8.28June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,50,false);exit";mv validation.mat validation3.mat;mv sample.jpg sample3.jpg;
../eegproc debug Trend /media/Coma8/Coma8.29June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,50,false);exit";mv validation.mat validation4.mat;mv sample.jpg sample4.jpg;
../eegproc debug Trend /media/Coma8/Coma8.30June2006.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(10,50,true);exit";

#../eegproc release Dump /media/Lizmore50875/Lizmore50875A.txt 1 1 21 11 21
#../eegproc release Dump /media/Lizmore50875/Lizmore50875B.txt 1 1 21 11 21
#../eegproc release Dump /media/Lizmore50875/Lizmore50875C.txt 1 1 21 11 21
#../eegproc release Dump /media/Lizmore50875/Lizmore50875D.txt 1 1 21 11 21
#../eegproc release Dump /media/Lizmore50875/Lizmore50875G.txt 1 1 21 11 21
#../eegproc release Dump /media/Lizmore50875/Lizmore50875H.txt 1 1 21 11 21

#../eegproc release Split ./Dump.txt 02/16/2006 17 0 1 2 3 4 5
