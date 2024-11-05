function testRun()
close all;
clear all;
pack;
%--------------------------------
load liz4;
%--------------------------------
[success1, lizARX, lizY]=testARModel(liz4,false, 3,4,.01,.99);
[success2, lizPLX, lizY]=testPoles(liz4,false, 3,4,.01,.99);
[success4, lizCPX, lizY]=testCeps(liz4,false, 3,4,.01,.99);
lizResult = [success1;success2;success4] 
%--------------------------------------------
clear success1 success2 success4 liz4
pack
%--------------------------------------------
load voth3;
%--------------------------------------------
[success1, vothARX, vothY]=testARModel(voth3,false, 2,1,.01,.99);
[success2, vothPLX, vothY]=testPoles(voth3,false, 2,1,.01,.99);
[success4, vothCPX, vothY,]=testCeps(voth3,false, 2,1,.01,.99);
vothResult = [success1;success2;success4] 
%--------------------------------------------
clear success1 success2 success4 voth3
pack
%--------------------------------------------
load masse2;
%--------------------------------
[success1, masseARX, masseY]=testARModel(masse2,false, 2,1,.01,.99);
[success2, massePLX, masseY]=testPoles(masse2,false, 2,1,.01,.99);
[success4, masseCPX, masseY,]=testCeps(masse2,false, 2,1,.01,.99);
masseResult = [success1;success2;success4] 
%--------------------------------------------
clear success1 success2 success4 masse2
pack
%--------------------------------------------
load rv1;
%--------------------------------
[success1, rvARX, rvY]=testARModel(rv1,false, 5,7,.01,.99);
[success2, rvPLX, rvY]=testPoles(rv1,false, 5,7,.01,.99);
[success4, rvCPX, rvY,]=testCeps(rv1,false, 5,7,.01,.99);
rvResult = [success1;success2;success4] 
%--------------------------------------------
clear success1 success2 success4 rv1
pack
%--------------------------------------------
%load n1;
%--------------------------------
%[success1, n1ARX, n1Y]=testARModel(n1,false, 3,4,.01,.99);
%[success2, n1PLX, n1Y]=testPoles(n1,false, 3,4,.01,.99);
%[success4, n1CPX, n1Y,]=testCeps(n1,false, 3,4,.01,.99);
%n1Result = [success1;success2;success4] 
%--------------------------
%clear success1 success2 success4 n1
%pack
%--------------------------------------------
%load n2;
%--------------------------------
%[success1, n2ARX, n2Y]=testARModel(n2,false, 3,4,.01,.99);
%[success2, n2PLX, n2Y]=testPoles(n2,false, 3,4,.01,.99);
%[success4, n2CPX, n2Y,]=testCeps(n2,false, 3,4,.01,.99);
%n2Result = [success1;success2;success4] 
%--------------------------------------------
%clear success1 success2 success4 n2
%pack
%--------------------------------------------
%load n3;
%--------------------------------
%[success1, n3ARX, n3Y]=testARModel(n3,false, 3,4,.01,.99);
%[success2, n3PLX, n3Y]=testPoles(n3,false, 3,4,.01,.99);
%[success4, n3CPX, n3Y,]=testCeps(n3,false, 3,4,.01,.99);
%n3Result = [success1;success2;success4] 
%--------------------------------------------
%clear success1 success2 success4 n3
%pack
%--------------------------------------------
%load n4;
%--------------------------------
%[success1, n4ARX, n4Y]=testARModel(n4,false, 3,4,.01,.99);
%[success2, n4PLX, n4Y]=testPoles(n4,false, 3,4,.01,.99);
%[success4, n4CPX, n4Y,]=testCeps(n4,false, 3,4,.01,.99);
%n4Result = [success1;success2;success4]; 
%--------------------------------------------
%clear success1 success2 success4 n4
%pack
%--------------------------------------------
%load n5;
%--------------------------------
%[success1, n5ARX, n5Y]=testARModel(n5,false, 3,4,.01,.99);
%[success2, n5PLX, n5Y]=testPoles(n5,false, 3,4,.01,.99);
%[success4, n5CPX, n5Y,]=testCeps(n5,false, 3,4,.01,.99);
%n5Result = [success1;success2;success4] 

%--------------------------------------------
%clear success1 success2 success4 n5
%pack
%--------------------------------------------
%avgNormal04 = (n1Result + n2Result + n3Result + n4Result + n5Result) ./5
%clear n1Result n2Result n3Result n4Result n5Result
%--------------------------------------------
avgPatient04 = (lizResult + vothResult + masseResult + rvResult)./4
clear lizResult vothResult masseResult rvResult
%--------------------------------------------
%avgDiff04 = (avgPatient04 - avgNormal04)
%save testRun04;