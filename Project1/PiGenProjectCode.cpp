// PiGenProjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "Thread1Code.h" //Code for First thread
#include "Thread2Code.h" //Code for Second Thread
#include <stdlib.h> //Library for system functions

using namespace std;

//// Globals
// Ints 
const int adc0Input = 100; 

// Strings
string MFile = "Master_Program_Data.txt";
char devID0 = 0x48; // GND to ADDR (ADC)

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

		////Setup Piezo
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC) Setup Started..." << systime << endl;
		ads1115SetupManual(adc0Input, devID0); //manual is 860SPS with gain of 2 V, changed from default 128 SPS with gain of 4V
		pthread_create(&threads[0], NULL, PiezoDataCollector, NULL);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC) Setup Complete..." << systime << endl;


		//////Setup Gyro
		//timeNow = time(0);
		//systime = ctime(&timeNow);
		//data_ << "\tGyro Setup Started..." << systime << endl;
		//pthread_create(&threads[1], NULL, GyroDataCollector, NULL);
		//timeNow = time(0);
		//systime = ctime(&timeNow);
		//data_ << "\tGyro Setup Complete..." << systime << endl;

		//timeNow = time(0);
		//systime = ctime(&timeNow);
		//data_ << "System Device Setup Complete..." << systime << endl;

		data_.flush();
		data_.close();
	}
	return 0;
}






int main()
{
	int rc = Startup();
	time_t timeNow = time(0);
	time_t timeCut = time(0);
	string begin_time;
	char* systime = ctime(&timeNow);
	begin_time = systime;

	if (rc != 0) {
		system("sudo reboot");
	}
	else if (rc == 0)
	{
		while (timeNow - timeCut < 1801) //Change to Start Time + 30
		{
			timeNow = time(0);
		}
		system("sudo shutdown now");
		return 0;
	}
}
