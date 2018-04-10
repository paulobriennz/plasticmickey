#include <Arduino.h>
#include <Wire.h> 

class I2CController
{   
    int SDA;
    int SCL;      

public:

    I2CController(int sda, int scl)
    {
        SDA = sda;
        SCL = scl;    
        Wire.begin(SDA, SCL);    
    }    

    bool TestI2CAddress(byte address)
    {
        byte error;
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            return true;
        } 
        else if (error == 4) 
        {
           return false;
        }
        return false;
    }

    void ScanAddressRange()
    {
        int nDevices = 0;
        byte address;

        for (address = 1; address < 127; address++)  
        {
          bool testResult = TestI2CAddress(address);

          if (testResult)
          {
            nDevices++;
            //log address : found
          }
          else
          {
            //log no address
          }
      }

      if (nDevices == 0)
      {
        //log no address
      }

    }

    void ScanPortArray()
    {
        //uint8_t portArray[] = {16, 5, 4, 0, 2, 14, 12, 13};
        uint8_t portArray[] = {5, 4};

        //String portMap[] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7" }; //for Wemos
        String portMap[] = { "GPIO5", "GPIO4" };

        for (uint8_t i = 0; i < sizeof(portArray); i++) //SDA
        {
            for (uint8_t j = 0; j < sizeof(portArray); j++) //SCL
            {
                if (i != j)
                {
                    Serial.print("Scanning (SDA : SCL) - " + portMap[i] + " : " + portMap[j] + " - ");
                    Wire.begin(portArray[i], portArray[j]);
                    ScanAddressRange();
                }
            }
        }
    }



/* void check_if_exist_I2C() 
{
  byte error, address;
  int nDevices;
  nDevices = 0;

  for (address = 1; address < 127; address++ )  
  {
    // The i2c_scanner uses the return value of the Write.endTransmisstion to see if a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } 
    else if (error == 4) 
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }

  } //for loop

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("**********************************\n");
    
  //delay(1000);           // wait 1 seconds for next scan, did not find it necessary
}




//I2C Scanner Main Function
void scanPorts() { 
  for (uint8_t i = 0; i < sizeof(portArray); i++) {
    for (uint8_t j = 0; j < sizeof(portArray); j++) {
      if (i != j){
        Serial.print("Scanning (SDA : SCL) - " + portMap[i] + " : " + portMap[j] + " - ");
        Wire.begin(portArray[i], portArray[j]);
        check_if_exist_I2C();
      }
    }
  }
} */

};