#include "stdafx.h"

using namespace std;

void *SpiDataCollector(void*);

void GyroHeader(string file)
{
	ofstream gyroFile;
	gyroFile.open(file, ios::out | ios::app); //output and append

	string header = "Supply Voltage; Accel X; Accel Y; Accel Z; Temperature; Gyro Roll; Gyro Pitch; Gyro; Aux ADC; Time; Supply Byte 1; Supply Byte 2; gyro Byte 1; Gyro Byte 2; AccelX Byte1; AccelX Byte2; AccelY Byte 1; AccelY Byte 2; AccelZ Byte 1; AccelZ Byte 2; Temp Byte 1; Temp Byte 2; Pitch Byte 1; Pitch Byte 2; Roll Byte 1; Roll Byte 2; Aux Byte 1; Aux Byte 2";//Piezo 0 and 3 are Y, 1 and 4 are X, 2 and 5 are Z. 3-5 are used on the multiplexer.

	if (gyroFile.is_open())
	{
		gyroFile << header << endl;
		//cout << header << endl;
		gyroFile.flush();
		gyroFile.close();
	}
}

void PiHatHeader(string file)
{
	ofstream AdcFile;
	AdcFile.open(file, ios::out | ios::app); //output and append

	string header = "Supply Voltage;Pizeo X;Pizeo Y;Pizeo Z;Time";

	if (AdcFile.is_open())
	{
		AdcFile << header << endl;
		//cout << header << endl;
		AdcFile.flush();
		AdcFile.close();
	}
}

void *SpiDataCollector(void *unused) { //import time now and start + 30
	time_t timeNow = time(0);
	time_t timeCut = time(0);
	string SPI_TIME = ctime(&timeNow);

	const int length = 30000;
	const int ADC_Pins = 3;

	PiHat ADC;
	PiHatHeader(ADC.fileName);
	//ADIS16460 Gyro;
	//Gyro.ClearBuffer();
	//GyroHeader(Gyro.fileName);	// file name is in the .h file

	while (timeNow - timeCut < 1800) { // Switch to if time now >= start + 30

		int result;
		double realResults[length][ADC_Pins];
		time_t timeData[length];
		/*							   
		Gyro.GetADISReadings();
		delayMicroseconds(100); // More than enough time for SPI communication. Less could drop performance: http://wiringpi.com/reference/timing/
		
		ofstream gyroFile;
		gyroFile.open(Gyro.fileName, ios::out | ios::app); //output and append

		if (gyroFile.is_open()) {
			timeNow = time(0);
			systime = ctime(&timeNow);
			gyroFile << Gyro.supply << ";";
			gyroFile << Gyro.accelx << ";";
			gyroFile << Gyro.accely << ";";
			gyroFile << Gyro.accelz << ";";
			gyroFile << Gyro.temp << ";";
			gyroFile << Gyro.roll << ";";
			gyroFile << Gyro.pitch << ";";
			gyroFile << Gyro.gyro << ";";
			gyroFile << Gyro.aux_ADC << ";";
			for(int i = 0; i < 18; i++)
			{
				int data = Gyro.buffer[i];
				gyroFile << data << ";";
			}
			gyroFile << systime;
			gyroFile.flush();
			gyroFile.close();
		}

		Gyro.ClearBuffer();
		delayMicroseconds(100);
		*/

		for (int i = 0; i < length; i++)
		{
			for (int pin = 0; pin < ADC_Pins; pin++)
			{
				result = ADC.readADCChannel(pin);
				realResults[i][pin] = (double)result / (double)4095 * ADC.vref;
			}
			timeData[i] = time(0) - timeCut; // get time in seconds since beginning of program
		}

		ofstream myfile;
		myfile.open("ADC_TEST.txt", ios::out | ios::app);

		for (int i = 0; i < length; i++) {
			if (myfile.is_open())
			{
				myfile << ADC.vref << ";" << realResults[i][0] << ";" << realResults[i][1] << ";" << realResults[i][2] << ";" << timeData[i] << endl;
			}
		}

		myfile.flush();
		myfile.close();

	}
	pthread_exit(NULL);
}