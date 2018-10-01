/*
 *  mnist.ino - hand written number recognition sample application
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

/**
 * @file mnist.ino
 * @author Sony Semiconductor Solutions Corporation
 * @brief %DNNRT sample application.
 */

#include <SDHCI.h>
#include <NetPBM.h>
#include <DNNRT.h>

DNNRT dnnrt;

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  File nnbfile("lenet-5/data/lenet-5.nnb");
  dnnrt.begin(nnbfile);

  File pgmfile("lenet-5/data/1.pgm");
  NetPBM pgm(pgmfile);

  unsigned short width, height;
  pgm.size(&width, &height);

  DNNVariable input(width * height);
  float *buf = input.data();

  // Normalize pixel data into between 0.0 and 1.0.
  // PGM file is gray scale pixel map, so divide by 255.
  // This normalization depends on the network.

  for (int x = 0; x < height; x++) {
    for (int y = 0; y < width; y++) {
      *buf = float(pgm.getpixel(x, y)) / 255.0;
      buf++;
    }
  }

  dnnrt.inputVariable(input, 0);
  dnnrt.forward();
  DNNVariable output = dnnrt.outputVariable(0);

  for (unsigned int i = 0; i < output.size(); i++) {
    if (output[i] > 0.8) {
      Serial.print("Recognize ");
      Serial.print(i);
      Serial.println();
      break;
    }
  }

  dnnrt.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}
