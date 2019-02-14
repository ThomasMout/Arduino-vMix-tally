#include <WEMOS_Matrix_GFX.h>

MLED matrix(5); //set intensity=5

#include "Flake.h"
#define NUM_FLAKES 10
Flake snow[NUM_FLAKES];

void setup() {
  // put your setup code here, to run once:

  
}

void loop() {

  matrix.clear();
  for ( int i=0; i < NUM_FLAKES ; i++ ) {
    snow[i].loop();
    int x = floor(snow[i].x);
    int y = floor(snow[i].y);
    matrix.drawPixel(x,y,1); // draw dot
  }
  matrix.writeDisplay();
  delay(50);
  
}
