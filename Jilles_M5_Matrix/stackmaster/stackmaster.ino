int i = 0;
int picture[1300];

#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WirePacker.h>
//#define DEBUG
#include "sdFunctions.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 18
#define MAX_SLAVE_RESPONSE_LENGTH 64

int knownDevices[17];   //Array to store "alive" devices

int indexNumber[] = {0,1195,1210,1225,1240,895,910,925,940,595,610,625,640,295,310,325,340};    //Store the first bit for the display
void sendPicture(int device);
void sendRandom(int device);
void adresSet();
int patern = 0;

// Starting up loop
void setup()
{
  M5.begin();   // Initialize the M5Stack object
  Serial.begin(115200);   // start serial for output
  Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
  readFile(SD, "/2.bmp");
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
                      M5.Lcd.print("Request device ID");    // print the received character
                      adresSet();
                      }
                    if (c == 'P') 
                      {
                      if (patern == 0){sendPicture(i+1);}
                      if (patern == 1){sendRandom(i+1);}
                      M5.lcd.print("Request picture");
                      }                  
                  
                    //None
                
                    }
            }
            else 
              {
              // if something went wrong, print the reason
              knownDevices[i] = 0;    //Delete the device from known devices array
              M5.Lcd.print(slaveReq.lastStatusToString());
              M5.Lcd.print(" Slave number: ");
              M5.Lcd.print(i + 1);
              }
            
       M5.Lcd.println("                 ");   //Clear line 
  }

#ifdef DEBUG
  for(int x=0;x<=15;x++)
    {
    M5.Lcd.print(knownDevices[x]);    //Print array contents
    }
  #endif
  
  
  M5.Lcd.setCursor(0, 0, 1);
  delay(10);

  if (M5.BtnA.wasPressed())
    {
    patern = patern + 1;
    if(patern == 2){patern = 0;}
    }
    
  M5.update();
}


void adresSet(){
        for(int x=0; x<=15; x++)
          {
            if (knownDevices[x]==0)   //Find first available adres
              {
              WirePacker packer;
              // then add data the same way as you would with Wire
              packer.write('A');
              packer.write(4);
              //packer.write(x+1);
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
}

  void sendPicture(int device){
            //Lets first find the row for the matrix     
             int row = indexNumber[device]; //Find the row where the matix is located, ingrease pixel number by row number*100
              WirePacker packer;
              packer.write('P');
              
            //for(int i = row; i>=(row-240); i=i-60) //Find first row, 2nd row, 3rd row, 4th row, 5th row
            for(int i = row; i>=(row-240); i=i-60)
            { 
              Serial.print("ROW: ");  
              Serial.println(i);
                for(int x=i; x<=(i+14); x++) //Find 1st, 2nd, 2rd, 4th and 5th pixel in colum
                {
                Serial.print("COL: ");  
                Serial.println(x);
                packer.write(picture[x]);  //Device number*5 to find pixels in array, first 0-4, next 20-24
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
    } // End function

 void sendRandom(int device){
            //Lets first find the row for the matrix            
              WirePacker packer;
              packer.write('P');
              
            for(int i = 0; i<=75; i++) //Find first row, 2nd row, 3rd row, 4th row, 5th row
                {
                
                packer.write(random(0,128));  //Device number*5 to find pixels in array, first 0-4, next 20-24
                }//end second for loop
    
              // after adding all data you want to send, close the packet
              packer.end();

              // now transmit the packed data
              Wire.beginTransmission(device);
              while (packer.available()) 
                {    // write every packet byte
                Wire.write(packer.read());
                }
              Wire.endTransmission();         // stop transmitting
} // End random
