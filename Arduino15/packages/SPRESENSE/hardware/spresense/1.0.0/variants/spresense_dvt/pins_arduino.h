/*
  pins_arduino.h - Pin definition functions for Arduino
  Copyright (C) 2018 Sony Semiconductor Solutions Corp.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis
  Modified for ESP8266 platform by Ivan Grokhotkov, 2014-2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <cxd56_pinconfig.h>

#define NUM_DIGITAL_PINS    (22)
#define NUM_ANALOG_INPUTS   (6)
#define PIN_NOT_ASSIGNED    (0xFF)

// Variant definition
#define TARGET_USES_DVT_BOARD

// Digital pins
#define PIN_D00     (uint8_t)(0)
#define PIN_D01     (uint8_t)(1)
#define PIN_D02     (uint8_t)(2)
#define PIN_D03     (uint8_t)(3)
#define PIN_D04     (uint8_t)(4)
#define PIN_D05     (uint8_t)(5)
#define PIN_D06     (uint8_t)(6)
#define PIN_D07     (uint8_t)(7)
#define PIN_D08     (uint8_t)(8)
#define PIN_D09     (uint8_t)(9)
#define PIN_D10     (uint8_t)(10)
#define PIN_D11     (uint8_t)(11)
#define PIN_D12     (uint8_t)(12)
#define PIN_D13     (uint8_t)(13)
#define PIN_D14     (uint8_t)(14)
#define PIN_D15     (uint8_t)(15)
#define PIN_D16     (uint8_t)(16)
#define PIN_D17     (uint8_t)(17)
#define PIN_D18     (uint8_t)(18)
#define PIN_D19     (uint8_t)(19)
#define PIN_D20     (uint8_t)(20)
#define PIN_D21     (uint8_t)(21)
#define PIN_D22     (uint8_t)(22)
#define PIN_D23     (uint8_t)(23)
#define PIN_D24     (uint8_t)(24)
#define PIN_D25     (uint8_t)(25)
#define PIN_D26     (uint8_t)(26)
#define PIN_D27     (uint8_t)(27)
#define PIN_D28     (uint8_t)(28)

// LED
#define PIN_LED0    (uint8_t)(100)
#define PIN_LED1    (uint8_t)(101)
#define PIN_LED2    (uint8_t)(102)
#define PIN_LED3    (uint8_t)(103)

static const uint8_t LED0 = PIN_LED0;
static const uint8_t LED1 = PIN_LED1;
static const uint8_t LED2 = PIN_LED2;
static const uint8_t LED3 = PIN_LED3;

#define PIN_DA0     PIN_D14
#define PIN_DA1     PIN_D15
#define PIN_DA2     PIN_D16
#define PIN_DA3     PIN_D17
#define PIN_DA4     PIN_D18
#define PIN_DA5     PIN_D19

#define PIN_PWM_0   PIN_D06
#define PIN_PWM_1   PIN_D05
#define PIN_PWM_2   PIN_D09
#define PIN_PWM_3   PIN_D03

static const uint8_t DA0 = PIN_DA0;
static const uint8_t DA1 = PIN_DA1;
static const uint8_t DA2 = PIN_DA2;
static const uint8_t DA3 = PIN_DA3;
static const uint8_t DA4 = PIN_DA4;
static const uint8_t DA5 = PIN_DA5;

// Analog pins
#define PIN_A0      (0u)
#define PIN_A1      (1u)
#define PIN_A2      (2u)
#define PIN_A3      (3u)
#define PIN_A4      (4u)
#define PIN_A5      (5u)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;

// SPI pins
#define PIN_SPI_SS    PIN_D10
#define PIN_SPI_MOSI  PIN_D11
#define PIN_SPI_MISO  PIN_D12
#define PIN_SPI_SCK   PIN_D13
static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

// TWI pins
#define PIN_WIRE_SCL  PIN_D21
#define PIN_WIRE_SDA  PIN_D20
static const uint8_t SCL = PIN_WIRE_SCL;
static const uint8_t SDA = PIN_WIRE_SDA;

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_MONITOR   Serial
#define SERIAL_PORT_HARDWARE  Serial

// WiFi Pins
#define WIFI_READY      7  // handshake pin
//#define WIFI_LED     9   // led on wifi shield

#endif /* Pins_Arduino_h */
