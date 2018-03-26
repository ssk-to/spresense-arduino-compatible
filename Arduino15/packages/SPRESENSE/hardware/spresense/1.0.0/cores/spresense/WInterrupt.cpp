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

/*
 following is copied from cxd56_gpioint.c
 slot used below indicates index in g_isr[MAX_SLOT] in cxd56_gpioint.c
 not index in s_irq_maps in this file
 */
/* ==================CXD56_GPIOINT.C START================== */
#define MAX_SLOT                (12)
#define MAX_SYS_SLOT            (6)

#define GET_SLOT2IRQ(slot)      (CXD56_IRQ_EXDEVICE_0 + (slot))
#define GET_IRQ2SLOT(irq)       ((irq) - CXD56_IRQ_EXDEVICE_0)

/* convert from slot to pin */
static int get_slot2pin(int slot)
{
    uint32_t base = (slot < MAX_SYS_SLOT) ? CXD56_TOPREG_IOCSYS_INTSEL0
                                          : CXD56_TOPREG_IOCAPP_INTSEL0;
    int offset = 1;

    if (MAX_SYS_SLOT <= slot)
    {
        slot -= MAX_SYS_SLOT;
        offset = 56;
    }

    return (int)getreg8(base + slot) + offset;
}

/* convert from pin to slot number (SYS: 0~5, APP: 6~11) */
static int get_pin2slot(int pin)
{
    int slot;
    uint32_t base = (pin < PIN_IS_CLK) ? CXD56_TOPREG_IOCSYS_INTSEL0
                                       : CXD56_TOPREG_IOCAPP_INTSEL0;
    int offset = (pin < PIN_IS_CLK) ? 1 : 56;

    for (slot = 0; slot < MAX_SYS_SLOT; slot++)
    {
        if ((pin - offset) == getreg8(base + slot)) /* byte access */
        {
            break;
        }
    }

    if (slot == MAX_SYS_SLOT)
    {
        return -1;
    }

    if (PIN_IS_CLK <= pin)
    {
        slot += MAX_SYS_SLOT;
    }

    return slot;
}

/* convert from pin to irq number */
static int get_pin2irq(int pin)
{
    int slot = get_pin2slot(pin);

    if ((0 <= slot) && (slot < MAX_SLOT))
        return GET_SLOT2IRQ(slot);

    printf("WARNING: There's no irq number assigned to this pin currently\n");
    return -1;
}

/* convert from irq number to pin */
static int get_irq2pin(int irq)
{
    int slot = GET_IRQ2SLOT(irq);
    return get_slot2pin(slot);
}
/* ==================CXD56_GPIOINT.C END================== */

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
    /* Known Issue: edge trigger not right */
    //printf("handle GPIO interrupt [%d]\n", irq);
    unuse(context);
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

    if (irq >= 0) {
        s_irq_maps[slot].irq = irq;
        s_irq_maps[slot].isr = isr;
        cxd56_gpioint_enable(s_irq_maps[slot].pin);
    }
    interrupts();
}

static void detach_interrupt(int slot)
{
    if (slot < 0) {
        printf("ERROR: Invalid slot [%d]\n", slot);
        return;
    }

    noInterrupts();
    cxd56_gpioint_disable(s_irq_maps[slot].pin);
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

int digitalPinToInterrupt(uint8_t pin)
{
    pin = pin_convert(pin);
    if (pin == PIN_NOT_ASSIGNED)
        return -1;
    int irq = get_pin2irq(pin);
    if (irq < 0) {
        printf("WARNING: Please call digitalPinToInterrupt() after attachInterrupt()\n");
    }

    return irq;
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

void attachInterrupt(uint8_t pin, void (*isr)(void), int mode)
{
    pin = pin_convert(pin);
    int slot = get_slot_by_pin(pin);
    attach_interrupt(slot, isr, mode);
}

void attachInterrupt(int irq, void (*isr)(void), int mode)
{
    unuse(irq);
    unuse(isr);
    unuse(mode);
    printf("WARNING: Spritzer DOES NOT support attaching to interrupt by irq number\n"
           "         Because irq number is assigned dynamically for pin\n"
           "         Please use \"void attachInterrupt(uint8_t pin, void (*isr)(void), int mode)\" instead\n");
}

void detachInterrupt(uint8_t pin)
{
    pin = pin_convert(pin);
    int slot = get_slot_by_pin(pin);
    detach_interrupt(slot);
}

void detachInterrupt(int irq)
{
    int pin = get_irq2pin(irq);
    int slot = get_slot_by_pin(pin);
    detach_interrupt(slot);
}
