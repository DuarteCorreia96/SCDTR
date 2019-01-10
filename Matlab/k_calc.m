clear
delete(instrfindall);
a = arduino('COM5','Uno');          % reading
s = serial('COM4','BaudRate',9600); % test
f = serial('COM3','BaudRate',9600); % interference
%close all

fopen(s);
fprintf(fscanf(s));
read = fscanf(s);   
fprintf(read);
gain = sscanf(read,'\tGain = %f');

fopen(f);
fprintf(fscanf(f)); 
fprintf(f, "s 0");
fprintf(s, "s 0");
pause(2)

% Start
tic 

k1 = zeros(2, length(1:256));
counter = 0;
for i = 1:length(k1)
    
    k1(1, i) = readVoltage(a, 'A0');
    k1(2, i) = readVoltage(a, 'A1');
    
    fprintf(s, "s %d", i);
    pause(2);
end
fprintf(s, "s 0");
pause(2);

k2 = zeros(2, length(k1));

for i = 1:length(k1)
  
    k2(1, i) = readVoltage(a, 'A1');
    k2(2, i) = readVoltage(a, 'A0');
    
    fprintf(f, "s %d", i);
    pause(2);
end
fprintf(f, "s 0");

plot(0:length(k1)-1, k1(1, :))
hold on
plot(0:length(k1)-1, k1(2, :))

toc
% Finish

fprintf(f, "s 0");
fprintf(s, "s 0");
fclose(s);
fclose(f);

