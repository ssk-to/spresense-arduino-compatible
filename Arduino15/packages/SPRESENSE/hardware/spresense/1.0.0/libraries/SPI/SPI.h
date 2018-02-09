/*
  SPI.h - SPI include file for the Sparduino SDK
  Copyright (c) 2017 Sony Corporation  All right reserved.

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

#ifndef Spi_h
#define Spi_h

/*
  This header file maybe inclued in plain C file.
  To avoid compiling error all C++ stuff should be ignored
 */
#ifdef __cplusplus

#include <nuttx/config.h>
#include <nuttx/spi/spi.h>
#include <Arduino.h>

#define SPI_MODE0 SPIDEV_MODE0
#define SPI_MODE1 SPIDEV_MODE1
#define SPI_MODE2 SPIDEV_MODE2
#define SPI_MODE3 SPIDEV_MODE3

enum SpiInterruptMode {
    SPI_INT_MODE_NONE = 0,
    SPI_INT_MODE_MASK,
    SPI_INT_MODE_GLOBAL
};

class SPISettings {
public:
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
        : clock_(clock), bit_order_(bitOrder), data_mode_(dataMode) { }
    SPISettings()
        : clock_(4000000), bit_order_(MSBFIRST), data_mode_(SPI_MODE0) { }

private:
    friend class SPIClass;
    uint32_t clock_;
    uint8_t bit_order_;
    uint8_t data_mode_;
};

class SPIClass {
public:
    // Create SPI object
    SPIClass(int port);

    // Initialize the SPI library
    void begin(void);

    // Disable the SPI bus
    void end(void);

    // Before using SPI.transfer() or asserting chip select pins,
    // this function is used to gain exclusive access to the SPI bus
    // and configure the correct settings.
    void beginTransaction(SPISettings settings);

    // After performing a group of transfers and releasing the chip select
    // signal, this function allows others to access the SPI bus
    void endTransaction(void);

    // This function is deprecated.  New applications should use
    // beginTransaction() to configure SPI settings.
    void setBitOrder(uint8_t bitOrder);

    // This function is deprecated.  New applications should use
    // beginTransaction() to configure SPI settings.
    void setDataMode(uint8_t dataMode);

    // This function is deprecated.  New applications should use
    // beginTransaction() to configure SPI settings.
    void setClockDivider(uint8_t clockDiv);

    // If SPI is used from within an interrupt, this function registers
    // that interrupt with the SPI library, so beginTransaction() can
    // prevent conflicts.  The input interruptNumber is the number used
    // with attachInterrupt.  If SPI is used from a different interrupt
    // (eg, a timer), interruptNumber should be 255.
    void usingInterrupt(uint8_t interruptNumber);
    // And this does the opposite.
    void notUsingInterrupt(uint8_t interruptNumber);
    // Note: the usingInterrupt and notUsingInterrupt functions should
    // not to be called from ISR context or inside a transaction.
    // For details see:
    // https://github.com/arduino/Arduino/pull/2381
    // https://github.com/arduino/Arduino/pull/2449

    // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
    uint8_t transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    void transfer(void *buf, size_t count);

private:
    int spi_port;
    uint8_t ref_count;
    FAR struct spi_dev_s* spi_dev;
    uint32_t spi_base_clock;
    uint8_t spi_bit_order;

    uint8_t spi_transmitting;

    // mode : 0=none, 1=mask, 2=global
    SpiInterruptMode interrupt_mode;

    // mask : which interrupts to mask
    // 0 indicates bit not used
    // E0 indicates CXD56_IRQ_EXDEVICE_0 + 0
    // E1 indicates CXD56_IRQ_EXDEVICE_0 + 1
    // ...
    // E11 indicates CXD56_IRQ_EXDEVICE_0 + 11
    // bit 15 14 13 12 11  10  09 08 07 06 05 04 03 02 01 00
    //     0  0  0  0  E11 E10 E9 E8 E7 E6 E5 E4 E3 E2 E1 E0
    uint16_t interrupt_mask;

    // temp storage, to restore state
    // (in case some interrupts are disabled before calling usingInterrupt)
    uint16_t interrupt_save;
};

#if defined(CONFIG_CXD56_SPI4) && defined(CONFIG_SPI_EXCHANGE)
extern SPIClass SPI4;

#ifndef IS_INCLUDED_BY_SPI_CPP
extern SPIClass SPI;
#endif

#else
#error Please enable SPI4 and SPI_EXCHANGE in Nuttx
#endif

#if defined(CONFIG_CXD56_SPI5) && defined(CONFIG_SPI_EXCHANGE)
extern SPIClass SPI5;
#endif


#endif // __cplusplus
#endif // Spi_h
