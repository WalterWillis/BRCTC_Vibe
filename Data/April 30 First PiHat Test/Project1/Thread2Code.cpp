#include "stdafx.h"

using namespace std;


const int A0 = 100; 
const int A1 = 101; 
const int A2 = 102; 
const int length = 30000;



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

void *PiezoDataCollector(void *unused)
{
	wiringPiSetup();
	string fileName = "Piezos.txt";
	WriteHeader(fileName);
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);

	time_t timeNow = time(0); // updates after every succesful loop
	time_t timeCut = time(0); // always stays as time that program started

	while (timeNow - timeCut < 1800) //1800 seconds = 30 min
	{

		/*
		TODO
		Instead of writing down tons of data, get the min, max and average of the data over a period of time and see how that looks.
		
		
		*/
		double inputValue[length][3];
		time_t timeData[length];

		for (int i = 0; i < length; i++) {
			
			inputValue[i][0] = analogRead(A0);
			inputValue[i][0] *= (2.048 / 32768);

			inputValue[i][1] = analogRead(A1);
			inputValue[i][1] *= (2.048 / 32768);

			inputValue[i][2] = analogRead(A2);
			inputValue[i][2] *= (2.048 / 32768);

			timeData[i] = time(0) - timeCut; // get time in seconds since beginning of program
		}

		ofstream myfile;
		myfile.open(fileName, ios::out | ios::app);

		for (int i = 0; i < length; i++) {
			if (myfile.is_open())
			{
				myfile << inputValue[i][0] << ";" << inputValue[i][1] << ";" << inputValue[i][2] << ";" << timeData[i] << endl;
			}
		}

		myfile.flush();
		myfile.close();

		timeNow = time(0);
	}
	pthread_exit(NULL);
}