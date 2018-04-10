/////////////////////////////////////////////////////////////////////////////////////////////////////

// Plastic Mickey - a robot controller solution for ESP8266
// Cobbled together by Paul O'Brien - April 2018
// http://paulobrien.co.nz/2018/03/27/plastic-micky-my-iot-robot-to-entertain-the-toddler/
// https://github.com/paulobriennz/plasticmicky

/////////////////////////////////////////////////////////////////////////////////////////////////////

// Hardware List
// Node MCU Hardware
// L298 Motor Controller
// PCA9685 16 Channel PWM Controller
// PCF8574 8 Channel GPIO Extender
// LCD1602 LCD Screen
// I2C Module for 1602 LCD Screen
// 2 x 12v DC Motors for Tracks
// 3 x Servo for Head, Left and Right Arms
// 8 Channel Relay Board

// libraries required:
//      Wifi Manager
//      LiquidCrystal_I2c
//      Adafruit_PWMServoDriver
//      Blynk
//      mDNS

//Includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>
#include <DNSServer.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_PWMServoDriver.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>

//Plastic Mickey Classes
#include "servoControl.cpp"
#include "lcdController.cpp"
#include "motorController.cpp"
#include "webserverController.cpp"
#include "i2cController.cpp"

//Blynk Token
char auth[] = "........................................";
#define BLYNK_PRINT Serial //Blynk Debug

//Servo Controller
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); 

//i2c bus pins
#define SDA   5     //i2c SDA GPIO  (D1)
#define SCL   4     //i2c SCL GPIO  (D2)

//i2c device addresses
#define LCD_I2CADDRESS   0x27
#define SERVOCONTROLLER_I2CADDRESS    0x40
#define RELAYCONTROLLER_I2CADDRESS   0x70

//servo contsants
#define SERVOMIN  200 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  550 // this is the 'maximum' pulse length count (out of 4096)
#define SERVOCENTRE  375 // half way between

//web server constants
#define WEBSERVERPORT   80

//gpio's on MCU to drive motor controller
//#define MOTORLEFT1   14
//#define MOTORLEFT2   12
//#define MOTORRIGHT1   13
//#define MOTORRIGHT2   15

//pwm channels to drive motor controller
#define MOTORLEFT1   10
#define MOTORLEFT2   11
#define MOTORRIGHT1   12
#define MOTORRIGHT2   13

//motor controller PWM channels
#define MOTORLEFT_CHANNEL  14     // channel on the 16 Channel PWM Module to control speed on IN1/2 
#define MOTORRIGHT_CHANNEL  15     // channel on the 16 Channel PWM Module to control speed on IN3/4 

//servo channels on PWM controller
#define HEAD_CHANNEL  0     // channel on the 16 Channel PWM Module to control speed on IN3/4 
#define LEFTARM_CHANNEL  1     // channel on the 16 Channel PWM Module to control speed on IN3/4 
#define RIGHTARM_CHANNEL  2     // channel on the 16 Channel PWM Module to control speed on IN3/4 

//set intial PWM Speed for Motor controller
#define INITIALSPEED  4000   //between 0 and 4095

//Initialise Tracks
//set the end parameter to false to use PWM Pins 10, 11, 12, 13 on PWM Controller to drive IN1, IN2, IN3, IN4 on Motor Controller
Motors tracks(MOTORLEFT1, MOTORLEFT2, MOTORRIGHT1, MOTORRIGHT2, MOTORLEFT_CHANNEL, MOTORRIGHT_CHANNEL, false);

//set the end parameter to true to use GPIO Pins 14, 12, 13, 15 (D5, D6, D7, D8 on NodeMCU) to drive IN1, IN2, IN3, IN4 on Motor Controller
//Motors tracks(MOTORLEFT1, MOTORLEFT2, MOTORRIGHT1, MOTORRIGHT2, MOTORLEFT_CHANNEL, MOTORRIGHT_CHANNEL, true);

//Initialise Servo's
Servo head(HEAD_CHANNEL);
Servo leftArm(LEFTARM_CHANNEL);
Servo rightArm(RIGHTARM_CHANNEL);

//Initialise LCD
LcdDisplay lcd(SDA, SCL);

//Initialise Web Server
WWWServer www(WEBSERVERPORT);

