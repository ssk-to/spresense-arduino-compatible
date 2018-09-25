#include <DNNRT.h>

DNNRT dnnrt;

void setup() {
  File nnb("network.nnb");       // Read network from SD card
  dnnrt.begin(nnb);              // Initialize DNN runtime with network (.nnb) file

  File input("9.pgm");          // Read image for input

  dnnrt.inputVariable(input, 0); // Set image to runtime
  dnnrt.forward();               // Forward propagation

  DNNVariable output = dnnrt.outputVariable(0); // Take results

  // Search and show recognized result
  // MNIST network outputs float array of 10, each elements represents 0 to 9.

  for (int i = 0; i < output.size(); i++) {

    if (float(output[i]) > 0.8) {
      Serial.print("Image is number ");
      Serial.print(i);
      Serial.println();
      break;
    }
  }
}

void loop() {

}
