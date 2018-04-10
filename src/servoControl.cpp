#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

class Servo
{
    int channelNumber;
    long durationTime;
    long centreAngle;    
    unsigned long previousMillis;

public:

    int operationFinshed;

    Servo(int channel)
    {
        channelNumber = channel;
        centreAngle = 375; //this is the PWM value for 90 degrees
        previousMillis = 0;
    }

    void Move(Adafruit_PWMServoDriver pwm, long angle, long duration)
    {  
        if (duration > 0) durationTime = duration; 
        pwm.setPWM(channelNumber, 0, angle); //move to new angle        
    }

    void Update(Adafruit_PWMServoDriver pwm)
    {
        unsigned long currentMillis = millis();

        if (durationTime > 0)
        {
            if (currentMillis - previousMillis >= durationTime) //once we're done
            {                
                pwm.setPWM(channelNumber, 0, centreAngle); //return to centre position                
                durationTime = 0;
                operationFinshed = 1;
                previousMillis = currentMillis; 
            } 
        }
    }
};