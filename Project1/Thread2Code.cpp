#include "stdafx.h"

using namespace std;

const int HarvesterSelect[2] = { 15, 16 }; //Harvester D0 and D1
const int A0 = 100; // Connect common (Z) to A0 (analog input)
const int EN0 = 0; //Harvester EN/PGOOD pin
const int TRANS0 = 1; //Transistor pin

const int A1 = 101; // Connect common (Z) to A0 (analog input)
const int EN1 = 2; //Harvester EN/PGOOD pin
const int TRANS1 = 4; //Transistor pin

const int A2 = 102; // Connect common (Z) to A0 (analog input)
const int EN2 = 3; //Harvester EN/PGOOD pin
const int TRANS2 = 5; //Transistor pin

					  /*NOTE:
					  * We can change the output voltage of the harvester without harming the device, according to the datasheet.
					  * This means I can potentially lower the threshold during an event to continually monitor the device for more than a few microseconds.
					  *However, PGOOD may send out at the voltage  being used and may be too low for the pi to read.
					  *
					  * 2.5 V:
					  *	pinMode(harvesterSelect[0], OUTPUT);
					  *  digitalWrite(harvesterSelect[0], HIGH);
					  *  pinMode(harvesterSelect[1], OUTPUT);
					  *  digitalWrite(harvesterSelect[1], LOW);
					  *
					  * 1.8 V:
					  *	pinMode(harvesterSelect[0], OUTPUT);
					  *  digitalWrite(harvesterSelect[0], LOW);
					  *  pinMode(harvesterSelect[1], OUTPUT);
					  *  digitalWrite(harvesterSelect[1], LOW);
					  */

void *HarvestorDataCollector(void*);

void WriteToFile(string file, int signal, double value) {
	ofstream myfile;
	myfile.open(file, ios::out | ios::app);

	if (myfile.is_open())
	{
		time_t timeNow = time(0);
		char* systime = ctime(&timeNow);
		myfile << "Pgood" << signal << ";" << value << ";" << systime << endl;
		myfile.flush();
		myfile.close();
	}
}

void *HarvestorDataCollector(void *unused)
{
	wiringPiSetup();
	string fileName = "Harvestor.txt";
	pinMode(A0, INPUT);
	pinMode(EN0, INPUT);
	pinMode(TRANS0, OUTPUT);

	pinMode(A1, INPUT);
	pinMode(EN1, INPUT);
	pinMode(TRANS1, OUTPUT);

	pinMode(A2, INPUT);
	pinMode(EN2, INPUT);
	pinMode(TRANS2, OUTPUT);

	// Set D0 and D1 to 3.3 V
	pinMode(HarvesterSelect[0], OUTPUT);
	digitalWrite(HarvesterSelect[0], LOW);
	pinMode(HarvesterSelect[1], OUTPUT);
	digitalWrite(HarvesterSelect[1], HIGH);
	time_t timeNow = time(0);
	time_t timeCut = time(0);

	while (timeNow - timeCut < 1800)
	{
		if (digitalRead(EN0) == HIGH) {
			if (digitalRead(TRANS0) == LOW) {
				digitalWrite(TRANS0, HIGH); // Turn on the transistor
			}
			double inputValue = analogRead(A0); // and read Z
			inputValue *= (4.096 / 32768);
			WriteToFile(fileName, 0, inputValue);
		}
		else if (digitalRead(EN0) == LOW && digitalRead(TRANS0) == HIGH) {
			digitalWrite(TRANS0, LOW); // Turn off the transistor
		}

		if (digitalRead(EN1) == HIGH) {
			if (digitalRead(TRANS1) == LOW) {
				digitalWrite(TRANS1, HIGH); // Turn on the transistor
			}
			double inputValue = analogRead(A1); // and read Z
			inputValue *= (4.096 / 32768);
			WriteToFile(fileName, 1, inputValue);
		}
		else if (digitalRead(EN1) == LOW && digitalRead(TRANS1) == HIGH) {
			digitalWrite(TRANS1, LOW); // Turn off the transistor
		}

		if (digitalRead(EN2) == HIGH) {
			if (digitalRead(TRANS2) == LOW) {
				digitalWrite(TRANS2, HIGH); // Turn on the transistor
			}
			double inputValue = analogRead(A2); // and read Z
			inputValue *= (4.096 / 32768);
			WriteToFile(fileName, 2, inputValue);
		}
		else if (digitalRead(EN2) == LOW && digitalRead(TRANS2) == HIGH) {
			digitalWrite(TRANS2, LOW); // Turn off the transistor
		}
		timeNow = time(0);
	}
	pthread_exit(NULL);
}