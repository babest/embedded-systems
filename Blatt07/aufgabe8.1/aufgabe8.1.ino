int PIN_EN = 3;
int PIN_FIX = 2;

const int serialReceiveBufferSize = 200;
char serialReceiveBuffer[serialReceiveBufferSize];

char* checksum_str;

void setup() {
  //init console output
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {  
  if (Serial1.available()) {
    memset(serialReceiveBuffer, '\0', serialReceiveBufferSize);

    Serial1.readBytesUntil('\n', serialReceiveBuffer, serialReceiveBufferSize);


    Serial.print("Receive Command: ");
    Serial.println(serialReceiveBuffer);

    checksum_str = strtok(serialReceiveBuffer, "*");
    checksum_str = strtok(NULL, "*\n");
    Serial.print("Checksum: ");
    Serial.print(checksum_str);
    Serial.print(", expected: ");
    Serial.println(checksum(serialReceiveBuffer + 1));

    //TODO test Checksum
    char *pch = NULL;
    pch = strtok(serialReceiveBuffer, ",\n");
    if (strcmp("$GPGGA", pch) == 0) {
      //The GPS Signal data
      pch = strtok(NULL, ","); //time
      pch = strtok(NULL, ","); //lat
      Serial.print("Breitengrad: ");
      Serial.print(pch);
      pch = strtok(NULL, ","); //lat NS
      Serial.println(pch);

      pch = strtok(NULL, ","); //long
      Serial.print("Längengrad: ");
      Serial.print(pch);
      pch = strtok(NULL, ","); //long EW
      Serial.println(pch);

      pch = strtok(NULL, ","); //fix quality
      Serial.print("Signal qualität: ");
      Serial.println(pch);

      pch = strtok(NULL, ","); //# of satelites
      Serial.print("Anzahl Satelliten: ");
      Serial.println(pch);

      pch = strtok(NULL, ","); //diluation

      pch = strtok(NULL, ","); //altitude

      pch = strtok(NULL, ","); //altitude Meter

      pch = strtok(NULL, ","); //height of geoid

      pch = strtok(NULL, ","); //height of geoid Meter

      //empty
      //empty
      //checksum
    }
  }
}

int checksum(char *s) {
  int c = 0;

  while (*s)
    c ^= *s++;

  return c;
}

