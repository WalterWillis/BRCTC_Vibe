#include "RTC.h"
#include "stdafx.h"

RTC::RTC() {
	_fd = wiringPiI2CSetup(DS3231_I2C_ADDR);
}

RTC::ts RTC::GetCurrentTime()
{
	int8_t readBuffer[7];

	//wiringPiI2CWriteReg8(_fd, DS3231_TIME_CAL_ADDR, DS3231_TIME_CAL_ADDR); //initiate communication
	//int8_t valid = read(_fd, readBuffer, 7); // uses the uinstd read function to use the file descriptor to fill the buffer.

	wiringPiI2CReadReg8(_fd, DS3231_TIME_CAL_ADDR); //initiate communication
	for (int i = 0; i < 7; i++) {
		readBuffer[i] = wiringPiI2CRead(_fd);
	}
	return ConvertByteBufferToDateTime(readBuffer);
}

double RTC::GetCurrentSeconds() {
	RTC::ts time = GetCurrentTime();
	return time.sec + (time.min * 60) + (time.hour * 3600);
}

int8_t RTC::IntegerToBinaryCodedDecimal(int8_t value)
{
	int8_t multipleOfOne = value % 10;
	int8_t multipleOfTen = value / 10;

	// convert to nibbles
	int8_t lowerNibble = multipleOfOne;
	int8_t upperNibble = multipleOfTen << 4;

	return lowerNibble + upperNibble;
}

int8_t RTC::BinaryCodedDecimalToInteger(int8_t value)
{
	int8_t lowerNibble = value & 0x0F;
	int8_t upperNibble = value >> 4;

	int8_t multipleOfOne = lowerNibble;
	int8_t multipleOfTen = upperNibble * 10;

	return multipleOfOne + multipleOfTen;
}

int16_t * RTC::ConvertTimeToByteArray(tm *dateTime)
{
	int16_t dateTimeByteArray[8];
	dateTimeByteArray[0] = 0;
	dateTimeByteArray[1] = IntegerToBinaryCodedDecimal(dateTime->tm_sec);
	dateTimeByteArray[2] = IntegerToBinaryCodedDecimal(dateTime->tm_min);
	dateTimeByteArray[3] = IntegerToBinaryCodedDecimal(dateTime->tm_hour);
	dateTimeByteArray[4] = IntegerToBinaryCodedDecimal(dateTime->tm_wday);
	dateTimeByteArray[5] = IntegerToBinaryCodedDecimal(dateTime->tm_mday);
	dateTimeByteArray[6] = IntegerToBinaryCodedDecimal(dateTime->tm_mon);
	dateTimeByteArray[7] = IntegerToBinaryCodedDecimal(dateTime->tm_year - 2000);

	return dateTimeByteArray;
}

RTC::ts RTC::ConvertByteBufferToDateTime(int8_t * dateTimeBuffer)
{
	//tm date;	
	/*date.tm_sec = BinaryCodedDecimalToInteger(dateTimeBuffer[0]);
	date.tm_min = BinaryCodedDecimalToInteger(dateTimeBuffer[1]);
	date.tm_hour = BinaryCodedDecimalToInteger(dateTimeBuffer[2]);
	date.tm_wday = BinaryCodedDecimalToInteger(dateTimeBuffer[3]);
	date.tm_mday = BinaryCodedDecimalToInteger(dateTimeBuffer[4]);
	date.tm_mon = BinaryCodedDecimalToInteger(dateTimeBuffer[5]);
	date.tm_year = 2000 + BinaryCodedDecimalToInteger(dateTimeBuffer[6]);*/

	ts date;
	date.sec = BinaryCodedDecimalToInteger(dateTimeBuffer[0]);
	date.min = BinaryCodedDecimalToInteger(dateTimeBuffer[1]);
	date.hour = BinaryCodedDecimalToInteger(dateTimeBuffer[2]);
	date.wday = BinaryCodedDecimalToInteger(dateTimeBuffer[3]);
	date.mday = BinaryCodedDecimalToInteger(dateTimeBuffer[4]);
	date.mon = BinaryCodedDecimalToInteger(dateTimeBuffer[5]);
	date.year = 2000 + BinaryCodedDecimalToInteger(dateTimeBuffer[6]); 

	return date;
}
