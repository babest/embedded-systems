#include <SPI.h>
#include <SD.h>

extern unsigned char font[95][6];

int PIN_SCE   = 4;
int PIN_RESET = 6;
int PIN_DC    = 8;
int PIN_SDIN  = 4;
int PIN_SCLK  = 3;
int PIN_BGLIGHT = 10;
int PIN_SCE_SD = 52;

int LCD_C     = LOW;
int LCD_D     = HIGH;

const int LCD_X     = 84;
const int LCD_Y     = 48;
const int LCD_BANKS = LCD_Y / 8;
const int LCD_CHAR_WIDTH = 6;
const int LCD_CPL   = LCD_X / LCD_CHAR_WIDTH;

unsigned int LCD_Buffer[LCD_BANKS][LCD_X];
unsigned int LCD_POS = 0;

const char* authors[][3] = {
  {"Timon", "Back", "6526091"},
  {"Fabian", "Behrendt", "6534523"},
  {"Nicolai", "Staeger", "6537387"}
};



void setup(void)
{
  // Init Pins
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_BGLIGHT, OUTPUT);
  
  //Serial
  Serial.begin(9600);

  // LCD RESET
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  digitalWrite(PIN_RESET, LOW);
  delay(500);
  digitalWrite(PIN_RESET, HIGH);

  // LCD Lighting
  digitalWrite(PIN_BGLIGHT, HIGH);

  // LCD SPI
  SPI.begin(PIN_SCE);
  SPI.setClockDivider(PIN_SCLK, 84);

  // LCD SPI INIT
  sendCommand(0x21); // 0010 0001
  sendCommand(0x14); // 0001 0100
  sendCommand(0xB1); // Set Contrast E0 = 1110 0000
  sendCommand(0x20); // 0010 0000
  sendCommand(0x0C); // 0000 1100
  
  //LCD Test Data
  drawString(2, "<Hello World>");
  drawString(3, "--Hello World--");
  
  // SD Init
  pinMode(PIN_SCE_SD, OUTPUT);
  Serial.println(SD.begin(PIN_SCE_SD));
}

void clearDisplay()
{
  for (int y = 0; y < LCD_BANKS; ++y) {
    for (int x = 0; x < LCD_X; x++) {
      setByte(x, y, 0);
    }
  }
  flushLCD();
}

int setPixel(unsigned int x, unsigned int y, unsigned int value) {
  if (x < LCD_X && y < LCD_Y) {
    int yBank = y / 8;
    int yOffset = y % 8;
    // Set the bit in the byte
    LCD_Buffer[yBank][x] ^= (-value ^ LCD_Buffer[yBank][x]) & (1 << yOffset);

    return 0;
  }

  return -1;
}

int setByte(unsigned int x, unsigned int yBank, unsigned int value) {
  if (x < LCD_X && yBank < LCD_BANKS) {
    // Set the byte
    LCD_Buffer[yBank][x] = value;
    return 0;
  }

  return -1;
}
int drawChar(unsigned int x, unsigned int yBank, char character)
{
  if (x < LCD_X && yBank < LCD_BANKS) {
    unsigned char* letter = font[character - 32];

    if (x < LCD_X && yBank < LCD_BANKS) {
      for (int i = 0; i < 6; ++i) {
        setByte(x + i, yBank, *(letter + i));
      }
    }

    flushLCD();

    return 0;
  }
  return -1;
}

int drawString(unsigned int yBank, const char* string)
{
  if (strlen(string) <= LCD_CPL || yBank < LCD_BANKS) {
    int xStartPos = (LCD_X - (strlen(string) * LCD_CHAR_WIDTH)) / 2;

    for (int i = 0; i < strlen(string); ++i) {
      drawChar(xStartPos + (i * LCD_CHAR_WIDTH), yBank, string[i]);
    }

    flushLCD();
    return 0;
  }
  return -1;
}

int readImageHeader(const char* header, unsigned int* x, unsigned int* y)
{
  *y = 0;
  *x = 0;

  
  *x = atoi(header);
  
  for(int i = 0; i < strlen(header); i++)
  {
    if(header[i] == ',')
    {
      *y = atoi(header + i + 1);
      return 0;
    }
  }
  return -1;
}

int drawImage(char* rawData)
{
  unsigned int imageSizeX, imageSizeY;
  
  char* pch = strtok(rawData, "\n");
  readImageHeader(pch, &imageSizeX, &imageSizeY);
  
  unsigned int xOffset, yOffset;
  xOffset = (LCD_X - imageSizeX) / 2;
  yOffset = (LCD_Y - imageSizeY) / 2;
  
  for(int y = 0; y < imageSizeY; y++)
  {
    for(int x = 0; x < imageSizeX; x++)
    {
      pch = strtok(NULL, ",\n");
      
      setPixel(x+xOffset, y+yOffset, (*pch == '1') );
      
    }
  }
  
  flushLCD();
  
}

