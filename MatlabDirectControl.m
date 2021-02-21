clear a

% Arduino Declaration
a = arduino('COM7', 'Mega2560');
StepPerRevolution = 800;
i=0;

% Pin Configuration
configurePin(a,'D2','DigitalOutput'); %D2 = Step pin
configurePin(a,'D3','DigitalOutput'); %D3 = Direction pin

while true
    writeDigitalPin(a,'D3',1);
    for i = 0:StepPerRevolution
        writeDigitalPin(a,'D2',1);
        writeDigitalPin(a,'D2',0);
        pause(0.0006);
    end
    writeDigitalPin(a,'D3',0);
    for i = 0:StepPerRevolution
        writeDigitalPin(a,'D2',1);
        pause(0.0002);
        writeDigitalPin(a,'D2',0);
        pause(0.0002);
    end
end
