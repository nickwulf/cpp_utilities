#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "Dec.h"

class Digit {
public:
	Digit();
	Digit(long long integer);
	Digit(const Dec& d);
	Digit(const Digit& other);
	~Digit();
	Digit& operator=(long long integer);
	Digit& operator=(const Dec& d);
	Digit& operator=(const Digit& other);
	Digit& operator+=(const Digit& other);
	void swap(int a, int b);
	int calcSum();
	long long genValueL();
	Dec genValueD();
	int getDigitNum()const;
	char getDigit(int index)const;
	bool hasDigit(int testValue)const;

private:
	int digitMax;
	int digitNum;
	char* digits;
	bool contents[10];
};

inline int Digit::getDigitNum()const {
	return digitNum;
}

inline char Digit::getDigit(int index)const {
	return digits[index];
}

inline bool Digit::hasDigit(int testValue)const {
	return contents[testValue];
}
