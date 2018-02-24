#include "stdafx.h"
//#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "PiHat.h"
PiHat::PiHat()
{
	(wiringPiSPISetupMode(_cs, _speed, _mode) < 0);
}

PiHat::PiHat(int cs, int speed, int mode)
{
	_cs = cs;
	_speed = speed;
	_mode = mode;
	(wiringPiSPISetupMode(_cs, _speed, _mode) < 0);
}

int PiHat::readADCChannel(int pin) {
	unsigned char cntrlStack[3];
	unsigned char tmp;
	unsigned int result;

	/* figure 6-1 of MCP3208 datasheet */
	cntrlStack[0] = 6 + ((pin & 4) >> 2);
	cntrlStack[1] = (pin & 3) << 6;
	cntrlStack[2] = 0;

	wiringPiSPIDataRW(_cs, cntrlStack, 3);

	return ((cntrlStack[1] & 15) << 8) + cntrlStack[2];
}
