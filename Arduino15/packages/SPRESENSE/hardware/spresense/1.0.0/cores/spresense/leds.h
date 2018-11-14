/*
  leds.h - LED control include file for the Spresense SDK
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

#ifndef Leds_h
#define Leds_h

#include <Arduino.h>
#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* LED control */
#define ledOn(x)  digitalWrite(x, HIGH)
#define ledOff(x) digitalWrite(x, LOW)

#ifdef __cplusplus
}
#endif

#endif // Leds_h
