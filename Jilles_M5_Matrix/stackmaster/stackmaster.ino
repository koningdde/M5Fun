//29-11-2020
int picture[1300];
int colorSaturation = 68;

#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WirePacker.h>
//#define DEBUG
String fileNames[20];
int w = 0;
#include "sdFunctions.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 18
#define MAX_SLAVE_RESPONSE_LENGTH 64

int knownDevices[17];   //Array to store "alive" devices

int indexNumber[] = {0,1195,1210,1225,1240,895,910,925,940,595,610,625,640,295,310,325,340};    //Store the first bit for the display
void sendPicture(int device);
void sendRandom(int device);
void changePic();
int actualPic=0;
int newPatern[16]; //Look for change in patern
int numberOfPics = 0; //Number of valid pictures on SD
void adresSet();
int currentFile = 255; //Placeholder fot current BMP file
bool autoChange = false;

long previousMillis = 0; 
long interval = 2000;
unsigned long currentMillis;

// Starting up loop
void setup()
{
  M5.begin();   // Initialize the M5Stack object
  Serial.begin(115200);   // start serial for output
  Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
  //M5.Lcd.setTextColor(0xffff);
    M5.Lcd.setCursor(35, 220);
    M5.Lcd.print("Change picture ");
    M5.Lcd.setCursor(135, 220);
    M5.Lcd.print("Autochange off"); 
    M5.Lcd.setCursor(230, 220);
    M5.Lcd.print("Bright: ");    
    M5.Lcd.print(colorSaturation);    
    M5.Lcd.print("  "); 

  listDir(SD, "/", 0);
  Serial.println("Bitmap list");
  
  for(int i = 0; i<20; i++)
  {
    if(fileNames[i] > " ")
    {
    numberOfPics+=1;
    Serial.println(fileNames[i]);
    }

  }
    Serial.print(numberOfPics);
    Serial.println(" .BMP Pictures found");  
    M5.Lcd.setCursor(0, 136);
    M5.Lcd.print(numberOfPics);
    M5.Lcd.print(" Pictures found on SD card");
    M5.Lcd.setCursor(0, 154);
    M5.Lcd.print("Interval: "); 
    M5.Lcd.print(interval);
    M5.Lcd.print("mS     ");
   // M5.Lcd.setTextColor(0xff80);
    M5.Lcd.setCursor(80, 180,4);
    M5.Lcd.print("Jilles M5 Matrix");
   // M5.Lcd.setTextColor(0xffff); 
    
    M5.Lcd.setCursor(0, 0, 1);
    
  changePic();
  
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
                        M5.lcd.print("Request picture");
                        if (newPatern[i] == 1){
                          sendPicture(i+1);
                          newPatern[i] = 0;
                        }
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
  
  if (M5.BtnA.wasPressed())
    {
    if(autoChange == false){changePic();}
    if(autoChange == true)
      {
        interval = interval + 2000;
        M5.Lcd.setCursor(0, 154);
        M5.Lcd.print("Interval: "); 
        M5.Lcd.print(interval);
        M5.Lcd.print("mS     ");
      }
    }

  if (M5.BtnB.wasPressed())
    {
    autoChange = !autoChange;
    M5.Lcd.setCursor(35, 220);
    if(autoChange == true){M5.Lcd.print("Change interval ");}
    if(autoChange == false){M5.Lcd.print("Change picture  ");}
    M5.Lcd.setCursor(135, 220);
    if(autoChange == false){M5.Lcd.print("Autochange off");}
    if(autoChange == true){M5.Lcd.print("Autochange on ");}
    M5.Lcd.setCursor(0, 0);
    }
    
   if (M5.BtnC.wasPressed())
    {
    colorSaturation = colorSaturation - 20;
    if(colorSaturation < 20){colorSaturation = 128;}
    M5.Lcd.setCursor(230, 220);
    M5.Lcd.print("Bright: ");    //Print array contents
    M5.Lcd.print(colorSaturation);    //Print array contents
    M5.Lcd.print("  ");    //Print array contents
    if (actualPic >0) {actualPic-=1;}
    else {actualPic = numberOfPics-1;}
    changePic();
    }

  currentMillis = millis();
  if(currentMillis - previousMillis > interval) 
  {
    previousMillis = currentMillis;
    if(autoChange == true){changePic();} 
  }
  
  M5.Lcd.setCursor(0, 0, 1);
  delay(10);
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
}

void sendPicture(int device){
            //Lets first find the row for the matrix     
             int row = indexNumber[device]; //Find the row where the matix is located, ingrease pixel number by row number*100
              WirePacker packer;
              packer.write('P');
              
            //for(int i = row; i>=(row-240); i=i-60) //Find first row, 2nd row, 3rd row, 4th row, 5th row
            for(int i = row; i>=(row-240); i=i-60)
            { 
              //Do Nothing
              #ifdef DEBUG
              Serial.print("ROW: ");  
              Serial.println(i);
              #endif
              
                for(int x=i; x<=(i+14); x++) //Find 1st, 2nd, 2rd, 4th and 5th pixel in colum
                {
                #ifdef DEBUG
                Serial.print("COL: ");  
                Serial.println(x);
                #endif
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

void changePic(){
    int str_len = fileNames[actualPic].length() + 1; 
    char char_array[str_len];
      if(str_len>2){ 
      fileNames[actualPic].toCharArray(char_array, str_len);
      readFile(SD, char_array); //readFile(SD, "/2.BMP"); 
      for(int i = 0; i<16; i++){newPatern[i] = 1;} 
      }
      actualPic+=1;
      if(actualPic == numberOfPics){actualPic = 0;}
      M5.Lcd.setCursor(0, 144);
      M5.Lcd.print("Showing picture: "); 
      M5.Lcd.print(char_array);
      M5.Lcd.setCursor(0, 0, 1);
}
