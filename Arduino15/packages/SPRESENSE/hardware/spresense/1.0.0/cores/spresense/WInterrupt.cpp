/*
  WInterrupt.cpp - Interrupts API implementation file for the Sparduino SDK
  Copyright (C) 2018 Sony Semiconductor Solutions Corp.
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

#include <stdbool.h>
#include <stdio.h>
#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <common/up_arch.h>
#include <arch/cxd56xx/irq.h>
#include <chip/cxd5602_topreg.h>
#include <cxd56_gpio.h>
extern "C" {
#include <cxd56_gpioint.h>
}
#include <Arduino.h>
#include "utility.h"
#include "wiring.h"
#include "wiring_private.h"

#ifndef CONFIG_CXD56_GPIO_IRQ
# error Please enable GPIO interrupt support in Nuttx
#endif // CONFIG_CXD56_GPIO_IRQ

static irqstate_t s_irq_flags;

typedef struct __interrupt_map {
    uint8_t pin;
    int irq;
    void (*isr)(void);
} interrupt_map_t;

/* irq number is allocated dynamically */
static interrupt_map_t s_irq_maps[] = {
    { PIN_UART2_RXD,     -1, 0 },
    { PIN_UART2_TXD,     -1, 0 },
    { PIN_HIF_IRQ_OUT,   -1, 0 },
    { PIN_PWM3,          -1, 0 },
    { PIN_SPI2_MOSI,     -1, 0 },
    { PIN_PWM1,          -1, 0 },
    { PIN_PWM0,          -1, 0 },
    { PIN_SPI3_CS1_X,    -1, 0 },
    { PIN_SPI2_MISO,     -1, 0 },
    { PIN_PWM2,          -1, 0 },
    { PIN_SPI4_CS_X,     -1, 0 },
    { PIN_SPI4_MOSI,     -1, 0 },
    { PIN_SPI4_MISO,     -1, 0 },
    { PIN_SPI4_SCK,      -1, 0 },
    { PIN_SPI3_SCK,      -1, 0 },
    { PIN_SPI3_MISO,     -1, 0 },
    { PIN_SPI3_MOSI,     -1, 0 },
    { PIN_SPI3_CS0_X,    -1, 0 },
    { PIN_SPI0_MOSI,     -1, 0 },
    { PIN_SPI0_MISO,     -1, 0 },
    { PIN_I2C0_BDT,      -1, 0 },
    { PIN_I2C0_BCK,      -1, 0 },
    { PIN_EMMC_DATA0,    -1, 0 },
    { PIN_EMMC_DATA1,    -1, 0 },
    { PIN_I2S0_DATA_OUT, -1, 0 },
    { PIN_I2S0_DATA_IN,  -1, 0 },
    { PIN_EMMC_DATA2,    -1, 0 },
    { PIN_EMMC_DATA3,    -1, 0 },
    { PIN_SEN_IRQ_IN,    -1, 0 },
    { PIN_EMMC_CLK,      -1, 0 },
    { PIN_EMMC_CMD,      -1, 0 },
    { PIN_I2S0_LRCK,     -1, 0 },
    { PIN_I2S0_BCK,      -1, 0 },
    { PIN_UART2_CTS,     -1, 0 },
    { PIN_UART2_RTS,     -1, 0 },
};

#define INTC_EN(n) (CXD56_INTC_BASE + 0x10 + (((n) >> 5) << 2))

static bool irq_enabled(int irq)
{
    assert(irq >= CXD56_IRQ_EXTINT);

    noInterrupts();
    irq -= CXD56_IRQ_EXTINT;
    uint32_t bit = 1 << (irq & 0x1f);
    uint32_t regval = getreg32(INTC_EN(irq));
    bool enabled = ((regval & bit) ? true : false);
    interrupts();

    return enabled;
}

static int get_slot_by_pin(uint8_t pin)
{
    arrayForEach(s_irq_maps, i) {
        if (s_irq_maps[i].pin == pin)
            return i;
    }

    return -1;
}

