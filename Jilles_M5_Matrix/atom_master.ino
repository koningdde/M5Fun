#include <Arduino.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <NeoPixelBus.h>

#define SDA_PIN 21
#define SCL_PIN 25
#define I2C_SLAVE_ADDR 17
#define MAX_SLAVE_RESPONSE_LENGTH 1

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
      x = slaveReq.read();       // receive byte as a character
    } else {
      x = 0;
    }
    if (x==0){
      strip.SetPixelColor(i, hslRed);     
    } else {
      strip.SetPixelColor(i, black);        
    }
  }
  strip.Show();
  delay(500);
}
