/*
 *  sdhci_api.ino - SD Card sample application
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
 * @file sdhci_api.ino
 * @author Sony Corporation
 * @brief SD Card sample application.
 */

#include <SDHCI.h>

SDClass SD;  /**< SDClass object */ 

/**
 * @brief Write to the file and read from the file.
 * 
 * @details The file is located on the SD card.
 * @note Since SDK 2.X SDHCI driver has bugs so SD writing is impossible right 
 *       now. So please prepare a "sd_test.txt" file in SD card in advance.
 *       The content of the file should be "hello\r\n" (no quotation marks).
 */
void setup() {
  printf("Since SDK 2.X SDHCI driver has bugs so SD writing is impossible right now.\n");
  printf("So please prepare a \"sd_test.txt\" file in SD card in advance.\n");
  printf("The content of the file should be \"hello\\r\\n\" (no quotation marks).\n");

  assert(SD.exists("sd_test.txt"));
  File f = SD.open("sd_test.txt", FILE_READ);

  assert(strcmp(f.name(), "sd_test.txt") == 0);
  assert((bool)f);
  assert(!f.isDirectory());
  //f.write("hello", 5);
  //f.write('\n');
  f.flush();

  assert(f.seek(0));
  assert(f.position() == 0);
  
  assert(f.size() == 7);
  
  assert(f.seek(0));
  assert(f.available() == 7);
  assert(f.read() == 'h');
  assert(f.peek() == 'e');
  assert(f.read() == 'e');
  assert(f.available() == 5);
  assert(f.position() == 2);

  File none = f.openNextFile();
  assert(!(bool)none);
  
  f.close();

  printf("SDHCI library read only mode OK!\n");
}

/**
 * @brief Run repeatedly.
 * 
 * @details Does not do anything.
 */
void loop() {

}
