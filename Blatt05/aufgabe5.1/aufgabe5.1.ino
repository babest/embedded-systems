#include <SPI.h>



int PIN_SCE   = 4;
int PIN_RESET = 6;
int PIN_DC    = 8;
int PIN_SDIN  = 4;
int PIN_SCLK  = 3;

int LCD_C     = LOW;
int LCD_D     = HIGH;

const int LCD_X     = 84;
const int LCD_Y     = 48;
const int LCD_BANKS = LCD_Y/8;

unsigned int LCD_PUFFER[LCD_X*LCD_Y];
unsigned int LCD_Buffer[LCD_BANKS][LCD_X];
unsigned int LCD_POS = 0;



void setup(void)
{
  //Init Pins
   pinMode(PIN_RESET, OUTPUT);
   pinMode(PIN_DC, OUTPUT);
   
   //LCD RESET
   digitalWrite(PIN_RESET, HIGH);
   delay(100);
   digitalWrite(PIN_RESET, LOW);
   delay(500);
   digitalWrite(PIN_RESET, HIGH);
   
   //LCD SPI
   SPI.begin(PIN_SCE);
   SPI.setClockDivider(PIN_SCLK, 84);
   
   //LCD SPI INIT
   sendCommand(0x21);//0010 0001
   sendCommand(0x14);//0001 0100
   sendCommand(0xB1);//E0
   sendCommand(0x20);//0010 0000
   sendCommand(0x0C);//0000 1100
   
   //LCD PUFFER INIT
   for (int i=0; i<LCD_X*LCD_Y; i++) {
     LCD_PUFFER[i] = 0;
   }
   
   // Create a chess field
   for (int y=0; y<LCD_Y; y=y+2) {
     for (int x=0; x<LCD_X; x++) {
       setPixel(x, y, x%2);
       setPixel(x, y+1, (1-(x%2)));
     }
   }
   
   flushLCD();
}



void setPixel(unsigned int x, unsigned int y, unsigned int value) {
  if (x<LCD_X && y < LCD_Y) {
    int yBank = y/8;
    int yOffset = y%8;
    // Set the bit in the byte
    LCD_Buffer[yBank][x] ^= (-value ^ LCD_Buffer[yBank][x]) & (1 << yOffset);
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
  for (int y=0; y<LCD_BANKS; y++) {
    for (int x=0; x<LCD_X; x++) {
      sendData(LCD_Buffer[y][x]);
    }
  }
}

// TODO: remove if not needed
void gotoXY(unsigned int x, unsigned int y) 
{
  if (x<LCD_X && y < LCD_Y) {
    sendCommand(0x80 | x); //column
    sendCommand(0x40 | y); //row
    LCD_POS = (LCD_X * y) + x;
  }
}



void loop(void)
{
}
