// PiGenProjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Thread1Code.h" //Code for First thread
#include "Thread2Code.h" //Code for Second Thread
#include <stdlib.h> //Library for system functions

using namespace std;

//// Globals
// Ints 
const int adc0Input = 100; 
const int adc1Input0 = 120; // Connect common (Z) to A0 (analog input)
const int adc1Input1 = 121;
const int adc1Input2 = 122;
// Strings
string MFile = "Master_Program_Data";
char devID0 = 0x48; // GND to ADDR (ADC0)
char devID1 = 0x49; // 3.3v to ADDR (ADC1)

// Data Stream
ofstream data_;

int Startup() {
	//Thread Count
	pthread_t threads[2];
	
	//Create Master File
	time_t timeNow = time(0);
	char* systime = ctime(&timeNow);
	MFile = MFile + ".txt";
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

		////Setup ADC 0 - Harvesters
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC0) Setup Started..." << systime << endl;
		ads1115Setup(adc0Input, devID0);
		pthread_create(&threads[0], NULL, HarvestorDataCollector, NULL);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC0) Setup Complete..." << systime << endl;

		////Setup the ADC 1 - Multiplexer
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC1) Setup Started..." << systime << endl;
		ads1115Setup(adc1Input, devID1);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tADS1115 (ADC1) Setup Complete..." << systime << endl;

		////Setup Gyro
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tGyro Setup Started..." << systime << endl;
		pthread_create(&threads[1], NULL, GyroDataCollector, NULL);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tGyro Setup Complete..." << systime << endl;

		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "System Device Setup Complete..." << systime << endl;

		data_.flush();
		data_.close();
	}
	return 0;
}


void WriteHeader(string file)
{
	ofstream myfile;
	myfile.open(file, ios::out | ios::app); //output and append

	string header = "Piezo Y; Piezo X; Piezo Z; Time";//Piezo 0 and 3 are Y, 1 and 4 are X, 2 and 5 are Z. 3-5 (A0 - A2) are used on the ADC.

	if (myfile.is_open())
	{
		myfile << header;
		myfile.flush();
		myfile.close();
	}
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
		string fileName = "Piezos.txt"; // file name, to be passed as an argument		
		
		pinMode(adc1Input0, INPUT); // Set up ADC as an input
		pinMode(adc1Input1, INPUT);
		pinMode(adc1Input2, INPUT);	
		
		WriteHeader(fileName);

		while (timeNow - timeCut < 1801) //Change to Start Time + 30
		{																	
			double inputValue0 = analogRead(adc1Input0); 
			inputValue0 *= (4.096 / 32768);				

			double inputValue1 = analogRead(adc1Input1); 
			inputValue1 *= (4.096 / 32768);	
				
			double inputValue2 = analogRead(adc1Input2); 
			inputValue2 *= (4.096 / 32768);
			
			ofstream PiezoFile;
			PiezoFile.open(fileName, ios::out | ios::app | ios::binary); //output and append

			if (PiezoFile.is_open())
				{
					time_t timeNow_ = time(0);
					char* systime_ = ctime(&timeNow_);
					PiezoFile << inputValue0 << ";" << inputValue1 << ";" << inputValue2 << ";" << systime_;
					PiezoFile.flush();
					PiezoFile.close();			
				}					
				delay(5);		
				timeNow = time(0);
					
				}
	system("sudo shutdown now");
    return 0;
}
