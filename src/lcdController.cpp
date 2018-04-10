#include <LiquidCrystal_I2C.h>
#include <Wire.h> 

class LcdDisplay
{
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,20,4);

    //LCD Characters
    uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
    uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
    uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
    uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
    uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
    uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
    uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
    uint8_t retarrow[8] = {	0x1,0x1,0x5,0x9,0x1f,0x8,0x4};

public:

    LcdDisplay(int sda, int sdc)
    {     
        lcd.init(sda, sdc); //SDA, SDC pins
        lcd.backlight();      
        lcd.createChar(0, bell);
        lcd.createChar(1, note);
        lcd.createChar(2, clock);
        lcd.createChar(3, heart);
        lcd.createChar(4, duck);
        lcd.createChar(5, check);
        lcd.createChar(6, cross);
        lcd.createChar(7, retarrow);       
        lcd.home();
        lcd.clear();
        Display("Plastic Mickey", "Booting..");   
    }

    void Display(String line1, String line2)
    {   
        lcd.clear();
        lcd.home(); 
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);
    }

    void UpdateLine1(String line)
    {
        lcd.setCursor(0, 0);
        lcd.print(line);
    }

    void UpdateLine2(String line)
    {
        lcd.setCursor(0, 1);
        lcd.print(line);
    }

    void SmileyFace()
    {
        Display("|              | ", " ---________--- "); //todo - make a better smiley face :)
    }

    /*    lcd.home ();
   // Do a little animation by writing to the same location
   for ( int i = 0; i < 2; i++ )
   {
      for ( int j = 0; j < 16; j++ )
      {
         lcd.print (char(random(7)));
      }
      lcd.setCursor ( 1, 1 );
   }
   delay (200);
 */

};