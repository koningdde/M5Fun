#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>
#include <NeoPixelBus.h>
#include <EEPROM.h>

#define SDA_PIN 21
#define SCL_PIN 25
int I2C_SLAVE_ADDR = 17; //18 For initial setup adres.

const uint16_t PixelCount = 25; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 27;  // make sure to set this to the correct pin, ignored for Esp8266
int x=0;

#define colorSaturation 128

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);
void changeAdres(int x);

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  if (I2C_SLAVE_ADDR == 17) 
  {
    WireSlave.print("A ");    //Request for new I2C Adres when adres was 17
  } 
  else 
    { 
      WireSlave.print("Hello, you "); //Do somthing else
      strip.SetPixelColor(x-1, hslBlue);
      strip.SetPixelColor(x-2, black);
      if(x == 1){strip.SetPixelColor(24, black);}
      x+=1;
      if(x>25){x=1;}
    }
}

void receiveEvent(int howMany)
{
  int x;
  char c;
    while (1 < WireSlave.available())   //Loop through all but the last byte
    {
        c = WireSlave.read();           //Receive byte as a character
        Serial.print(c);                //Print the character
            if (c == 'A') {             //Receive A > Change I2C adres
            x = WireSlave.read();       //Receive byte as an integer
            Serial.print(x);            //Read the next byte, this will be the addres send from the master.
            changeAdres(x);             //Change I2C Adres to received adres in EEPROM
            
            }
      else
      {
        //Do some nice stuff here
      }
        }
  
    x = WireSlave.read();       //Receive byte as an integer
    Serial.println(x);          //Print the integer
}

void changeAdres(int x) {
  Serial.println("CHANGING I2C ADDRES in eeprom");
  EEPROM.write(0, x);
  EEPROM.commit();
  ESP.restart();      //Reboot ESP, need to reset the I2C bus.
  }

  void setup() {
  Serial.begin(115200); // start serial for output
  delay(500);           //Wait for master to boot
  EEPROM.begin(1);
  int readEprom  = EEPROM.read(0);
  Serial.print("Eprom ");
  Serial.println(readEprom);
  
  if (readEprom >=1 || readEprom <=16 ) {
    I2C_SLAVE_ADDR = readEprom; //Set adress to current from Eeprom
    Serial.print("I2C adres set to ");
    Serial.println(readEprom);
    }
    else
    {
    Serial.println("Initial setup for empty Eeprom"); //This loop is not needed? 
    I2C_SLAVE_ADDR = 17;
    EEPROM.write(0, I2C_SLAVE_ADDR);
    EEPROM.commit();     
    }
  
  bool res = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR); //Start I2C with adres 
    if (!res) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }
    
  strip.Begin();
  strip.Show();  

  WireSlave.onRequest(requestEvent); // register event
  WireSlave.onReceive(receiveEvent);
}

void loop() {
  delay(1);
  WireSlave.update();
  strip.Show();
  
  if (M5.BtnA.pressedFor(2000))
    {
    M5.Lcd.printf("Reset I2C Bus adres");
    changeAdres(17); //Reset I2C adres
    }
}
