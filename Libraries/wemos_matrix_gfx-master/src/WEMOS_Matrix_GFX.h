
#ifndef __WEMOS_Matrix_GFX_H
#define __WEMOS_Matrix_GFX_H


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Adafruit_GFX.h"

#define LED_ON 1
#define LED_OFF 0

class MLED :  public Adafruit_GFX 
{
	public:
		MLED(uint8_t _intensity=4, byte dataPin=D7, byte clockPin=D5);
		void writeDisplay();
		void clear();
		void drawPixel(int16_t  x, int16_t y, uint16_t color);
		


	    volatile uint8_t disBuffer[8]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	    uint8_t intensity;
	    

	protected:
		void sendCommand(byte led);
	    void sendData(byte add, byte data);
	    void send(byte data);

	    byte dataPin;
	    byte clockPin;

	    

};
	

#endif

