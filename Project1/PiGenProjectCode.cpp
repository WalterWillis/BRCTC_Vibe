// PiGenProjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h> //Library for system functions
#include "PiGenProjectCode.h"


using namespace std;

//// Globals

// Strings
static const string MDir = "/home/pi/Desktop/Vibe2019Cpp/";
string MFile = MDir + "Master_Program_Data.txt";

// Ints
static int fileIncrementer = 0;
const int arraySize = 30000;
const int elementSize = 12; //actual size is 13



int Startup() {
	// Data Stream
	ofstream data_;

	//Create Master File
	time_t timeNow = time(0);
	char* systime = ctime(&timeNow);
	data_.open(MFile);

	//system("sudo rmmod rtc_ds1307");
	try {
		system("sudo modprobe rtc_ds1307"); // we should be able to use the system time to call the rtc
	}
	catch (exception e) {
		cout << e.what();
	}
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



void SendList(forward_list<int> list) { //simulates writing to the SD card

	ofstream myfile;
	string file = MDir + "Test " + to_string(fileIncrementer) + ".txt";

	//cout << "Wrinting to file: " << file << endl;
	myfile.open(file, ios::out | ios::app);

	const char semi = ';';
	time_t timeNow = time(0);
	char* systime = ctime(&timeNow);

	int counter = 0;
	for (auto const& i : list) {
		myfile << i << semi;
		if (counter++ == elementSize) {
			myfile << systime;
			counter = 0;
		}
	}
	myfile.flush();
	myfile.close();

	list.clear();

	//cout << "file write successful";
}

int main()
{
	try {
		//int rc = Startup();
		Startup();

		////ADC SETUP
		PiHat ADC;
		ADIS16460 Gyro;

		const int ADC_Pins = 3;
		const int fileLines = 1200000;

		forward_list<int> masterList;

		int fileLineCount = 0;

		thread t;

		cout << "starting loop" << endl;
		while (true) {
			try {
				for (int i = 0; i < arraySize; i++) {
					
					for (int pin = 0; pin < ADC_Pins; pin++)
					{
						masterList.emplace_front(ADC.readADCChannel(pin));// / (double)4095 * ADC.vref;
					}
					short array[10];
					Gyro.burstRead(array);
					
					for (short d : array) {
						masterList.emplace_front(d);
					}					
				}


				if (t.joinable()) {
					t.join();					
				}
			
				if (fileLineCount >= fileLines) { // do this after the join due to fileIncrementer being global
					fileLineCount = 0;
					fileIncrementer++;
				}
				t = thread(bind( SendList, masterList));
				masterList.clear();
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

