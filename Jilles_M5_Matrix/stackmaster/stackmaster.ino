#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WirePacker.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 18
#define MAX_SLAVE_RESPONSE_LENGTH 64
#define DEBUG

int knownDevices[16];   //Array to store "alive" devices
int picture[] = {
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

int rowNumber[] = {0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3};    //Store the row where a matrix is located, first array bit is not used.

// Starting up loop
void setup()
{
  M5.begin();   // Initialize the M5Stack object
  Serial.begin(115200);   // start serial for output
  Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
}

// The actual happening
void loop() {

  for(int i=0;i<=16;i++) {
    WireSlaveRequest slaveReq(Wire, i+1, MAX_SLAVE_RESPONSE_LENGTH);
    slaveReq.setRetryDelay(5);
    bool success = slaveReq.request();

    if (success) 
            {
                knownDevices[i] = 1;    //Store the device precense in de array
                while (1 < slaveReq.available()) 
                  {
                  char c = slaveReq.read();   // receive byte as a character
                    if (c == 'A') 
                      {
                      adresSet();
                      }
                    if (c == 'P') 
                      {
                      sendPicture(i+1);   //Send picture for device i+1
                      }                  
                  
                    M5.Lcd.print(c);    // print the received character
                
                    }
            }
            else 
              {
              // if something went wrong, print the reason
              knownDevices[i+1] = 0;    //Delete the device from known devices array
              M5.Lcd.print(slaveReq.lastStatusToString());
              M5.Lcd.print(" Slave number: ");
              M5.Lcd.print(i + 1);
              }
            
       M5.Lcd.println("                     ");   //Clear line 
  }
  
#ifdef DEBUG
  for(x=0;x<=15;x++)
    {
    M5.Lcd.print(knownDevices[x]);    //Print array contents
    }
  
  M5.Lcd.setCursor(0, 0, 1);
  delay(10);
}
#endif

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
              Wire.beginTransmission(17);
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
                for(int x=i; x<=i+4; x++) //Find 1st, 2nd, 2rd, 4th and 5th pixel in colum
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

