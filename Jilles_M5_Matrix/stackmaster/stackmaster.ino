#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WirePacker.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 17
#define MAX_SLAVE_RESPONSE_LENGTH 64
int slaveAdres = 1;

// the setup routine runs once when M5Stack starts up
void setup(){

  // Initialize the M5Stack object
  M5.begin();

  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  //M5.Power.begin();
  Serial.begin(115200);           // start serial for output
  Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
  
  // LCD display
  M5.Lcd.println("Hello World");
}

// the loop routine runs over and over again forever
void loop() {
  int x = 0;
  for(int i=0;i<=15;i++) {
    WireSlaveRequest slaveReq(Wire, i+1, MAX_SLAVE_RESPONSE_LENGTH);
    slaveReq.setRetryDelay(5);
    bool success = slaveReq.request();

    if (success) 
            {
                while (1 < slaveReq.available()) {
                char c = slaveReq.read();       // receive byte as a character
                if (c == 'A') {adresSet();}
                M5.Lcd.print(c);                // print the character
                
            }
            }
            else 
            {
            // if something went wrong, print the reason
            
            M5.Lcd.print(slaveReq.lastStatusToString());
            M5.Lcd.print(" Slave number: ");
            M5.Lcd.print(i + 1);
            }
            
       M5.Lcd.println("                     ");
  }
 
  if(x==0) {M5.Lcd.setCursor(0, 0, 1);}
  delay(500);
}

void adresSet(){
        WirePacker packer;

        // then add data the same way as you would with Wire
        packer.write('A');
        packer.write(slaveAdres);
        //packer.write(x);

        // after adding all data you want to send, close the packet
        packer.end();

        // now transmit the packed data
        Wire.beginTransmission(16);
        while (packer.available()) {    // write every packet byte
            Wire.write(packer.read());
        }
        Wire.endTransmission();         // stop transmitting
slaveAdres +=1;
}
