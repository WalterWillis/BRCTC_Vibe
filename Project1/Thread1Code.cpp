#include "stdafx.h"

using namespace std;

void *SpiDataCollector(void*);

void GyroHeader(string file)
{
	ofstream gyroFile;
	gyroFile.open(file, ios::out | ios::app); //output and append

	string header = "Diagnostic; GyroX; GyroY; GyroZ; AccelX; AccelY; AccelZ; Temp; SampleCounter; ChecksumRead; ChecksumCalculated"
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
	//const int Gyro_DataPoints = 10; 


	////ADC SETUP
	PiHat ADC;
	PiHatHeader(ADC.fileName); // file name is in the .h file
	
	////GYRO SETUP
	//ADIS16460 Gyro;
	//GyroHeader(Gyro.fileName);	

	//Gyro.RegWrite(MSC_CTRL, 0xC1); //Tells Gryo to enable Data Ready, and sets the polarity according to the ADIS16460 example (See .cpp file) 
	//delay(20);
	//Gyro.RegWrite(FLTR_CTRL, 0x500); // Sets digital filter
	//delay(20);
	//Gyro.RegWrite(DEC_RATE, 0); //Disables decimation
	//delay(20);

	//const int pinDR = 2; // Data Ready pin on RPI


	////BEGIN
	while (timeNow - timeCut < 1800) { // Switch to if time now >= start + 30

		////ADC
		int result;
		double realResults[length][ADC_Pins];

		////GYRO
		//int16_t *burstData[length][Gyro_DataPoints];
		//int16_t burstChecksum = 0; // used to verify checksum

		////Time
		time_t timeData[length];
		

		for (int i = 0; i < length; i++)
		{
			timeData[i] = time(0) - timeCut; // get time in seconds since beginning of program
			for (int pin = 0; pin < ADC_Pins; pin++)
			{
				result = ADC.readADCChannel(pin);
				realResults[i][pin] = (double)result / (double)4095 * ADC.vref;
			}
			
			//burstData[i] = Gyro.burstRead();
			//delayMicroseconds(16); // according to the datasheet, this is the appropriate delay between readings. The ADC read process is a timing blackbox.
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

		//myfile.open("GYRO_TEST.txt", ios::out | ios::app);
		//if (myfile.is_open())
		//{
		//	for (int i = 0; i < length; i++) {
		//		for (int j = 0; j < Gyro_DataPoints; j++)
		//		{
		//			char semi = ';';
		//			if (j == 0) // Redundant with else, but 0 will always be the first selection hit. May need to be optimized.
		//				myfile << burstData[i][j] << semi;
		//			else if (j > 0 && j <= 3)
		//				myfile << Gyro.gyroScale(*(burstData[i][j])) << semi;
		//			else if (j > 3 && j <= 6)
		//				myfile << Gyro.accelScale(*(burstData[i][j])) << semi;
		//			else if (j == 7)
		//				myfile << Gyro.tempScale*(burstData[i][j])) << semi;
		//			else
		//				myfile << burstData[i][j] << semi;
		//		}
		//		myfile << Gyro.checksum(burstData[i]) << timeData[i] << endl;
		//	}			
		//}
		//myfile.flush();
		//myfile.close();

	}
	pthread_exit(NULL);
}