//Initialise I2C Controller
I2CController i2c(SDA, SCL); //start tthe i2c controller bus on pins 5 and 4  

//Hardware Check States
bool servoControllerHardwarePresent;
bool lcdHardwarePresent;
bool i2cRelayHardwarePresent;
bool resetWifiLeftButton;
bool resetWifiRightButton;
bool resetWifiArmButton;
bool wifiInitiated;

//Robot Action Methods
void SetMotorSpeed(String sp)
{
    tracks.SetSpeed(pwm, sp.toInt());
    lcd.Display("Motor Speed Set", sp);
}

void lookLeft()
{
    lcd.Display("Look Left","");
    head.Move(pwm, SERVOMAX, 0);
}

void lookRight()
{
    lcd.Display("Look Right","");
    head.Move(pwm, SERVOMIN, 0);
}

void lookCentre()
{
    lcd.SmileyFace();
    head.Move(pwm, SERVOCENTRE, 0);
}

void leftArmUp()
{
    lcd.Display("Left Arm Up","");
     leftArm.Move(pwm, SERVOMAX, 0);
}

void leftArmDown()
{
    lcd.Display("Left Arm Down","");
    leftArm.Move(pwm, SERVOMIN, 0);
}

void leftArmCentre()
{
    lcd.SmileyFace();
    leftArm.Move(pwm, SERVOCENTRE, 0);
}

void rightArmUp()
{
    lcd.Display("Right Arm Up","");
    rightArm.Move(pwm, SERVOMAX, 0);
}

void rightArmDown()
{
    lcd.Display("Right Arm Down","");
    rightArm.Move(pwm, SERVOMIN, 0);
}

void rightArmCentre()
{
    lcd.SmileyFace();
    rightArm.Move(pwm, SERVOCENTRE, 0);
}

void halt()
{
    tracks.Halt(pwm);
    lcd.SmileyFace();
}

void forward()
{
    tracks.GoForward(pwm);
    lcd.Display("Go Forward","");
}

void reverse()
{
    tracks.GoBackwards(pwm);
    lcd.Display("Go Backwards","");
}

void left()
{
    tracks.TurnLeft(pwm);
    lcd.Display("Turn Left","");
}

void right()
{
    tracks.TurnRight(pwm);
    lcd.Display("Turn Right","");
}

void webLookLeft(int msec) 
{
    String reply = "";
    String msString = String(msec);

    if (msec > 0)
    {
        leftArm.Move(pwm, 500, msec);
        reply = "Turn Left " + msString;
    }
    else
    {
        reply = "Turn Left";
        leftArm.Move(pwm, 500, 0);
    }

    lcd.Display(reply,"");
    www.server.send(200, "text/plain", reply);
}

void webLookRight() 
{
    lookRight();
    www.server.send(200, "text/plain", "look right");
}


//wifi manager config mode callback
void configModeCallback (WiFiManager *myWiFiManager) 
{
    lcd.Display("WiFi Config Mode", myWiFiManager->getConfigPortalSSID() + " " + WiFi.softAPIP());
    Serial.println("WiFi Config Mode");
    Serial.println(myWiFiManager->getConfigPortalSSID() + " " + WiFi.softAPIP());
}


