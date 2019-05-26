#include "stdafx.h"

#ifndef PiHat_h
#define PiHat_h
using namespace std;
class PiHat 
{

	private:
		//WiringPi Pin Scheme
		int _mode = 1; // SPI Mode
		int _speed = 1000000; // frequency of signal
		int _cs = 0;// CE0 on WiringPi

	public:
		const string fileName = "PiHat.txt";
		double vref = 3.3; // jumper selected: 5.0 (default), 3.3, 1.0, or 0.3 Volts

		PiHat();
		PiHat(int channel, int speed, int mode);
		int readADCChannel(int pin);
};
#endif