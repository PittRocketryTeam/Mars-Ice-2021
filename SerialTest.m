clear all;
close all;
clc;

%define the integers for each state here
state1 = '1';

%sets up communication with arduino
device = serialport("COM7",115200);
configureTerminator(device,255); %this is necessary to send signals in proper format 

while(1)
    writeline(device,state1); %this is the command that tells Arduino which state to execute
    pause(5);
end
