#include "UART.h"
#include "stdafx.h"

UART::UART() {
	//get the file descriptor
	if ((fd = serialOpen("/dev/serial0", 57600)) < 0)
	{
		isValid = false;
	}
	else {
		isValid = true;
	}
}

void UART::SendData(list<list<double>> dataList) {
	for (list<double> data : dataList) {
		string str;

		for (double d : data) {
			str = std::to_string(d) + semi;			
		}

		str += "\n";
		serialPrintf(fd, str.c_str());		
	}
	serialFlush(fd);
}