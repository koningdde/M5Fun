#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WirePacker.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 18
#define MAX_SLAVE_RESPONSE_LENGTH 64
int knownDevices[16];   //Array to store "alive" devices
//int picture[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
int pircture[] = {
1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1
0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0
0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0
0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0
0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0
0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0
0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0
0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0
0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0
0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0
}

int rowNumber[] = {0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3}; //Store the row where a matrix is located, first array bit is not used.

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
  
}

// the loop routine runs over and over again forever
void loop() {

  for(int i=0;i<=16;i++) {
    WireSlaveRequest slaveReq(Wire, i+1, MAX_SLAVE_RESPONSE_LENGTH);
    slaveReq.setRetryDelay(5);
    bool success = slaveReq.request();

    if (success) 
            {
                knownDevices[i] = 1;          //Store the device precense in de array
                while (1 < slaveReq.available()) 
                  {
                  char c = slaveReq.read();       // receive byte as a character
                    if (c == 'A') 
                      {
                      adresSet();
                      }
                    if (c == 'P') 
                      {
                      sendPicture(i+1);           //Request picture for device i+1
                      }                  
                  
                    M5.Lcd.print(c);                // print the character
                
                    }
            }
            else 
              {
              // if something went wrong, print the reason
              knownDevices[i+1] = 0;    //Delete the device from known devices
              M5.Lcd.print(slaveReq.lastStatusToString());
              M5.Lcd.print(" Slave number: ");
              M5.Lcd.print(i + 1);
              }
            
       M5.Lcd.println("                     ");
  }
  
  for(x=0;x<=15;x++)
    {
    M5.Lcd.print(knownDevices[x]);    //Print array contents
    }
  
  M5.Lcd.setCursor(0, 0, 1);
  delay(10);
}

void adresSet(){
        for(int x=0; x<=15; x++)
          {
            if (knownDevices[x]==0)   //Find first available adres
              {
              WirePacker packer;
              // then add data the same way as you would with Wire
              packer.write('A');
              packer.write(x+1);
              // after adding all data you want to send, close the packet
              packer.end();

              // now transmit the packed data
              Wire.beginTransmission(16);
              while (packer.available()) 
                {    // write every packet byte
                Wire.write(packer.read());
                }
              Wire.endTransmission();         // stop transmitting
              break; //No need to search further
              }
          // Adres is in use, do nothing
          } // End for loop
  
  void sendPicture(int device){
            //Lets first find the row for the matrix
             int row = (rowNumber[device]*100); //Find the row where the matix is located, ingrease pixel number by row number*100
    
              WirePacker packer;
    
            for(int i = 0+row; i<=80+row; i+=20) //Find first row, 2nd row, 3rd row, 4th row, 5th row
            {   //Do nothing
                for(int x=i; x<=i+4; x++)
                {
                // then add data the same way as you would with Wire
                packer.write(picture[x+((device-1)*5)]);  //Device number*5 to find pixels in array, first 0-4, next 20-24
                }//end second for loop
            }//end first for loop
    
              // after adding all data you want to send, close the packet
              packer.end();

              // now transmit the packed data
              Wire.beginTransmission(device);
              while (packer.available()) 
                {    // write every packet byte
                Wire.write(packer.read());
                }
              Wire.endTransmission();         // stop transmitting
              } // End for loop

