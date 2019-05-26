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



void SendList(std::list<std::list<double>> list) { //simulates writing to the SD card
	if (list.size() < 1)
		return;

	ofstream myfile;
	string file = MDir + "Test " + to_string(fileIncrementer) + ".txt";

	cout << "Wrinting to file: " << file << endl;
	myfile.open(file, ios::out | ios::app);

	for (std::list<double> innerlist : list) {
		string line = "";
		for (double value : innerlist) {
			if (myfile.is_open())
			{
				line += to_string(value) + ";";
			}
			cout << line << endl;
			sleep(1);
			myfile << line << endl;
		}
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
		int rc = Startup();



		std::list<double> smallList;
		std::list<std::list<double>> bigList;

		std::thread t;  // ideally, we would have 2 threads. One for local storage, and another for telemetry. Good enough for testing.

				////ADC SETUP
		PiHat ADC;
		ADIS16460 Gyro;
		RTC RTC;

		double start_time = RTC.GetCurrentSeconds();

		const int resultLen = 10;



		const int ADC_Pins = 3;
		const int arraySize = 30000;
		const int fileLines = 1200000;
		int fileLineCount = 0;

		bool firstStart = true;

		cout << "starting loop"<< endl;
		while (true) { // the t thread should wait long enough for the next 5 loop cycles without any errors

			try {

					if (t.joinable()) {
						cout << "joining thread" << endl;
						t.join(); //if the thread is already running, wait until the work is done;
						cout << "finished joining thread" << endl;
					}

					if (fileLineCount >= fileLines) { // do this after the join due to fileIncrementer being global
						fileLineCount = 0;
						fileIncrementer++;
					}

					t = std::thread{ SendList, std::move(bigList) }; // start the thread
					fileLineCount += bigList.size();
					bigList.clear(); // remove previous list values

					

				for (int i = 0; i < arraySize; i++) {//get lots of data. In reality, the outer loop would be where there are many iterations with the inner loop collecting data
					int result;

					//maybe just use rtc day, hour, min and sec values
					smallList.push_back(RTC.GetCurrentSeconds() - start_time); // get time in seconds since beginning of program
					for (int pin = 0; pin < ADC_Pins; pin++)
					{
						result = ADC.readADCChannel(pin);
						smallList.push_back((double)result / (double)4095 * ADC.vref);
					}
					double results[resultLen];
					smallList.push_back(RTC.GetCurrentSeconds() - start_time);
					Gyro.burstRead(results);

					smallList.insert(smallList.end(), results, results + resultLen); //so, this is a thing

				}
				bigList.push_back(smallList); // add value to the back of the list
				smallList.clear();
			}
			catch (const exception& e) {
				cout << "Error in loop" << endl;
				cout << e.what();
				throw e;
			}
		}
		if (t.joinable())
			t.join(); // wait for the thread to finish
		t.detach();
	}
	catch (const exception& e) {
		cout << "fatal error" << endl << e.what() << endl;
		system("sudo modprobe rtc_ds1307");
	}
}

