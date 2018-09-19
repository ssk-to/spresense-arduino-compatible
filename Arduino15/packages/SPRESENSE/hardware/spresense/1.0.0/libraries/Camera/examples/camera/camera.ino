/*
 *  camera.ino - One minute interval time-lapse Camera
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
 *
 *  This is a test app for the camera library.
 *  This library can only be used on the Spresense with the FCBGA chip package.
 */

#include <SDHCI.h>

#include <Camera.h>
#include <arch/board/board.h>

#define BAUDRATE     (115200)
#define FILENAME_LEN (14)

SDClass theSD;
uint32_t capture_cnt = 0;
char filename[FILENAME_LEN] = {0};

/**
 * @brief Callback from Camera library
 */

void CamCB(CamImage img)
{
  uint8_t *buf;
  size_t img_size;

  buf = img.getImgBuff();
  img_size = img.getImgSize();

  /* Save to SD card */
  sprintf(filename, "VIDEO%03d.YUV", capture_cnt);
  File myFile = theSD.open(filename, FILE_WRITE);
  myFile.write(buf, img_size);
  myFile.close();
  
  capture_cnt++;
}

/**
 * @brief Initialize camera
 */
void setup() {
  /* Open serial communications and wait for port to open */
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB port only */
  }

  Serial.print("Prepare camera\n");
  theCamera.begin(CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V, CAM_VIDEO_FPS_60);
  Serial.print("Start streaming");
  theCamera.startStreaming(true, CamCB);
}

/**
 * @brief No procedure
 */
void loop() {
  usleep(1);
}
