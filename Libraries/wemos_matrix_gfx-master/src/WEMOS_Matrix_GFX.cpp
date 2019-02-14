#include "WEMOS_Matrix_GFX.h"


MLED::MLED(uint8_t _intensity, byte dataPin, byte clockPin) : Adafruit_GFX(8, 8) {
	this->dataPin = dataPin;
	this->clockPin = clockPin;

	if(_intensity>7)
		intensity=7;
	else
		intensity=_intensity;

	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	
	digitalWrite(dataPin, HIGH);
	digitalWrite(clockPin, HIGH);

}

void MLED::writeDisplay() {

	for(uint8_t i=0;i<8;i++)
	{
		sendData(i,disBuffer[i]);

		digitalWrite(dataPin, LOW);
  		digitalWrite(clockPin, LOW);
  		digitalWrite(clockPin, HIGH);
  		digitalWrite(dataPin, HIGH);
	}

	sendCommand(0x88|(intensity));
}

void MLED::clear()
{
	for(uint8_t i=0;i<8;i++)
	{
		disBuffer[i]=0x00;
	}

}



void MLED::drawPixel(int16_t  x, int16_t y, uint16_t color)
{
	if ((y < 0) || (y >= 8)) return;
  	if ((x < 0) || (x >= 8)) return;

  	y = 7 - y;

	if(color)
	{
		disBuffer[y]|=(1<<x);
	}
	else
	{
		disBuffer[y]&=~(1<<x);
	}

}

void MLED::sendCommand(byte cmd)
{
  digitalWrite(dataPin, LOW);
  send(cmd);
  digitalWrite(dataPin, HIGH);
}

void MLED::sendData(byte address, byte data)
{
  sendCommand(0x44);
  digitalWrite(dataPin, LOW);
  send(0xC0 | address);
  send(data);
  digitalWrite(dataPin, HIGH);
}

void MLED::send(byte data)
{
  for (int i = 0; i < 8; i++) {
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, data & 1 ? HIGH : LOW);
    data >>= 1;
    digitalWrite(clockPin, HIGH);
  }
}