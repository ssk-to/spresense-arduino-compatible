/*
  leds.c - LED control for the Sparduino SDK
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

#include <cxd56_pmic.h>
#include <arch/board/board.h>
#include "leds.h"

#ifdef TARGET_USES_PROTO3_BOARD
// This will be deleted in the near future
static int getChanel(int led)
{
    int chanel = -1;

    switch(led)
    {
        case PIN_LED0:
            chanel = PMIC_GPO(0);
            break;
        case PIN_LED1:
            chanel = PMIC_GPO(1);
            break;
        case PIN_LED2:
            chanel = PMIC_GPO(2);
            break;
        case PIN_LED3:
            chanel = PMIC_GPO(3);
            break;
        default:
            break;
    }
    assert((chanel != (-1)));
    return chanel;
}
#endif

void Led_On(int ledch)
{
#ifdef TARGET_USES_PROTO3_BOARD
    // This will be deleted in the near future
    int ch = getChanel(ledch);
    board_power_control(ch, false);
#else
    pinMode(ledch, OUTPUT);
    digitalWrite(ledch, HIGH);
#endif
}

void Led_Off(int ledch)
{
#ifdef TARGET_USES_PROTO3_BOARD
    // This will be deleted in the near future
    int ch = getChanel(ledch);
    board_power_control(ch, true);
#else
    pinMode(ledch, OUTPUT);
    digitalWrite(ledch, LOW);
#endif
}
