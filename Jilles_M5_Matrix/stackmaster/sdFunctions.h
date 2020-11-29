void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);
  int i = 0;
  
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
 
  while (file.available()) {
    int ch = file.read();
    i=i+1;
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
    picture[x] = map(picture[x],0,255,0,colorSaturation);
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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {

  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      String testName = file.name();

      if(testName.endsWith("BMP"))
        {
        fileNames[w] = file.name();
        w = w +1;
        }
      
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