void setup()
{
    Serial.begin(115200);    

    delay(2000); //delay 2 seconds to view LCD Initialize message

    //scan i2c bus for required hardware and report 
    //TODO


    //initialize wifi
    lcd.Display("Connect WiFi..","");
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);
    wifiManager.setAPCallback(configModeCallback); //set callback
    
    if(!wifiManager.autoConnect()) 
    {
        //failed to connect
        Serial.println("failed to connect and hit timeout");
        lcd.Display("Wifi Failed", "Failed to connect");

        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(1500);
    } 
    else
    {  
        //wifi connected
        wifiInitiated = true;
        lcd.Display("Wifi Connected", "IP:" + WiFi.localIP().toString());
        Serial.println("Wifi Connected - IP:" + WiFi.localIP().toString());
        delay(2000);

        //initialize mDNS
        if (MDNS.begin("mickey")) {
            Serial.println("MDNS responder started");
            lcd.Display("mDNS Started","mickey.local");
            delay(2000);
        }

        //initialize PWM Service Controller
        pwm.begin();
        pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

        //initialize Blynk
        lcd.Display("Connect Blynk..","");
        Blynk.config(auth);

        //delay(5000);

        if (Blynk.CONNECTED)
        {
            lcd.Display("Blynk Connected", "Starting Server");
            delay(2000);
        }

        //set initial PWM speed for Motor controller
        tracks.SetSpeed(pwm, INITIALSPEED);        

        //set up web methods
        www.server.on("/setspeed",  []()
        {
            String speed = www.server.arg("speed");

            if (speed != NULL)
            {
                SetMotorSpeed(speed);
                www.server.send(200, "text/plain", "Motor Speed Set to " + speed);
            }   
            else
            {
                www.server.send(200, "text/plain", "No Speed specified");
            }    
        }); 

        www.server.on("/goleft",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL && duration.toInt() > 0)
            {    
                tracks.durationTime = duration.toInt();
                reply = "Turn Left for " + duration + " ms";
            }   
            else
            {
                reply = "Turn Left";                            
            }   

            tracks.TurnLeft(pwm);
            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/goright",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL && duration.toInt() > 0)
            {    
                tracks.durationTime = duration.toInt();
                reply = "Turn Right for " + duration + " ms";
            }   
            else
            {
                reply = "Turn Right";                            
            }   

            tracks.TurnRight(pwm);
            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/goforwards",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL && duration.toInt() > 0)
            {    
                tracks.durationTime = duration.toInt();
                reply = "Go Forwards for " + duration + " ms";
            }   
            else
            {
                reply = "Go Forwards";                            
            }   

            tracks.GoForward(pwm);
            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/gobackwards",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL && duration.toInt() > 0)
            {    
                tracks.durationTime = duration.toInt();
                reply = "Go Backwards for " + duration + " ms";
            }   
            else
            {
                reply = "Go Backwards";                            
            }   

            tracks.GoBackwards(pwm);
            www.server.send(200, "text/plain", reply);  
        });

        www.server.on("/lookright",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Look Right for " + duration + " ms";
                    head.Move(pwm, SERVOMIN, duration.toInt());
                }
            }   
            else
            {
                reply = "Look Right";     
                head.Move(pwm, SERVOMIN, 0);                 
            } 

            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/lookleft",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Look Left for " + duration + " ms";
                    head.Move(pwm, SERVOMAX, duration.toInt());
                }
            }   
            else
            {
                reply = "Look Left";     
                head.Move(pwm, 500, SERVOMAX);                 
            } 
            
            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/lookcentre",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Look Centre for " + duration + " ms";
                    head.Move(pwm, SERVOCENTRE, duration.toInt());
                }
            }   
            else
            {
                reply = "Look Centre";     
                head.Move(pwm, 500, SERVOCENTRE);                 
            } 
            
            www.server.send(200, "text/plain", reply);  
        }); 
       

        www.server.on("/leftarmup",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Left Arm Up for " + duration + " ms";
                    leftArm.Move(pwm, SERVOMAX, duration.toInt());
                }
            }   
            else
            {
                reply = "Left Arm Up";     
                leftArm.Move(pwm, SERVOMAX, 0);                 
            } 

            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/leftarmdown",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Left Arm Down for " + duration + " ms";
                    leftArm.Move(pwm, SERVOMIN, duration.toInt());
                }
            }   
            else
            {
                reply = "Left Arm Down";     
                leftArm.Move(pwm, SERVOMIN, 0);                 
            } 
            
            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/leftarmcentre",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Left Arm Centre for " + duration + " ms";
                    leftArm.Move(pwm, SERVOCENTRE, duration.toInt());
                }
            }   
            else
            {
                reply = "Left Arm Centre";     
                leftArm.Move(pwm, SERVOCENTRE, 0);                
            } 
            
            www.server.send(200, "text/plain", reply);  
        }); 


        www.server.on("/rightarmup",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Right Arm Up for " + duration + " ms";
                    rightArm.Move(pwm, SERVOMAX, duration.toInt());
                }
            }   
            else
            {
                reply = "Right Arm Up";     
                rightArm.Move(pwm, SERVOMAX, 0);                 
            } 

            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/rightarmdown",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Right Arm Down for " + duration + " ms";
                    rightArm.Move(pwm, SERVOMIN, duration.toInt());
                }
            }   
            else
            {
                reply = "Right Arm Down";     
                rightArm.Move(pwm, SERVOMIN, 0);                 
            } 
            
            www.server.send(200, "text/plain", reply);  
        }); 

        www.server.on("/rightarmcentre",  []()
        {
            String reply;
            String duration = www.server.arg("duration");

            if (duration != NULL)
            {
                if (duration.toInt() > 0)
                {   
                    reply = "Right Arm Centre for " + duration + " ms";
                    rightArm.Move(pwm, SERVOCENTRE, duration.toInt());
                }
            }   
            else
            {
                reply = "Right Arm Centre";     
                rightArm.Move(pwm, SERVOCENTRE, 0);                
            } 
            
            www.server.send(200, "text/plain", reply);  
        }); 


        //start the web server
        www.Initialize();

        //say cheese
        lcd.Display("Mickey is Online", "----------------");
        delay(2000);
        lcd.SmileyFace();
    }
   
}

