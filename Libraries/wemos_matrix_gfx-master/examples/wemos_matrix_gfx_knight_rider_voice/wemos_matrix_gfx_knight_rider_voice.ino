// LeRoy Miller, 2017

#include <Adafruit_GFX.h>
#include <WEMOS_Matrix_GFX.h>

MLED matrix(7); //set intensity=7 (maximum)

void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  
 
}

static const uint8_t PROGMEM
  large_bmp[] =
  { B00000000,
    B00011000,
    B00111100,
    B00111100,
    B00111100,
    B00111100,
    B00011000,
    B00000000 },
  med_bmp[] =
  { B00000000,
    B00000000,
    B00011000,
    B00111100,
    B00111100,
    B00011000,
    B00000000,
    B00000000 },
    empty_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000 },
  small_bmp[] =
  { B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000 };

    

void loop() {
  matrix.clear();
  matrix.drawBitmap(0, 0, empty_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);
  
  matrix.clear();
  matrix.drawBitmap(0, 0, small_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);

  matrix.clear();
  matrix.drawBitmap(0, 0, med_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);

  matrix.clear();
  matrix.drawBitmap(0, 0, large_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);
  
  matrix.clear();
  matrix.drawBitmap(0, 0, med_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);
  
  matrix.clear();
  matrix.drawBitmap(0, 0, small_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);
  
  matrix.clear();
  matrix.drawBitmap(0, 0, empty_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(75);
  
}
