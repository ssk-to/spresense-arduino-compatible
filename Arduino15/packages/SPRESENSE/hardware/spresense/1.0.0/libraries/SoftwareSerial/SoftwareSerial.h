/*
  SoftwareSerial.h - SoftwareSerial include file for the SPRESENSE SDK
  Copyright (C) 2018 Sony Semiconductor Solutions Corp.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SoftwareSerial_h
#define SoftwareSerial_h

#include <inttypes.h>
#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#ifndef SS_MAX_RX_BUFF
#define SS_MAX_RX_BUFF 64 // RX buffer size
#endif

class SoftwareSerial : public Stream
{
private:
  uint8_t _receivePin;
  uint8_t _transmitPin;
  uint32_t _receivePinRegAddr;
  uint32_t _transmitPinRegAddr;

  /* 4-cycle delays (must never be 0!) */
  unsigned long _rx_delay_centering;
  unsigned long _rx_delay_intrabit;
  unsigned long _tx_delay;

  uint16_t _buffer_overflow:1;

  uint8_t _receive_buffer[SS_MAX_RX_BUFF]; 
  volatile uint8_t _receive_buffer_tail;
  volatile uint8_t _receive_buffer_head;

  inline void recv();

  static inline void tunedDelay(unsigned long delay);
  static bool isActiveListener(SoftwareSerial *object);
  static bool addActiveListener(SoftwareSerial *object, int irq);
  static void removeActiveListener(SoftwareSerial *object);
  static SoftwareSerial *findActiveListener(int irq);

public:
  SoftwareSerial(uint8_t receivePin, uint8_t transmitPin);
  ~SoftwareSerial();

  void begin(long speed);
  bool listen();
  void end();
  bool isListening() { return isActiveListener(this); }
  bool stopListening();
  bool overflow() { bool ret = _buffer_overflow; if (ret) _buffer_overflow = false; return ret; }
  int peek();

  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();
  
  using Print::write;

  static inline int handle_interrupt(int irq, FAR void* context, FAR void *arg);
};

#endif
