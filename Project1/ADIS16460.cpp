/*
ADIS16460.cpp
Library for BRCTC Space Team's Gyroscope.
Walter Willis -- Lead Programmer

This code was created for an Arduino system taking part in the 2016 RockSat program.

In this file are the functions for the gyroscope.

RESOURCES:
Steven Hard shared an example file with me called IMU.ino. Most of the commands and logic of this class file were borrowed from this code.

This code is free to use. Distributed as-is; no warranty is given.

NOTES: using WiringPi delayMicroseconds() with less than 100 microseconds can cause hangs when using multiple threads

*/
#include "stdafx.h"
//#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "ADIS16460.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>


ADIS16460::ADIS16460()
{
	(wiringPiSPISetupMode (_channel, _speed, _mode) < 0);
}

ADIS16460::ADIS16460(int channel, int speed, int mode)
{
	(wiringPiSPISetupMode(channel, speed, mode) < 0);
}

void ADIS16460::ClearBuffer(void)
{
	for (int i = 0; i < 18; i += 2) //Don't count reserved registers 6 and 8
	{
		if(i == 6 || i == 8)
		{//do nothing
		}
		else
		{
			unsigned char C = 0x00;
			buffer[i+1] = C; // Fills the element above i with a zero byte
			C += (2 * i);
			buffer[i] = C;	 // Sets the LSB address of the	target register
		}
	}
	
	buffer[0] = (BURST_READ>>8) & 0xff;
	buffer[1] = BURST_READ & 0xff;
}

double ADIS16460::twoscomptransform(unsigned char nbits, unsigned int num)
{	
	unsigned int mask;//, padding;
    // select correct mask
    mask = 1 << (nbits -1);
  
    // if MSB is 1, then number is negative, so invert it and add one
    // if MSB is 0, then just return the number 
    return (num & mask)?( -1.0 * (~(num | 0xFF << nbits)  + 1) ):( 1.0 * num );
}

// Joins the bytes of data together. Pass array values.
void JoinData(unsigned char *data, int16_t *words, int length) 
{
	length = length / 2;
	for (int i = 0; i < length; i++)
	{
		int index = i * 2;
		words[i] = ((data[index] << 8) | (data[index] & 0xFF));
	}
}

void ADIS16460::GetADISReadings(void) 
{
  
  // I believe the format is: Set the buffer to contain the bits required, and then run the SPI RW function with
  // the channel, the buffer array with the commands to write to the register, 
  // and the amount of bits that will be first writen, and then replaced by the read bits.
  // wiringPiSPIDataRW(channel, BufferContainingRegisterInfo, BytesToBeWritten)
	int length = 20;
	unsigned char burstData[length];
	int16_t joinedData[length / 2];

	for (int i = 0; i < length; i++)
	{
		  burstData[i] = 0x00;
	}

  //A check can be drawn here to verify that wiringPiSPIDataRW >= 0. Anything above 0 should be okay.
  
  wiringPiSPIDataRW(channel, burstData, length); // Passes the char array into the function, along with the element length of the buffer
  delayMicroseconds(40);
  
  //Every two elements contain two bytes of info
  //This info needs to be combined to create the single 16 bit (word) answer from the device
  Diagnostic = SetData(SUPPLYBITS, buffer[0], buffer[1])* SUPPLYSCALE;
  
  GyroX = SetData(GYROBITS, buffer[2], buffer[3]);
  GyroX = twoscomptransform(GYROBITS, GyroX);
  GyroX *= GYROSCALE;
  
  GyroY = SetData(ACCBITS, buffer[4], buffer[5]);
  GyroY = twoscomptransform(ACCBITS, GyroY);
  GyroY *= ACCSCALE;
  
  GyroZ = SetData(ACCBITS, buffer[6], buffer[7]);
  //GyroZ = twoscomptransform(ACCBITS, GyroZ);
  GyroX *= ACCSCALE;
  
  AccelX = SetData(ACCBITS, buffer[8], buffer[9]);
  AccelX = twoscomptransform(ACCBITS, AccelX);
  AccelX *= ACCSCALE;
  
  AccelY = SetData(TEMPBITS, buffer[10], buffer[11]);
  //AccelY = twoscomptransform(TEMPBITS, AccelY);
  AccelY *= TEMPSCALE;
  
  AccelZ = SetData(PITCHBITS, buffer[12], buffer[13]);
  AccelZ = twoscomptransform(PITCHBITS, AccelZ);
  AccelZ *= PITCHSCALE;
  
  Temp = SetData(ROLLBITS, buffer[14], buffer[15]);
  Temp = twoscomptransform(ROLLBITS, Temp);
  Temp *= ROLLSCALE;
  
  SampleCounter = SetData(AUXBITS, buffer[16], buffer[17]); // should be between 0 and 3.3 V -- refer to the datasheet to determine the voltage using the decimal representation

}

// Takes the upper and lower bytes from the array and combines them into an int value
unsigned int ADIS16460::SetData(unsigned char nbits, unsigned char upper, unsigned char lower)
{
	unsigned char mask;	  
	  
	mask = 0xFF >> (16 - nbits); //for 14 bit transfers, shifts the 0xFF byte by two: one for the ND (New Data) bit, one for the EA (Eror/Alarm) bit.
	  
	return   ( ( upper & mask) << 8 | lower)  ; // ANDs the two bytes consisting of 7 bits of data together, and then ORs them to zero out the 1s from the mask.
}

//Used to get data from register
int16_t ADIS16460::RegRead(uint8_t regAddr)
{
	unsigned char Data[] = { regAddr, 0x00 };
	wiringPiSPIDataRW(channel, Data, 2);

	delayMicroseconds(40);

	int16_t dataOut = (Data[0] << 8) | (Data[1] & 0xFF);

	return dataOut;
}

//Used to write to registers
int ADIS16460::RegWrite(uint8_t regAddr, int16_t regData) 
{
	// Write register address and data
	uint16_t addr = (((regAddr & 0x7F) | 0x80) << 8);
	uint16_t lowWord = (addr | (regData & 0xFF));
	uint16_t highWord = ((addr | 0x100) | ((regData >> 8) & 0xFF));

	//Split words into chars and place into char array
	unsigned char Data[4];
	Data[0] = (highWord >> 8);
	Data[1] = (highWord & 0xFF);
	Data[2] = (lowWord >> 8);
	Data[3] = (lowWord & 0xFF);

	//Write to SPI bus
	wiringPiSPIDataRW(channel, Data, 4);

	return 1;
}
