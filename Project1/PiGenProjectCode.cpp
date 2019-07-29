// PiGenProjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h> //Library for system functions
#include "PiGenProjectCode.h"
#include <sstream>


using namespace std;

//// Globals

//const string newline = "\n";

// Numerical
static int fileIncrementer = 0;
const int elementSize = 13;
//The SD card should be writing in 4KB chunks. Divide that into the expected amount of data to get the array size required for optimal 4 KB writes.
const int arraySize = 4096 / (elementSize * 2 + 24); //45;// (5100 dps);//290 (4500 dps);
long timeCounter = 0;
const long timeChange = arraySize * 20; //get the time less often. uses RTC less, thus less latency

// Time
time_t timeNow = time(0);
char* systime = ctime(&timeNow);
int UART;


// Strings
static string MDir = "/home/pi/Desktop/Vibe2019Cpp/";
string file = MDir + "Test " + to_string(fileIncrementer) + ".txt";
string MFile = MDir + "Master_Program_Data.txt";
const char semi = ';';
const string newline = "\n";


int Startup() {
	// Data Stream
	ofstream data_;

	//Create Master File
	timeNow = time(0);
	systime = ctime(&timeNow);

	struct tm* now = localtime(&timeNow);

	char fileTimeBuffer[80];
	strftime(fileTimeBuffer, 80, "%Y-%m-%d-%H-%M-%S/", now);

	std::stringstream ss;
	ss << fileTimeBuffer;
	MDir = MDir + ss.str();
	file = MDir + "Page" + to_string(fileIncrementer) + ".txt";

	system(("mkdir -p " + MDir).c_str()); //casts string to c string on all entries within parentesis


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


		///interestingly, file descriptor as 0 is cout.
		///so, forgetting to set UART defaults to cout
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tUART Setup Started..." << systime << endl;
		UART = serialOpen("/dev/serial0", 57600);
		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "\tUART Setup Complete..." << systime << endl;

		timeNow = time(0);
		systime = ctime(&timeNow);
		data_ << "System Device Setup Complete..." << systime << endl;

		data_.flush();
		data_.close();
	}
	return 0;
}



void DataHandler(double values[arraySize][elementSize], string times[arraySize]) { //simulates writing to the SD card

	ofstream myfile;
	
	myfile.open(file, ios::out | ios::app);
	
	//if (timeCounter++ >= timeChange) {
		/*timeNow = time(NULL);
		systime = ctime(&timeNow);*/
		//timeCounter = 0;
	//}

	for (int i = 0; i < arraySize; i++) {
		myfile << values[i][0] << semi << values[i][1] << semi << values[i][2] << semi << values[i][3] << semi
			<< values[i][4] << semi << values[i][5] << semi << values[i][6] << semi << values[i][7] << semi
			<< values[i][8] << semi << values[i][9] << semi << values[i][10] << semi 
			<< values[i][11] << semi << values[i][12] << semi << times[i] << semi << newline;
	}

	timeNow = time(NULL);
	systime = ctime(&timeNow);	

	myfile.flush();
	myfile.close();
}

void Telemetry(double values[arraySize][elementSize], string times[arraySize]) {
	//char x[arraySize][(elementSize * 2)];
	//memcpy(&values, x, sizeof(short)); //convert to char array

	string s = "";
	for (int i = 0; i < arraySize; i++) {
		for (int j = 0; j < elementSize; j++) {
			s += to_string(values[i][j]) + semi;
		}
		s += times[i]+ semi;
		s += newline;
	}
	for (int i = 0; i < arraySize; i++) {
		serialPrintf(UART, s.c_str());
	}
}

string GetTime() {
	time_t rawtime;
	struct tm* timeStruct;
	char fileTimeBuffer[80];

	time(&rawtime);
	timeStruct = localtime(&rawtime);
	strftime(fileTimeBuffer, 80, "%X", timeStruct);
	return fileTimeBuffer;
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
		const int fileLines = 10000000;

		int fileLineCount = 0;

		thread t;
		thread t_uArt;
		double values[arraySize][elementSize];
		string times[arraySize];
		
		cout << "starting loop" << endl;
		while (true) {
			try {
				
				for (int i = 0; i < arraySize; i++) {
					for (int pin = 0; pin < ADC_Pins; pin++)
					{
						values[i][pin] = ADC.readADCChannel(pin) / (double)4095 * ADC.vref;
					}		
					Gyro.burstRead(values[i], ADC_Pins);	

					times[i] = GetTime();
				}

				if (t.joinable()) {
					t.join();					
				}

				if (fileLineCount >= fileLines) { // do this after the join due to fileIncrementer being global
					fileLineCount = 0;
					fileIncrementer++;
					file = MDir + "Page" + to_string(fileIncrementer) + ".txt";
				}
				t = thread(DataHandler, values, times);

				if (!t_uArt.joinable()) {// UART is very slow. just let it finish on its own time.
					t_uArt = thread(Telemetry, values, times);
				}
				fileLineCount += arraySize;
			}
			catch (const exception& e) {
				ofstream log;
				timeNow = time(0);
				systime = ctime(&timeNow);
				log.open(MFile);
				log << systime << ": Error in loop!" << endl;
				log << e.what();
				log.flush();
				log.close();
			}
		}
	}
	catch (const exception& e) {
		ofstream log;
		timeNow = time(0);
		systime = ctime(&timeNow);
		log.open(MFile);
		log << systime << ": Fatal Error!" << endl;
		log << e.what();
		log.flush();
		log.close();
	}
}