int scrollString(unsigned int yBank, const char* string) {
  //Temporärer String Buffer, der einmal komplett aufs Display passt
  char drawStringBuffer[LCD_CPL + 1];
  //Den temporären String Buffer terminieren (strlen muss funktionieren)
  drawStringBuffer[LCD_CPL] = '\0';
  
  //loopCounterMax definiert die Druchgänge in der Schleife [Scrollschritte]
  unsigned int loopCounterMax = strlen(string);
  if(loopCounterMax < LCD_CPL) {
    //Der Text passt aufs Display? Dann kein Scrollen, nur anzeigen
    loopCounterMax = 1;
  } else if(LCD_CPL < strlen(string)) {
    //Der Text muss gescrollt werden.
    //Aber nur bis zum letzten Zeichen. Keine Leerzeichen am Ende
    loopCounterMax-= LCD_CPL + 1;
  }
  
  //Den Text anzeigen
  for(int i=0; i<loopCounterMax; i++) {
    //Aktuellen Textausschnitt in den Buffer kopieren
    strncpy(drawStringBuffer, string+i, LCD_CPL);
    //Text auf dem Display anzeigen
    drawString(yBank, drawStringBuffer);
    //Dem Mensch die Chance lassen, den Text zu lesen
    delay(250);
  }
}

void sendCommand(unsigned int value) {
  digitalWrite(PIN_DC, LCD_C);
  SPI.transfer(PIN_SDIN, value);
}

void sendData(unsigned int value) {
  digitalWrite(PIN_DC, LCD_D);
  SPI.transfer(PIN_SDIN, value);
}

// Write the Buffer to the display
void flushLCD() {
  for (int y = 0; y < LCD_BANKS; y++) {
    for (int x = 0; x < LCD_X; x++) {
      sendData(LCD_Buffer[y][x]);
    }
  }
}

// TODO: remove if not needed
void gotoXY(unsigned int x, unsigned int y)
{
  if (x < LCD_X && y < LCD_Y) {
    sendCommand(0x80 | x); //column
    sendCommand(0x40 | y); //row
    LCD_POS = (LCD_X * y) + x;
  }
}

int readFileToBuffer(char* filename, char *buffer, unsigned int length) {
  if (SD.exists(filename)) {
    //File exists
    
    //Open the file
    File dataFile = SD.open(filename);
    
    if (dataFile) {
      //File is opened
      
      //Counter for amount of bytes read from the file
      unsigned int bytesRead = 0;
    
      //Read the bytes as long as there are still bytes and the buffer is large enough
      while (dataFile.available() && bytesRead < length) {
        char character = dataFile.read();
        *(buffer+bytesRead) = character;
        bytesRead++;
      }
      //Terminate buffer
      *(buffer+bytesRead) = '\n';
      
      //Clean up
      dataFile.close();
      
      return bytesRead;
    } else {
      Serial.println("Cant open");
      return -1;
    }
  }
  
  Serial.println("Does not exist");
  return -2;
}

void loop(void)
{
    
  /*int imageTextLength = LCD_X * LCD_Y * 2 + 20;
  char textContent[imageTextLength];
  
  clearDisplay();

  clearString(textContent, imageTextLength);
  readFileToBuffer("TAMS.IMG", textContent, imageTextLength);
  drawImage(textContent);
  
  delay(1000);

  clearDisplay();

  clearString(textContent, imageTextLength);
  readFileToBuffer("SMILE1.IMG", textContent, imageTextLength);
  drawImage(textContent);
  
  delay(1000);
  
  clearDisplay();

  clearString(textContent, imageTextLength);
  readFileToBuffer("SMILE2.IMG", textContent, imageTextLength);
  drawImage(textContent);
  
  delay(1000);
  
  clearDisplay();

  clearString(textContent, imageTextLength);
  readFileToBuffer("SMILE3.IMG", textContent, imageTextLength);
  drawImage(textContent);
  
  delay(1000);
  
  return;*/
  
  char textContent[200];
  
  clearString(textContent, 200);
  readFileToBuffer("TEXT1.TXT", textContent, 200);
  scrollString(0, textContent);
  
  clearString(textContent, 200);
  readFileToBuffer("TEXT2.TXT", textContent, 200);
  scrollString(1, textContent);
  delay(2000);
  
  return;
  
  
  for (int i = 0; i < 3; ++i) {
    clearDisplay();
    drawString(1, authors[i][0]);
    drawString(2, authors[i][1]);
    drawString(3, authors[i][2]);

    delay(5000);
  }
}

void clearString(char* buffer, unsigned int length) {
  for(int i=0; i<length;++i) {
    buffer[i] = '\0';
  }
}

void drawChessField()
{
  // Create a chess field
  for (int y = 0; y < LCD_Y; y = y + 2) {
    for (int x = 0; x < LCD_X; x++) {
      setPixel(x, y, x % 2);
      setPixel(x, y + 1, (1 - (x % 2)));
    }
  }
  
  flushLCD();
}

