// PiGenProjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "Thread1Code.h" //Code for First thread
#include "Thread2Code.h" //Code for Second Thread
#include <stdlib.h> //Library for system functions

using namespace std;

//// Globals
////ADS1115
//const int adc0Input = 100; 
//char devID0 = 0x48; // GND to ADDR (ADC)

// Strings
string MFile = "Master_Program_Data.txt";

// Data Stream
ofstream data_;

int Startup() {
	//Thread Count
	pthread_t threads[1];
	
	//Create Master File
	time_t timeNow = time(0);
	char* systime = ctime(&timeNow);
	data_.open(MFile);

	if (data_.is_open()) {
		data_ << "Master Data File Created..." << systime << endl;
		data_ << "----------------------------------------" << endl;

		//Setup System Devices
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "System Device Setup Started..." << systime << endl;

		////Setup Wiring Pi
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tWiring Pi Setup Started..." << systime << endl;
		wiringPiSetup();
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tWiring Pi Setup Complete..." << systime << endl;

/*
		////Setup I2C ADC Device
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC) Setup Started..." << systime << endl;
		ads1115SetupManual(adc0Input, devID0); //manual is 860SPS with gain of 2 V, changed from default 128 SPS with gain of 4V
		pthread_create(&threads[0], NULL, PiezoDataCollector, NULL);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC) Setup Complete..." << systime << endl;
*/

		////Setup SPI Devices
		/*timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tGyro Setup Started..." << systime << endl;
		pthread_create(&threads[1], NULL, SpiDataCollector, NULL);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tSPI Setup Complete..." << systime << endl;

		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "System Device Setup Complete..." << systime << endl;

		data_.flush();
		data_.close();*/
	}
	return 0;
}






int main()
{
	ADIS16460 Gyro = ADIS16460(1, 1000000, 3);
	delay(1);

	while (true) {
		int16_t* results = Gyro.burstRead();
		//cout << "\nArray: " << results;

		int16_t checksumExp = Gyro.checksum(results);
		int16_t checksumReal = Gyro.checksum(results[9]);

		//cout << "\nChecksum Expected: " << checksumExp << "\nChecksum Reality: " << results[9] << "\nIs match: " << (results[9] == checksumExp);

		if(true){//checksumExp == results[9]){
			cout << "\nGyro X: " << Gyro.gyroScale(results[1]);
			cout << "\nGyro Y: " << Gyro.gyroScale(results[2]);
			cout << "\nGyro Z: " << Gyro.gyroScale(results[3]);

			cout << "\nAccel X: " << Gyro.accelScale(results[4]);
			cout << "\nAccel Y: " << Gyro.accelScale(results[5]);
			cout << "\nAccel Z: " << Gyro.accelScale(results[6]);

			cout << "\nTemp: " << Gyro.tempScale(results[7]);

			cout << "\nSample Rate: " << Gyro.tempScale(results[8]);
		}
		delay(20);
	}
}
