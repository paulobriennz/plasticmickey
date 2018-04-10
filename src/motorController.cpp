#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

class Motors
{
    int IN1Channel;
    int IN2Channel;
    int IN3Channel;
    int IN4Channel;
    int PWM1Channel;
    int PWM2Channel;

    int IN1Pin;
    int IN2Pin;
    int IN3Pin;
    int IN4Pin;

    unsigned long previousMillis;

    bool DigitalPinMode;

    int PWMLOW = 0;
    int PWMHIGH = 4096;

public:

    int operationFinshed;
    int durationTime;

    Motors(int in1pin, int in2pin, int in3pin, int in4pin, int pwm1, int pwm2, bool digitalPinMode)
    {        
        if (digitalPinMode)
        {
            IN1Pin = in1pin;
            IN2Pin = in2pin;
            IN3Pin = in3pin;
            IN4Pin = in4pin;
        }
        else
        {
            IN1Channel = in1pin;
            IN2Channel = in2pin;
            IN3Channel = in3pin;
            IN4Channel = in4pin;
        }
       
        PWM1Channel = pwm1;
        PWM2Channel = pwm2;

        DigitalPinMode = digitalPinMode;        
    }

    void SetSpeed(Adafruit_PWMServoDriver pwm, int speed)
    {
        pwm.setPWM(PWM1Channel, 0, speed);    
        pwm.setPWM(PWM2Channel, 0, speed); 
    }

    void Halt(Adafruit_PWMServoDriver pwm)
    {
        if (DigitalPinMode)
        {
            digitalWrite(14, LOW);
            digitalWrite(12, LOW);
            digitalWrite(13, LOW);
            digitalWrite(15, LOW);
        }
        else
        {
            pwm.setPin(IN1Channel, PWMLOW); 
            pwm.setPin(IN2Channel, PWMLOW); 
            pwm.setPin(IN3Channel, PWMLOW);
            pwm.setPin(IN4Channel, PWMLOW); 
        }
    }

    void GoForward(Adafruit_PWMServoDriver pwm)
    {      
        if (DigitalPinMode)
        {
            digitalWrite(14, LOW);
            digitalWrite(12, HIGH);
            digitalWrite(13, HIGH);
            digitalWrite(15, LOW);
        }
        else
        {
            pwm.setPin(IN1Channel, PWMLOW); 
            pwm.setPin(IN2Channel, PWMHIGH); 
            pwm.setPin(IN3Channel, PWMHIGH); 
            pwm.setPin(IN4Channel, PWMLOW); 
        }
    }

    void GoBackwards(Adafruit_PWMServoDriver pwm)
    {
        if (DigitalPinMode)
        {
            digitalWrite(14, HIGH);
            digitalWrite(12, LOW);
            digitalWrite(13, LOW);
            digitalWrite(15, HIGH);
        }
        else
        {
            pwm.setPin(IN1Channel, PWMHIGH); 
            pwm.setPin(IN2Channel, PWMLOW); 
            pwm.setPin(IN3Channel, PWMLOW); 
            pwm.setPin(IN4Channel, PWMHIGH); 
        }
    }

    void TurnLeft(Adafruit_PWMServoDriver pwm)
    {
        if (DigitalPinMode)
        {
            digitalWrite(14, HIGH);
            digitalWrite(12, LOW);
            digitalWrite(13, HIGH);
            digitalWrite(15, LOW);
        }
        else
        {
            pwm.setPin(IN1Channel, PWMHIGH); 
            pwm.setPin(IN2Channel, PWMLOW); 
            pwm.setPin(IN3Channel, PWMHIGH); 
            pwm.setPin(IN4Channel, PWMLOW); 
        }
    }

    void TurnRight(Adafruit_PWMServoDriver pwm)
    {
        if (DigitalPinMode)
        {
            digitalWrite(14, LOW);
            digitalWrite(12, HIGH);
            digitalWrite(13, LOW);
            digitalWrite(15, HIGH); 
        }
        else
        {
            pwm.setPin(IN1Channel, PWMLOW); 
            pwm.setPin(IN2Channel, PWMHIGH); 
            pwm.setPin(IN3Channel, PWMLOW); 
            pwm.setPin(IN4Channel, PWMHIGH); 
        }
    }

    void Update(Adafruit_PWMServoDriver pwm)
    {
        unsigned long currentMillis = millis();

        if (durationTime > 0)
        {
            if (currentMillis - previousMillis >= durationTime) //once we're done
            {   
                Halt(pwm);        
                durationTime = 0;
                operationFinshed = 1;
                previousMillis = currentMillis; 
            } 
        }
    }
};