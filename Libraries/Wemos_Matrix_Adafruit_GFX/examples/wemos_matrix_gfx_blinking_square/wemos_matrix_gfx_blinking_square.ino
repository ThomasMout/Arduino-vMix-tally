// LeRoy Miller, 2017
#include <Adafruit_GFX.h>
#include <WEMOS_Matrix_GFX.h>

MLED matrix(7); //set intensity=7 (maximum)

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
 
}
void loop() {
 
 matrix.clear();
  matrix.drawRect(0,0, 8,8, LED_ON);
  matrix.fillRect(2,2, 4,4, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);
matrix.drawRect(0,0,8,8, LED_OFF);
matrix.drawRect(1,1, 6,6, LED_ON);
matrix.fillRect(2,2,4,4, LED_OFF);
matrix.writeDisplay();
delay(500);
}