static int get_slot_by_irq(int irq)
{
    if (irq < 0) return -1;

    arrayForEach(s_irq_maps, i) {
        if (s_irq_maps[i].irq == irq)
            return i;
    }

    return -1;
}

static inline bool is_valid_mode(int mode) inline_function;
static inline bool is_valid_mode(int mode)
{
    return (mode == LOW || mode == HIGH || mode == CHANGE || mode == RISING || mode == FALLING);
}

static int interrupt_handler(int irq, FAR void* context, FAR void *arg)
{
    //printf("handle GPIO interrupt [%d]\n", irq);
    unuse(context);
    unuse(arg);

    int slot = get_slot_by_irq(irq);
    if (slot >= 0 && s_irq_maps[slot].isr)
        s_irq_maps[slot].isr();

    return 0;
}

static void attach_interrupt(int slot, void (*isr)(void), int mode)
{
    if (slot < 0 || !isr || !is_valid_mode(mode)) {
        printf("ERROR: Invalid arg detected. slot [%d], isr [0x%08x], mode [%d]\n", slot, isr, mode);
        return;
    }

    uint32_t _mode = mode == LOW    ? GPIOINT_LEVEL_LOW  :
                     mode == HIGH   ? GPIOINT_LEVEL_HIGH :
                     mode == CHANGE ? GPIOINT_EDGE_BOTH  :
                     mode == RISING ? GPIOINT_PSEUDO_EDGE_RISE  : GPIOINT_PSEUDO_EDGE_FALL;

    noInterrupts();
    int irq = cxd56_gpioint_config(s_irq_maps[slot].pin,
                                   GPIOINT_NOISE_FILTER_ENABLE |
                                   _mode,
                                   interrupt_handler);
    //printf("cxd56_gpioint_config returns [%d]\n", irq);

    if (irq < 0) {
        interrupts();
        printf("ERROR: Out of interrupt resources\n");
        return;
    }

    /* wait RTC few cycles before the interrupt is enabled for noise filter. */
    delay(1);
    s_irq_maps[slot].irq = irq;
    s_irq_maps[slot].isr = isr;
    cxd56_gpioint_enable(s_irq_maps[slot].pin);
    interrupts();
}

static void detach_interrupt(int slot)
{
    if (slot < 0) {
        printf("ERROR: Invalid slot [%d]\n", slot);
        return;
    }

    noInterrupts();
    cxd56_gpioint_config(s_irq_maps[slot].pin, 0, NULL);
    s_irq_maps[slot].irq = -1;
    s_irq_maps[slot].isr = 0;
    interrupts();
}

extern "C" {
void interrupts(void)
{
    leave_critical_section(s_irq_flags);
}

void noInterrupts(void)
{
    s_irq_flags = enter_critical_section();
}

uint16_t irq_save(uint16_t mask)
{
    uint16_t flags = 0;
    for (int i = CXD56_IRQ_EXDEVICE_0; i <= CXD56_IRQ_EXDEVICE_11; ++i) {
        int b = i - CXD56_IRQ_EXDEVICE_0;
        if (irq_enabled(i)) {
            bitSet(flags, b);
            if (bitRead(mask, b))
                up_disable_irq(i);
        }
    }

    return flags;
}

void irq_restore(uint16_t flags)
{
    for (int i = CXD56_IRQ_EXDEVICE_0; i <= CXD56_IRQ_EXDEVICE_11; ++i) {
        if (bitRead(flags, i - CXD56_IRQ_EXDEVICE_0))
            up_enable_irq(i);
    }
}
} // extern "C"

void attachInterrupt(uint8_t interrupt, void (*isr)(void), int mode)
{
    uint8_t pin = pin_convert(interrupt);
    int slot = get_slot_by_pin(pin);
    attach_interrupt(slot, isr, mode);
}

void detachInterrupt(uint8_t interrupt)
{
    uint8_t pin = pin_convert(interrupt);
    int slot = get_slot_by_pin(pin);
    detach_interrupt(slot);
}
