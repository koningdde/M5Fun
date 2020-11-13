#include <Arduino.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <NeoPixelBus.h>

#define SDA_PIN 21
#define SCL_PIN 25
#define I2C_SLAVE_ADDR 17
#define MAX_SLAVE_RESPONSE_LENGTH 64

const uint16_t PixelCount = 25; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 27;  // make sure to set this to the correct pin, ignored for Esp8266

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

void setup() {
  Serial.begin(115200);           // start serial for output
  Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
  strip.Begin();
  strip.Show();  
}

void loop() {
  int x = 0;
  for(int i=0;i<=15;i++) {
    WireSlaveRequest slaveReq(Wire, i+1, MAX_SLAVE_RESPONSE_LENGTH);
    slaveReq.setRetryDelay(5);
    bool success = slaveReq.request();

    if (success) {
            while (1 < slaveReq.available()) {  // loop through all but the last byte
                char c = slaveReq.read();       // receive byte as a character
                Serial.print(c);                // print the character
            }
            Serial.println();
            //int x = slaveReq.read();    // receive byte as an integer
            //Serial.println(x);          // print the integer
        }
        else {
            // if something went wrong, print the reason
            /*
            Serial.print(slaveReq.lastStatusToString());
            Serial.print(" Slave number: ");
            Serial.println(i + 1);
            */
        }
    /*
    if (success) {     
      x = slaveReq.read();       // receive byte as a character
      Serial.println(x);
    } else {
      x = 0;
    }
    if (x==0){
      strip.SetPixelColor(i, hslRed);     
    } else {
      strip.SetPixelColor(i, black);        
    }
  */
  strip.SetPixelColor(i+1, hslRed);
  strip.SetPixelColor(i, black); 
  if (i+1 == 1) {strip.SetPixelColor(16, black);}
  strip.Show();
  }
 
  //strip.Show();
  delay(500);
}
