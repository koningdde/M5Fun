void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);
 
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
    
  while (file.available()) {
    int ch = file.read();
    //Serial.write(ch);
    //M5.Lcd.write(ch);
    i=i+1;
    //Serial.println(i);
    picture[i] = ch;
  }

  #ifdef DEBUG
//Index 55 is first color
  for(int x = 0; x<=1299; x++){
    Serial.print("Index: ");
    Serial.print(x);
    Serial.print(" ");
    Serial.println(picture[x]);
  }
  #endif

  for(int x=0;x<=1299; x++)
  {
    picture[x] = map(picture[x],0,255,0,128);
  }

  #ifdef DEBUG
    for(int x = 0; x<=1299; x++){
    Serial.print("Index: ");
    Serial.print(x);
    Serial.print(" ");
    Serial.println(picture[x]);
  }
  #endif
  
}
