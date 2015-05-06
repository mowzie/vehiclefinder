clear all
close all
% A = half the distance between mics
A = 1/2;
% c = speed of sound 340.29m/s
c = 340;
% Tn = time of when mic(n) recives sound
t1 = 1.897;
t2 = 1.899;
t3 = 1.900;
t4 = 1.899;
% T31 is time difference between mic 3 and mic 1
t31 = t3-t1
% T42is time difference between mic 4 and mic 2
t42 = t4-t2

a1 = ((t3-t1)*c)/2
b1 = sqrt(A^2 - (((t3-t1)^2 *(c)^2)/4))
a2 = ((t4-t2)*c)/2
b2 = sqrt(A^2 - (((t4-t2)^2 *(c)^2)/4))

% z = x^2 , n = y^2
z = (((b1)^2 + (a2)^2)*((a1^2 * b2^2)))/((b1^2 * b2^2)-(a1^2 * a2^2));
n = (((b2)^2 + (a1)^2)*(a2^2 * b1^2))/((b1^2 * b2^2)-(a1^2 * a2^2));
% x = point on the x axis
% y = point on the y axis
x = sqrt(z)
y = sqrt(n)

%If the time difference between 3&1 is less than 0 make the x-axis value
%negative else make it postive.
if t31 < 0 
    x = -x
else
    x = x
end
%If the time difference between 4&2 is less than 0 make the y-axis value
%negative else make it postive.
if t42 < 0 
    y = -y
else
    y = y
end

%conversion from x & y axis values to an angle
P = atan2(y,x);
Angle = (P * 57.2957795)+360;

%make it so that when the angle is equal to 360 degrees to make it 0
%degrees. 
if Angle == 360
    Angle = 0
end

