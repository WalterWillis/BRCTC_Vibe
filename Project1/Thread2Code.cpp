#include "stdafx.h"

using namespace std;


const int A0 = 100; 
const int A1 = 101; 
const int A2 = 102; 



void *PiezoDataCollector(void*);

void WriteHeader(string file)
{
	ofstream myfile;
	myfile.open(file, ios::out | ios::app); //output and append

	string header = "Piezo X; Piezo Y; Piezo Z; Time";

	if (myfile.is_open())
	{
		myfile << header;
		myfile.flush();
		myfile.close();
	}
}

void WriteToFile(string file, double *value) {
	ofstream myfile;
	myfile.open(file, ios::out | ios::app);

	if (myfile.is_open())
	{
		time_t timeNow = time(0);
		char* systime = ctime(&timeNow);
		myfile << *value << ";" << *(value + 1) << ";" << *(value + 2) << ";" << systime << endl;
		myfile.flush();
		myfile.close();
	}
}

void *PiezoDataCollector(void *unused)
{
	wiringPiSetup();
	string fileName = "Piezos.txt";
	WriteHeader(fileName);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);

	time_t timeNow = time(0);
	time_t timeCut = time(0);

	while (timeNow - timeCut < 1800)
	{
		double inputValue[3];
		inputValue[0] = analogRead(A0);
		inputValue[0] *= (2.048 / 32768);		

		inputValue[1] = analogRead(A1);
		inputValue[1] *= (2.048 / 32768);

		inputValue[2] = analogRead(A2); 
		inputValue[2] *= (2.048 / 32768);

		WriteToFile(fileName, inputValue);

		timeNow = time(0);
	}
	pthread_exit(NULL);
}