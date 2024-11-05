file = input('Enter the file need to be processed: ')
fid = fopen(file,'r');
str = fgetl(fid);
while(~feof(fid))
    str = fgetl(fid)
end