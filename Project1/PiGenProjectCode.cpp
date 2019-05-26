// PiGenProjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Thread1Code.h" //Code for First thread
//#include "Thread2Code.h" //Code for Second Thread
#include <stdlib.h> //Library for system functions
#include "PiGenProjectCode.h"
#include "RTC.h"

using namespace std;

//// Globals

// Strings
static const string MDir = "/home/pi/Desktop/Vibe2019Cpp/";
string MFile = MDir + "Master_Program_Data.txt";

// Ints
static int fileIncrementer = 0;
const int arraySize = 300;

int Startup() {
	// Data Stream
	ofstream data_;

	//Create Master File
	time_t timeNow = time(0);
	char* systime = ctime(&timeNow);
	data_.open(MFile);

	system("sudo rmmod rtc_ds1307");

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

		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "System Device Setup Complete..." << systime << endl;

		data_.flush();
		data_.close();
	}
	return 0;
}



void SendList(double adcValues[arraySize][4], double gyroValues[arraySize][11]) { //simulates writing to the SD card

	ofstream myfile;
	string file = MDir + "Test " + to_string(fileIncrementer) + ".txt";


	cout << "Wrinting to file: " << file << endl;
	myfile.open(file, ios::out | ios::app);

	const char semi = ';';

	for (int i = 0; i < arraySize; i++) {
		myfile << adcValues[i][0] << semi << adcValues[i][1] << semi << adcValues[i][2] << semi << adcValues[i][3] << semi
			<< gyroValues[i][0] << semi << gyroValues[i][1] << semi << gyroValues[i][2] << semi << gyroValues[i][3] << semi
			<< gyroValues[i][4] << semi << gyroValues[i][5] << semi << gyroValues[i][6] << semi << gyroValues[i][7] << semi
			<< gyroValues[i][8] << semi << gyroValues[i][9] << semi << gyroValues[i][10] << semi << endl;
	}

	myfile.flush();
	myfile.close();

	cout << "file write successful";
}


void GyroHeader(string file)
{
	
	ofstream gyroFile;
	gyroFile.open(file, ios::out | ios::app); //output and append

	string header = "Diagnostic; GyroX; GyroY; GyroZ; AccelX; AccelY; AccelZ; Temp; SampleCounter; ChecksumRead; ChecksumCalculated";
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

int main()
{
	try {
		//int rc = Startup();
		Startup();

				////ADC SETUP
		PiHat ADC;
		ADIS16460 Gyro;
		RTC RTC;

		double start_time = RTC.GetCurrentSeconds();

		const int ADC_Pins = 3;
		const int fileLines = 1200000;
		const char semi = ';';

		double adcValues[arraySize][4];
		double gyroValues[arraySize][11];

		double copy_adcValues[arraySize][4];
		double copy_gyroValues[arraySize][11];
		int fileLineCount = 0;

		thread t;

		cout << "starting loop"<< endl;
		while (true) { // the t thread should wait long enough for the next 5 loop cycles without any errors
			try {
				for (int i = 0; i < arraySize; i++) {//get lots of data. In reality, the outer loop would be where there are many iterations with the inner loop collecting data
					//maybe just use rtc day, hour, min and sec values
					//adcValues[i][3] =RTC.GetCurrentSeconds() - start_time; // get time in seconds since beginning of program
					for (int pin = 0; pin < ADC_Pins; pin++)
					{
						adcValues[i][pin] = ADC.readADCChannel(pin) / (double)4095 * ADC.vref;
					}
					//gyroValues[i][10] = RTC.GetCurrentSeconds() - start_time;
					Gyro.burstRead(gyroValues[i]);
				}

				
/*
				ofstream myfile;
				string file = MDir + "Test " + to_string(fileIncrementer) + ".txt";
				cout << "Wrinting to file: " << file << endl;
				myfile.open(file, ios::out | ios::app);

				cout << adcValues[5][0] << semi << adcValues[5][1] << semi << adcValues[5][2] << semi << adcValues[5][3] << semi
					<< gyroValues[5][0] << semi << gyroValues[5][1] << semi << gyroValues[5][2] << semi << gyroValues[5][3] << semi
					<< gyroValues[5][4] << semi << gyroValues[5][5] << semi << gyroValues[5][6] << semi << gyroValues[5][7] << semi
					<< gyroValues[5][8] << semi << gyroValues[5][9] << semi << gyroValues[5][10] << semi << endl;

				for (int i = 0; i < arraySize; i++) {
					myfile << adcValues[i][0] << semi << adcValues[i][1] << semi << adcValues[i][2] << semi << adcValues[i][3] << semi
						<< gyroValues[i][0] << semi << gyroValues[i][1] << semi << gyroValues[i][2] << semi << gyroValues[i][3] << semi
						<< gyroValues[i][4] << semi << gyroValues[i][5] << semi << gyroValues[i][6] << semi << gyroValues[i][7] << semi
						<< gyroValues[i][8] << semi << gyroValues[i][9] << semi << gyroValues[i][10] << semi << endl;
				}

				myfile.flush();
				myfile.close();
*/

				if (t.joinable()) {
					t.join();
				}

				memcpy(&copy_adcValues[0], &adcValues[0], arraySize * 4 * sizeof copy_adcValues[0][0]);
				memcpy(&copy_gyroValues[0], &gyroValues[0], arraySize * 11 * sizeof copy_gyroValues[0][0]);
				t = thread(SendList, copy_adcValues, copy_gyroValues);

				if (fileLineCount >= fileLines) { // do this after the join due to fileIncrementer being global
					fileLineCount = 0;
					fileIncrementer++;
				}

				fileLineCount += arraySize;

				//cout << "file write successful";
			}
			catch (const exception& e) {
				cout << "Error in loop" << endl;
				cout << e.what();
				throw e;
			}
		}
	}
	catch (const exception& e) {
		cout << "fatal error" << endl << e.what() << endl;
		system("sudo modprobe rtc_ds1307");
	}
}

