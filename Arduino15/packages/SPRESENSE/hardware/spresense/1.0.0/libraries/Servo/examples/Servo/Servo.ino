/*
 *  Servo.ino - Servo sample application
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Servo.h>

static Servo s_servo;

void setup() {
  // put your setup code here, to run once:
  s_servo.attach(PIN_D09);
  s_servo.write(90);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  s_servo.write(0);
  delay(1000);
  s_servo.write(180);
  delay(1000);
}
