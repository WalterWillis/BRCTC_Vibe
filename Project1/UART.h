#pragma once
#include "stdafx.h"

using namespace std;
class UART
{
private:
	int fd;
	const string semi = ";";
public:
	bool isValid = false;
	UART();
	void SendData(list<list<double>> dataList);
};