void loop()
{
    if (wifiInitiated)
    {
        //handle blynk
        Blynk.run();

        //handle the web server
        www.server.handleClient();

        //handle any async updates to servos or motors
        leftArm.Update(pwm);
        rightArm.Update(pwm);
        head.Update(pwm);
        tracks.Update(pwm);

        //cancel any pending servo/motor state changes
        if (leftArm.operationFinshed > 0)
        {
            lcd.SmileyFace();
            leftArm.operationFinshed = 0;
        }

        if (rightArm.operationFinshed > 0)
        {
            lcd.SmileyFace();
            rightArm.operationFinshed = 0;
        }

        if (head.operationFinshed > 0)
        {
            lcd.SmileyFace();
            head.operationFinshed = 0;
        }

        if (tracks.operationFinshed > 0)
        {
            lcd.SmileyFace();
            tracks.operationFinshed = 0;
        }


        //if left and right are held down and right arm up in blynk app - reset wifimanager
        if (resetWifiLeftButton && resetWifiRightButton  && resetWifiArmButton)
        {
            WiFiManager wm;
            wm.resetSettings();
            lcd.Display("Resetting WiFi..","");
            delay(3000);
            ESP.reset();
        }    

    }
}


//Blynk Connections
BLYNK_WRITE(V0) //forward
{
    if (param[0])
    forward();
    else
    halt();
}

BLYNK_WRITE(V1) //reverse
{
    if (param[0])
        reverse();
    else
        halt();
}

BLYNK_WRITE(V2) //left
{
    if (param[0])
    {
        resetWifiLeftButton = true;
        left();
    }    
    else
    {
        resetWifiLeftButton = false;
        halt();             
    }    
}

BLYNK_WRITE(V3) //right
{
    if (param[0])
    {
        resetWifiRightButton = true;
        right();
    }    
    else
    {
        resetWifiRightButton = false;
        halt();             
    }    
}

BLYNK_WRITE(V4) //look left
{
    if (param[0])
        lookLeft();
    else
        lookCentre();
}

BLYNK_WRITE(V5)
{
    if (param[0]) //look right
        lookRight();
    else
        lookCentre();
}

BLYNK_WRITE(V6) //left arm up
{
    if (param[0])
    leftArmUp();
    else
    leftArmCentre();
}

BLYNK_WRITE(V7) //left arm down
{
    if (param[0]) 
    leftArmDown();
    else
    leftArmCentre();
}

BLYNK_WRITE(V8) //right arm up
{
  if (param[0])
    {
        resetWifiArmButton = true;
        rightArmUp();
    }    
    else
    {
        resetWifiArmButton = false;
        rightArmCentre();             
    }   
}

BLYNK_WRITE(V9) //right arm down
{
    if (param[0]) 
    rightArmDown();
    else
    rightArmCentre();
}

BLYNK_WRITE(V10) //nose light
{
    if (param[0])
    {

    }
}
BLYNK_WRITE(V11) //neck lights
{
    if (param[0])
   {

   }
}

BLYNK_WRITE(V12) //speed
{
    if (param[0])
    {
        SetMotorSpeed(param[0].asString());
    }
}