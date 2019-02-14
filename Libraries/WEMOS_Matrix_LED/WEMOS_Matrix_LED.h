
#ifndef __WEMOS_Matrix_LED_H
#define __WEMOS_Matrix_LED_H


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif



class MLED
{
	public:
		MLED(uint8_t _intensity=4, byte dataPin=D7, byte clockPin=D5);
		void display();
		void clear();
		void dot(uint8_t x, uint8_t y, bool draw=1);


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

