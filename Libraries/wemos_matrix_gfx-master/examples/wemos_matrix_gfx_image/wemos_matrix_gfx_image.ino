
#include <Adafruit_GFX.h>
#include <WEMOS_Matrix_GFX.h>

MLED matrix(7); //set intensity=7 (maximum)

const unsigned char myImage [] PROGMEM = {
  0xff, 0xff, 0xbd, 0xff,0xff, 0xbd, 0xc3, 0xff
};

void setup() {
 

 
}


void loop() {
  matrix.clear();
  matrix.drawBitmap(0, 0, myImage , 8, 8, LED_ON);
  matrix.writeDisplay();

  
}
