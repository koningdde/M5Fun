//Master for ATOM matrix, run on M5 Core
//When using 16 atoms make sure to use an 3 amp power supply
//04-12-2020 https://github.com/koningdde/M5Fun
//Bitmap structure https://en.wikipedia.org/wiki/BMP_file_format
//Place only 24 Bit, 20x20 pixel with the .BMP extention on the sd card.

int picture[1300];    //Placeholder for active picture
int colorSaturation = 68;   //Brightness at startup

//Dependicies
#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <WirePacker.h>

//#define DEBUG

String fileNames[20];   //Placeholder for gilenames
int w = 0;    //Counter for valid BMP files

//Dependicies
#include "sdFunctions.h"

//I2C Setup
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 18
#define MAX_SLAVE_RESPONSE_LENGTH 64

//Picture setup
int knownDevices[17];   //Array to store "alive" devices
int indexNumber[] = {0,1195,1210,1225,1240,895,910,925,940,595,610,625,640,295,310,325,340};    //Store the first bit for the display
void sendPicture(int device);
void sendRandom(int device);
void changePic();
int actualPic=0;
int newPatern[16];    //Look for change in patern
int numberOfPics = 0;   //Number of valid pictures on SD

//Change slave adres function
void adresSet();

//Autochange setup
int currentFile = 255;    //Placeholder fot current BMP file
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
  
    M5.Lcd.setCursor(35, 220);
    M5.Lcd.print("Change picture ");
    M5.Lcd.setCursor(135, 220);
    M5.Lcd.print("Autochange off"); 
    M5.Lcd.setCursor(230, 220);
    M5.Lcd.print("Bright: ");    
    M5.Lcd.print(colorSaturation);    
    M5.Lcd.print("  "); 

  //Read the SD card
  listDir(SD, "/", 0);
  Serial.println("Bitmap list");
  
  //Count the valid files names
  for(int i = 0; i<20; i++)
  {
    if(fileNames[i] > " ")
    {
    numberOfPics+=1;
    Serial.println(fileNames[i]);
    }
    
  //Show results
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
    M5.Lcd.setCursor(80, 180,4);
    M5.Lcd.print("Jilles M5 Matrix"); 
    M5.Lcd.setCursor(0, 0, 1);
  
  //Load first picture
  changePic();
  
}//End setup loop

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
                  
                    //Nothing here
                
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
  }//End for loop

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
        interval = interval + 2000;   //Change autochange interval
        M5.Lcd.setCursor(0, 154);
        M5.Lcd.print("Interval: "); 
        M5.Lcd.print(interval);
        M5.Lcd.print("mS     ");
      }
    }

  if (M5.BtnB.wasPressed())
    {
    autoChange = !autoChange;   //Set autochange on or off
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
    M5.Lcd.print("Bright: ");    
    M5.Lcd.print(colorSaturation);    
    M5.Lcd.print("  ");    
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
              Wire.beginTransmission(17);   //Send back the adres to unit 17
              while (packer.available()) 
                {    // write every packet byte
                Wire.write(packer.read());
                }
              Wire.endTransmission();         // stop transmitting
              break; //No need to search further, we have an valid adres
              }
          // Adres is in use, do nothing, search for next adres
          } // End for loop
}//End function

void sendPicture(int device){    
              int row = indexNumber[device]; //Find the first pixel for the atom.
              WirePacker packer;
              packer.write('P');
              
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
      readFile(SD, char_array);   //Read the file from the SD
      for(int i = 0; i<16; i++){newPatern[i] = 1;}    //Set the array to know a new picture is ready
      }
      actualPic+=1;
      if(actualPic == numberOfPics){actualPic = 0;}
      M5.Lcd.setCursor(0, 144);
      M5.Lcd.print("Showing picture: "); 
      M5.Lcd.print(char_array);
      M5.Lcd.setCursor(0, 0, 1);
}
