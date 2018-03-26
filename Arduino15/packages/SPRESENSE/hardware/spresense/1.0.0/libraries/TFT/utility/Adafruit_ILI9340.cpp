/***************************************************
  Copyright (C) 2018 Sony Semiconductor Solutions Corp.
  This is an Arduino Library for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ILI9340.h"

#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

#define SPI_CLOCK       10000000

#define SET_BIT(port) \
{ \
    if (port != csport) \
        digitalWrite(*(port), HIGH); \
}

#define CLEAR_BIT(port) \
{ \
    if (port != csport) \
        digitalWrite(*(port), LOW); \
}

static inline uint8_t pgm_read_byte(uint8_t *addr)
{
    return *addr;
}

// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
Adafruit_ILI9340::Adafruit_ILI9340(uint8_t cs, uint8_t dc, uint8_t rst) : Adafruit_GFX(ILI9340_TFTWIDTH, ILI9340_TFTHEIGHT) {
  _cs   = cs;
  _dc   = dc;
  _rst  = rst;
}

void Adafruit_ILI9340::spiwrite(uint8_t c) {

  //Serial.print("0x"); Serial.print(c, HEX); Serial.print(", ");
  SPI.transfer(c);
}

void Adafruit_ILI9340::spiwrite(uint8_t* buf, uint16_t size) {
    SPI.transfer(buf, size);
}

void Adafruit_ILI9340::writecommand(uint8_t c) {
  CLEAR_BIT(dcport);
  CLEAR_BIT(csport);

  spiwrite(c);

  SET_BIT(csport);
}


void Adafruit_ILI9340::writedata(uint8_t c) {
  SET_BIT(dcport);
  CLEAR_BIT(csport);

  spiwrite(c);

  SET_BIT(csport);
}

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Adafruit_ILI9340::commandList(uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}

void Adafruit_ILI9340::begin(void) {
  pinMode(_rst, OUTPUT);
  digitalWrite(_rst, LOW);
  pinMode(_dc, OUTPUT);
  if (_cs)
    pinMode(_cs, OUTPUT);
  rsport = &_rst;
  csport = &_cs;
  dcport = &_dc;

  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();

  // toggle RST low to reset
  digitalWrite(_rst, HIGH);
  delay(5);
  digitalWrite(_rst, LOW);
  delay(20);
  digitalWrite(_rst, HIGH);
  delay(150);

  writecommand(0xEF);
  writedata(0x03);
  writedata(0x80);
  writedata(0x02);

  writecommand(0xCF);
  writedata(0x00);
  writedata(0XC1);
  writedata(0X30);

  writecommand(0xED);
  writedata(0x64);
  writedata(0x03);
  writedata(0X12);
  writedata(0X81);

  writecommand(0xE8);
  writedata(0x85);
  writedata(0x00);
  writedata(0x78);

  writecommand(0xCB);
  writedata(0x39);
  writedata(0x2C);
  writedata(0x00);
  writedata(0x34);
  writedata(0x02);

  writecommand(0xF7);
  writedata(0x20);

  writecommand(0xEA);
  writedata(0x00);
  writedata(0x00);

  writecommand(ILI9340_PWCTR1);    //Power control
  writedata(0x23);   //VRH[5:0]

  writecommand(ILI9340_PWCTR2);    //Power control
  writedata(0x10);   //SAP[2:0];BT[3:0]

  writecommand(ILI9340_VMCTR1);    //VCM control
  writedata(0x3e); //�Աȶȵ���
  writedata(0x28);

  writecommand(ILI9340_VMCTR2);    //VCM control2
  writedata(0x86);  //--

  writecommand(ILI9340_MADCTL);    // Memory Access Control
  writedata(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);

  writecommand(ILI9340_PIXFMT);
  writedata(0x55);

  writecommand(ILI9340_FRMCTR1);
  writedata(0x00);
  writedata(0x18);

  writecommand(ILI9340_DFUNCTR);    // Display Function Control
  writedata(0x08);
  writedata(0x82);
  writedata(0x27);

  writecommand(0xF2);    // 3Gamma Function Disable
  writedata(0x00);

  writecommand(ILI9340_GAMMASET);    //Gamma curve selected
  writedata(0x01);

  writecommand(ILI9340_GMCTRP1);    //Set Gamma
  writedata(0x0F);
  writedata(0x31);
  writedata(0x2B);
  writedata(0x0C);
  writedata(0x0E);
  writedata(0x08);
  writedata(0x4E);
  writedata(0xF1);
  writedata(0x37);
  writedata(0x07);
  writedata(0x10);
  writedata(0x03);
  writedata(0x0E);
  writedata(0x09);
  writedata(0x00);

  writecommand(ILI9340_GMCTRN1);    //Set Gamma
  writedata(0x00);
  writedata(0x0E);
  writedata(0x14);
  writedata(0x03);
  writedata(0x11);
  writedata(0x07);
  writedata(0x31);
  writedata(0xC1);
  writedata(0x48);
  writedata(0x08);
  writedata(0x0F);
  writedata(0x0C);
  writedata(0x31);
  writedata(0x36);
  writedata(0x0F);

  writecommand(ILI9340_SLPOUT);    //Exit Sleep
  delay(120);
  writecommand(ILI9340_DISPON);    //Display on
}

void Adafruit_ILI9340::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,
 uint16_t y1) {

  writecommand(ILI9340_CASET); // Column addr set
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);     // XSTART
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);     // XEND

  writecommand(ILI9340_PASET); // Row addr set
  writedata(y0>>8);
  writedata(y0);     // YSTART
  writedata(y1>>8);
  writedata(y1);     // YEND

  writecommand(ILI9340_RAMWR); // write to RAM
}

void Adafruit_ILI9340::pushColor(uint16_t color) {
  SET_BIT(dcport);
  CLEAR_BIT(csport);

  spiwrite(color >> 8);
  spiwrite(color);

  SET_BIT(csport);
}

void Adafruit_ILI9340::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x+1,y+1);

  SET_BIT(dcport);
  CLEAR_BIT(csport);

  spiwrite(color >> 8);
  spiwrite(color);

  SET_BIT(csport);
}

void Adafruit_ILI9340::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;

  if((y+h-1) >= _height)
    h = _height-y;

  setAddrWindow(x, y, x, y+h-1);

  uint8_t hi = color >> 8, lo = color;

  SET_BIT(dcport);
  CLEAR_BIT(csport);

  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  SET_BIT(csport);
}

void Adafruit_ILI9340::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  uint8_t hi = color >> 8, lo = color;
  SET_BIT(dcport);
  CLEAR_BIT(csport);
  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  SET_BIT(csport);
}

void Adafruit_ILI9340::fillScreen(uint16_t color) {
  fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void Adafruit_ILI9340::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  SET_BIT(dcport);
  CLEAR_BIT(csport);

  uint32_t size = w * h * sizeof(color);
  uint16_t color16 = color >> 8 | ((color & 0xff) << 8);

  if (size >= 32) {
    const int line_buf_size = 128;
    static uint32_t *start;
    static uint16_t last_color;
    static uint32_t last_fill;
    uint32_t *p;
    uint32_t fill_len;

    if (start == 0) {
      start = (uint32_t*)malloc(line_buf_size);
      memset(start, 0, line_buf_size);
    }

    if (!start)
      goto one_by_one;

    fill_len = size > line_buf_size? line_buf_size: size;
    fill_len = (fill_len + 3) & ~3;

    if (last_color != color || last_fill < fill_len) {
        uint32_t color32;
        *(uint16_t*) &color32 = color16;
        *((uint16_t*)&color32 + 1) = color16;
        uint32_t* end = start + fill_len/4;

        if (last_color != color)
            last_fill = 0;

        for (p = start + last_fill/4; p < end; ++p)
            *p = color32;

        last_color = color;
        last_fill = fill_len;
    }

    while (size) {
      if (size >= line_buf_size) {
        spiwrite((uint8_t*)start, line_buf_size);
        size -= line_buf_size;
      }
      else {
        spiwrite((uint8_t*)start, size);
        break;
      }
    }

    SET_BIT(csport);

    return;
  }

one_by_one:
  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      spiwrite((uint8_t*)&color16, sizeof(color16));
    }
  }
  SET_BIT(csport);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_ILI9340::Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void Adafruit_ILI9340::setRotation(uint8_t m) {

  writecommand(ILI9340_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTWIDTH;
     _height = ILI9340_TFTHEIGHT;
     break;
   case 1:
     writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTHEIGHT;
     _height = ILI9340_TFTWIDTH;
     break;
  case 2:
    writedata(ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTWIDTH;
     _height = ILI9340_TFTHEIGHT;
    break;
   case 3:
     writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_MY | ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTHEIGHT;
     _height = ILI9340_TFTWIDTH;
     break;
  }
}

void Adafruit_ILI9340::invertDisplay(boolean i) {
  writecommand(i ? ILI9340_INVON : ILI9340_INVOFF);
}

////////// stuff not actively being used, but kept for posterity
uint8_t Adafruit_ILI9340::spiread(void) {
  return SPI.transfer(0x00);
}
