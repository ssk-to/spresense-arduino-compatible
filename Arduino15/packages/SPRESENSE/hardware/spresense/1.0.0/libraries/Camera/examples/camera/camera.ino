/*
 *  One minute interval time-lapse Camera
 *
 *  This is a test app for the camera library.
 *  This library can only be used on the SPRESENSE with the FCBGA chip package.
 */

#include <SDHCI.h>

#include <Camera.h>
#include <arch/board/board.h>

SDClass theSD;

CameraClass theCamera;

void setup() {
  puts("Prepare camera");
  theCamera.begin();
  theCamera.initialize();
}

void loop() {
  static int  cnt = 0;
  static char filename[14] = {0};

  if (cnt > 999)
    {
      printf("Saved %d JPEG pictures\n", cnt);
      printf("Camera exit\n");
      exit(1);
    }

  sprintf(filename, "camera%03d.jpg", cnt);
  File myFile = theSD.open(filename, FILE_WRITE);

  printf("Requesting JPEG capture and save picture to SD card. Filename: %s\n", filename);
  theCamera.read(myFile);

  cnt++;

  /* One minute interval. */
  sleep(60);
}
