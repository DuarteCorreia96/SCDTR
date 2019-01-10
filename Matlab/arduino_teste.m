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

lux_ref = gain*40;
Vcc = 5;
C = 1e-6;
m = -0.72;
b = 5.084;
R1 = 10000;

npoints =  120;
voltage =  zeros(npoints * 20,1);
time =     zeros(npoints * 20, 1);
tic

fprintf(s, "s 0");
fprintf(f, "s 0");
pause(2)
for i = 1:npoints

 voltage(i) = readVoltage(a, 'A0');
 time(i)    = toc;
end

fprintf(s, "s 40");
for i = npoints : npoints * 4

 voltage(i) = readVoltage(a, 'A0');
 time(i)    = toc;
end

fprintf(f, "s 255");
disp("light it");
for i = npoints * 4 : npoints * 8 

 voltage(i) = readVoltage(a, 'A0');
 time(i)    = toc;
end

fprintf(f, "s 0");
disp("turn it off");
for i = npoints * 8 : npoints * 20 

 voltage(i) = readVoltage(a, 'A0');
 time(i)    = toc;
end


fprintf(f, "s 0");
fprintf(s, "s 0");
fclose(s);
fclose(f);

toc

R2 = R1 * (Vcc - voltage) ./ voltage;
lux = 10.^((log10(R2)-b)/m); 

figure(1)
plot(time, lux)
hold on

ref_values = ones(length(time), 1) * lux_ref;
ref_values(1:find(voltage > 0, 1)-1) = 0; 
plot(time, ref_values)
hold on

clear a s f
delete(instrfindall);

