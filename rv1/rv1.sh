##../eegproc release Trend /media/01RV49617B_txt/RV49617B.txt 1 1 21 11 21
##../eegproc release Trend /media/02RV49617B_txt/02RV49617B.txt 1 1 21 11 21
##../eegproc release Dump /media/02RV49617B_txt/02RV49617B.txt 1 1 21 11 21

##../eegproc debug Trend /media/RV49617C/RV49617C.txt 1 1 21 11 21
##../eegproc release Dump /media/RV49617C/RV49617C.txt 1 1 21 11 21

#../eegproc debug Trend /media/RV49617D/RV49617D.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(30,10,false);exit";mv validation.mat validation1.mat;mv sample.jpg sample1.jpg;
#../eegproc release Dump /media/RV49617D/RV49617D.txt 1 1 21 11 21

#../eegproc debug Trend /media/01RV49617F/01RV49617F.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(30,10,false);exit";mv validation.mat validation2.mat;mv sample.jpg sample2.jpg;
#../eegproc release Dump /media/01RV49617F/01RV49617F.txt 1 1 21 11 21

#../eegproc debug Trend /media/02RV49617F/02RV49617F.txt 1 1 21 11 21
#matlab -nosplash -nojvm -r "trendGen(30,10,false);exit";mv validation.mat validation3.mat;mv sample.jpg sample3.jpg;
#../eegproc release Dump /media/02RV49617F/02RV49617F.txt 1 1 21 11 21

../eegproc debug Trend /media/RV49617G/RV49617G.txt 1 1 21 11 21
matlab -nosplash -nojvm -r "trendGen(30,10,true);exit";
#../eegproc release Dump /media/RV49617G/RV49617G.txt 1 1 21 11 21

#../eegproc release Split /home/shaikat/public_html/Dump.txt 09/12/2005 12 0 1 2 3 4 5 6 7 8 9 10 11 12