#include <SPI.h>

extern unsigned char font[95][6];

int PIN_SCE   = 4;
int PIN_RESET = 6;
int PIN_DC    = 8;
int PIN_SDIN  = 4;
int PIN_SCLK  = 3;
int PIN_BGLIGHT = 10;

int LCD_C     = LOW;
int LCD_D     = HIGH;

const int LCD_X     = 84;
const int LCD_Y     = 48;
const int LCD_BANKS = LCD_Y / 8;
const int LCD_CHAR_WIDTH = 6;

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

  drawString(2, "<Hello World>");
  drawString(3, "--Hello World--");
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
  if (strlen(string) <= LCD_X / LCD_CHAR_WIDTH || yBank < LCD_BANKS) {
    int xStartPos = (LCD_X - (strlen(string) * LCD_CHAR_WIDTH)) / 2;

    for (int i = 0; i < strlen(string); ++i) {
      drawChar(xStartPos + (i * LCD_CHAR_WIDTH), yBank, string[i]);
    }

    flushLCD();
    return 0;
  }
  return -1;
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



void loop(void)
{
  for (int i = 0; i < 3; ++i) {
    clearDisplay();
    drawString(1, authors[i][0]);
    drawString(2, authors[i][1]);
    drawString(3, authors[i][2]);

    delay(5000);
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
