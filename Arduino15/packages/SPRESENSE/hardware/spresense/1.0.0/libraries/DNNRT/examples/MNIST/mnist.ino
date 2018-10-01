#include <NetPBM.h>
#include <DNNRT.h>
#include <SDHCI.h>

DNNRT dnnrt;

void setup() {

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
