#include "WEMOS_Matrix_LED.h"


MLED::MLED(uint8_t _intensity, byte dataPin, byte clockPin)
{
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

void MLED::display()
{

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



void MLED::dot(uint8_t x, uint8_t y, bool draw)
{
	x&=0x07;
	y&=0x07;

	if(draw)
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