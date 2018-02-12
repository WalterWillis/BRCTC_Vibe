// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS 0

//#include "targetver.h"
#include "wiringPi.h" //Wiring Pi Library
#include "ads1115.h" // Library for the ADS1115 ADC
//#include "wiringPiSPI.h"
#include <stdio.h>
//#include <tchar.h>
#include <pthread.h> //Library for Threading
#include <iostream> //Standard C++ Library for Input / Output Stream
#include <fstream> //Library for Working with .txt Files
#include <cstdint> //Standard C Int Library
#include <string> //Int to String Conversion Library
#include <ctime> //Standard C++ Time Library
#include "ADIS16460.h"

// TODO: reference additional headers your program requires here