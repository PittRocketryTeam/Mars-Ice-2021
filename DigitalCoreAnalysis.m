close all;
clc;
inputArray = [1 2 3 4; 1 2 3 4; 5 4 3 2;1 2 3 4; 1 2 3 4; 5 4 3 2];

i = 1; %column index
j=1; %averaging row index
l=1; %output row index
k = size(DigitalCoreSample,1); %figure this out

while(i<5)
        j=1;
        l=1;
        while(j<(k-1))
            outputArray(l,i) = (DigitalCoreSample(j,i)+DigitalCoreSample(j+1,i)+DigitalCoreSample(j+2,i))/3;
            j=j+3;
            l=l+1;
        end
    i=i+1;
end

figure;
plot(DigitalCoreSample(:,2),DigitalCoreSample(:,3));
title('Force versus Distance');
xlabel('Steps Taken');
ylabel('Force (N)');

figure;
plot(DigitalCoreSample(:,2),DigitalCoreSample(:,4));
title('Current versus Distance');
xlabel('Steps Taken');
ylabel('Current (A)');

figure;
plot(DigitalCoreSample(:,2),DigitalCoreSample(:,3));
title('Averaged Force versus Distance');
xlabel('Steps Taken');
ylabel('Force (N)');

figure;
plot(DigitalCoreSample(:,2),DigitalCoreSample(:,4));
title('Averaged Current versus Distance');
xlabel('Steps Taken');
ylabel('Current (A)');